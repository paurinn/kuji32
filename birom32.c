/*
Kuji32 Flash MCU Programmer
Copyright (C) 2014 Kari Sigurjonsson

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
@addtogroup birom32
@{
*/
#include "stdafx.h"

int birom32_new(struct birom32_state **state, struct chipdef32 *chip, struct serial *serial) {
	char path[256];
	int rc;

	assert(state);

	*state = calloc(1, sizeof(struct birom32_state));
	assert(*state);

	(*state)->chip = chip;
	(*state)->serial = serial;

	LOGD("User says MCU is '%s' on '%s'.", mcu32_name(chip->mcu), serial->address);

	memset(path, 0x00, sizeof(path));
	snprintf(path, sizeof(path) - 1, "kernal32/%s", chip->kernal);

	(*state)->kernalsize = filedata(path, &(*state)->kernaldata);
	if ((*state)->kernalsize <= 0 || (*state)->kernaldata == NULL) {
		LOGE("Could not read contents of '%s'.", path);
		return E_READ;
	}

	LOGD("Loaded '%s' (0x%02X bytes).", path, (*state)->kernalsize);

	return E_NONE;
}

void birom32_free(struct birom32_state **state) {
	if (state && *state) {
		if ((*state)->kernaldata) {
			free((*state)->kernaldata);
		}
		free(*state);
		*state = NULL;
	}
}

int birom32_connect(struct birom32_state *state, int timeoutsec) {
	int rc;
	uint8_t buf[2];
	int tries;

	serial_purge(state->serial);

	memset(&buf, 0x00, sizeof(buf));

	if (timeoutsec < 1 || timeoutsec > 60) timeoutsec = 5;

	//First handshake is to give user chance to power up MCU.
	double timeout = get_ticks() + timeoutsec;
	while (get_ticks() < timeout) {
		buf[0] = BIROM32_CMD_PROBE;
		rc = serial_write(state->serial, buf, 1);
		if (rc < 1) {
			LOGE("Error writing to serial port! Aborting.");
			return rc;
		}

		serial_drain(state->serial);

		buf[0] = 0;
		rc = serial_read(state->serial, buf, 1);
		if (rc < 0) {
			LOGE("Error reading from serial port! Aborting.");
			return rc;
		}

		if (buf[0] == BIROM32_RESP_PROBE) {
			LOGD("OK, MCU found.");
			return E_NONE;
		}

		msleep(50);
	}

	LOGE("TIME-OUT");
	return E_TIMEOUT;
}

int birom32_check(struct birom32_state *state) {
	uint8_t buf[2];
	int rc;

	serial_purge(state->serial);

	msleep(10);

	buf[0] = BIROM32_CMD_CHECK;
	rc = serial_write(state->serial, buf, 1);
	if (rc < 1) {
		LOGE("Error writing to serial port! Aborting.");
		return rc;
	}

	serial_drain(state->serial);

	memset(&buf, 0x00, sizeof(buf));

	double timeout = get_ticks() + 1;
	while (get_ticks() < timeout) {
		rc = serial_read(state->serial, buf, 1);
		if (rc < 0) {
			LOGE("Error reading from serial port! Aborting.");
			return rc;
		}
		if (rc == 0) continue;
		if (buf[0] == BIROM32_RESP_CHECK) break;
	}

	if (buf[0] != BIROM32_RESP_CHECK) {
		LOGE("Malformed response from MCU (0x%X). Please power off board and try again.", buf[0]);
		return E_TIMEOUT;
	}

	return E_NONE;
}

int birom32_write(struct birom32_state *state, uint32_t address, uint8_t *data, uint32_t size) {
	int rc;
	uint8_t buffer[4096];
	memset(buffer, 0x00, sizeof(buffer));

	serial_purge(state->serial);
	msleep(10);

	buffer[0] = BIROM32_CMD_WRITE;
	buffer[1] = (address & 0x000000FF);
	buffer[2] = (address & 0x0000FF00) >> 8;
	buffer[3] = (address & 0x00FF0000) >> 16;
	buffer[4] = (address & 0xFF000000) >> 24;

	//Write only command + 4 byte address.
	rc = serial_write(state->serial, buffer, 5);
	if (rc < 0 || rc < 5) {
		LOGE("Error writing to MCU.");
		return E_WRITE;
	}

	serial_drain(state->serial);

	//Expect BIROM32_RESP_WRITE as indicator that we can dump the binary down to MCU.
	uint8_t code;
	double timeout = get_ticks() + 2;
	while (get_ticks() < timeout) {
		code = 0;
		if ((rc = serial_read(state->serial, &code, 1)) == 0) {
			continue;
		}
		if (rc < 0) {
			LOGE("Error reading from '%s'.", state->serial->address);
			return E_READ;
		}
		if (code == BIROM32_RESP_WRITE) {
			break;
		}
	}

	if (code != BIROM32_RESP_WRITE) {
		LOGE("MCU did not respond.");
		return E_READ;
	}

	//Send 0x3003 over before 816 bytes of m_flash.362.
	//Is this the checksum?
	buffer[1] = (size >> 8) & 0xFF;
	buffer[0] = (size & 0xFF);

	rc = serial_write(state->serial, buffer, 2);
	if (rc < 0 || rc < 2) {
		LOGE("Error writing to MCU.");
		return E_WRITE;
	}

	serial_drain(state->serial);

	memcpy(buffer, data, size);

	uint16_t csum16 = checksum16(buffer, size);
	LOGD("Writing %d bytes to address 0x%04X. Checksum 0x%X", size, address, csum16);

	rc = serial_write(state->serial, buffer, size);
	if (rc < 0 || rc < (int32_t)size) {
		LOGE("Error writing to MCU.");
		return E_WRITE;
	}

	//Give MCU some breathing space.
	serial_drain(state->serial);
	msleep(250);

	uint8_t buf[2];
	if ((rc = serial_read(state->serial, buf, 2)) < 2) {
		LOGE("Error reading from '%s'.", state->serial->address);
		return E_READ;
	}

	uint16_t csum16chk = 0x0000;
	csum16chk |= buf[0] & 0x00FF;
	csum16chk |= (buf[1] << 8) & 0xFF00;

	if (csum16 != csum16chk) {
		LOGE("Checksum mismatch! 0x%X (PC) != 0x%X (MCU)", csum16, csum16chk);
		return E_CRC;
	}

	return E_NONE;
}

int birom32_call(struct birom32_state *state, uint32_t address) {
	uint8_t buffer[5];

	memset(buffer, 0x00, sizeof(buffer));

	serial_purge(state->serial);
	msleep(10);

	buffer[0] = BIROM32_CMD_CALL;
	buffer[1] = (address & 0x000000FF);
	buffer[2] = (address & 0x0000FF00) >> 8;
	buffer[3] = (address & 0x00FF0000) >> 16;
	buffer[4] = (address & 0xFF000000) >> 24;

	unsigned int rc = serial_write(state->serial, buffer, sizeof(buffer));
	if (rc < sizeof(buffer)) {
		LOGE("Error writing to MCU.");
		return E_WRITE;
	}

	serial_drain(state->serial);

	//Expect first single byte result of operation from MCU.
	rc = serial_read(state->serial, buffer, 1);
	if (buffer[0] != BIROM32_RESP_CALL) {
		LOGE("Malformed response 0x%X from MCU.", buffer[0]);
		return E_MSGMALFORMED;
	}

	msleep(10);
	//And then the final 0x31 response to indicate that stage 2 is running.
	while ((rc = serial_read(state->serial, buffer, 1)) > 0) {
		if (buffer[0] == BIROM32_RESP_CALL_DONE) {
			break;
		}
	}

	if (buffer[0] != BIROM32_RESP_CALL_DONE) {
		LOGE("Error: Did not receive 0x31 after CALL.");
		return E_MSGMALFORMED;
	}

	return E_NONE;
}

/** @} */
