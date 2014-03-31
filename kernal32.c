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
@addtogroup kernal32
@{
*/
#include "stdafx.h"

int kernal32_new(struct kernal32 **state, struct chipdef32 *chip, struct serial *serial) {
	int rc;

	*state = (struct kernal32 *)calloc(1, sizeof(struct kernal32));
	assert(*state);

	(*state)->chip = chip;
	(*state)->serial = serial;

	LOGD("User says MCU is '%s' on %s.", mcu32_name(chip->mcu), serial->address);

	return E_NONE;
}

void kernal32_free(struct kernal32 **state) {
	if (state && *state) {
		free(*state);
		*state = NULL;
	}
}

int kernal32_intro(struct kernal32 *state) {
	uint8_t buf[10];
	int rc;

	memset(buf, 0x00, sizeof(buf));
	msleep(100);

	//serial_drain(state->serial);
	if (serial_read(state->serial, buf, 1) > 0) {
		LOGI("Found 0x%02X from birom");
	}

	double timeout = get_ticks() + 10;
	while (get_ticks() < timeout) {
		msleep(100);
		buf[0] = KERNAL32_CMD_INTRO;
		rc = serial_write(state->serial, buf, 1);
		if (rc < 1) {
			LOGE("Error writing to serial port! Aborting.");
			return rc;
		}

		serial_drain(state->serial);

		rc = serial_read(state->serial, buf, 1);
		if (rc < 0) {
			LOGE("Error reading from serial port! Aborting.");
			return rc;
		}

		if (buf[0] == KERNAL32_RESP_ACK) break;
	}

	if (buf[0] != KERNAL32_RESP_ACK) {
		LOGE("Malformed response from MCU.");
		return E_MSGMALFORMED;
	}

	return E_NONE;
}

int kernal32_blankcheck(struct kernal32 *state, uint32_t flash_base) {
	uint8_t buf[20];
	int rc;

	memset(buf, 0x00, sizeof(buf));

	serial_purge(state->serial);
	msleep(10);

	buf[0] = KERNAL32_CMD_BLANKCHECK;

	//Address & size are big endian.
	buf[3] = (flash_base & 0xFF0000) >> 16;
	buf[2] = (flash_base & 0x00FF00) >> 8;
	buf[1] = (flash_base & 0x0000FF);

	rc = serial_write(state->serial, buf, 4);
	if (rc < 4) {
		LOGE("Could not write to: %s.", state->serial->address);
		return E_WRITE;
	}

	serial_drain(state->serial);

	rc = serial_read(state->serial, buf, 1);

	double timeout = get_ticks() + 1;
	while (rc >= 0 && get_ticks() < timeout) {
		if (rc < 0) {
			LOGE("Read error.");
			return E_READ;
		} else if (rc > 0) {
			if (buf[0] == KERNAL32_RESP_BUSY) {
				LOGD("...MCU busy...");
			} else if (buf[0] == KERNAL32_RESP_ACK) {
				return 1;	//Returning 1 to mean SUCCESS, that is, chip flash is blank.
			} else if (buf[0] == KERNAL32_RESP_ERRBLANK) {
				//Read 4 bytes (address) and 4 bytes (data) and 1 byte KERNAL32_RESP_ERRBLANK again.
				rc = serial_read(state->serial, buf, 9);
				if (rc < 9) {
					LOGE("Error reading address and data from 0x34 response.");
					return E_MSGMALFORMED;
				}
				uint32_t address = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
				uint32_t data = buf[4] << 24 | buf[5] << 16 | buf[6] << 8 | buf[7];
				LOGD("Chip is NOT blank. Address 0x%04X has value 0x%04X.", address, data);

				if (buf[8] != KERNAL32_RESP_ERRBLANK) {
					LOGE("Error: Did not receive final 0x34.");
					return E_MSGMALFORMED;
				}

				return E_NONE;
			} else {
				LOGW("Erroneounus data received %02X", buf[0]);
			}
		} else {
			msleep(1);
		}
		rc = serial_read(state->serial, buf, 1);
	}

	LOGE("ERROR: Time-out waiting for MCU.");

	return E_TIMEOUT;
};

int kernal32_erasechip(struct kernal32 *state, uint32_t flash_base) {
	uint8_t buf[256];
	int rc;

	memset(buf, 0x00, sizeof(buf));

	serial_purge(state->serial);

	buf[0] = KERNAL32_CMD_ERASECHIP;

	//Address & size are big endian.
	buf[3] = (flash_base & 0xFF0000) >> 16;
	buf[2] = (flash_base & 0x00FF00) >> 8;
	buf[1] = (flash_base & 0x0000FF);

	rc = serial_write(state->serial, buf, 4);
	if (rc < 4) {
		LOGE("Could not write to: %s.", state->serial->address);
		return E_WRITE;
	}

	serial_drain(state->serial);

	//Receive busy marker.
	rc = serial_read(state->serial, buf, 1);

	//Receive ACK or NAK.

	double timeout = get_ticks() + 30;
	double ticktimeout = get_ticks() + 2;
	while (get_ticks() < timeout) {
		rc = serial_read(state->serial, buf, 1);
		if (rc < 0) {
			LOGD("Read error.");
			return E_READ;
		} else if (rc > 0) {
			if (buf[0] == KERNAL32_RESP_BUSY) {
				LOGD("...MCU busy...");
				timeout = get_ticks() + 30;
			} else if (buf[0] == KERNAL32_RESP_ACK) {
				return E_NONE;	//Return success.
			} else if (buf[1] == KERNAL32_RESP_NAK) {
				LOGE("ERROR - Chip NOT erased.");
				return E_FULL;
			} else {
				LOGE("Erroneous reply from MCU: ");
				return E_MSGMALFORMED;
			}
		} else if (get_ticks() > ticktimeout) {
			LOGR("#");
			ticktimeout = get_ticks() + 2;
			msleep(100);
		}
	}

	LOGE("ERROR: Time-out waiting for MCU.");

	return E_TIMEOUT;
}

int kernal32_readflash(struct kernal32 *state, uint32_t flash_base, uint8_t *buf, uint32_t size, uint16_t *pcrc) {
	int rc;

	memset(buf, 0xFF, size);

	serial_purge(state->serial);
	msleep(10);

	uint8_t cmd[4];

	cmd[0] = KERNAL32_CMD_READFLASH;

	//Address & size are big endian.
	cmd[3] = (flash_base & 0xFF0000) >> 16;
	cmd[2] = (flash_base & 0x00FF00) >> 8;
	cmd[1] = (flash_base & 0x0000FF);

	rc = serial_write(state->serial, cmd, 4);
	if (rc < 4) {
		LOGE("Could not write to: %s.", state->serial->address);
		return E_WRITE;
	}

	serial_drain(state->serial);

	//Receive 'busy' marker...
	rc = serial_read(state->serial, buf, 1);
	if (rc < 1 || buf[0] != KERNAL32_RESP_BUSY) {
		LOGE("ERROR: Did not receive busy marker after READ command.");
		return rc < 0 ? E_READ : E_MSGMALFORMED;
	}

	//...and ACK marker.
	rc = serial_read(state->serial, buf, 1);
	if (rc < 1 || buf[0] != KERNAL32_RESP_ACK) {
		LOGE("ERROR: Did not receive acknowledge to READ command.");
		return rc < 0 ? E_READ : E_MSGMALFORMED;
	}

	int i = 0;
	int retry = 30;
	while (i < 0 || i < (int32_t)size) {
		rc = serial_read(state->serial, buf + i, size - i);
		if (rc < 0) {
			LOGE("Error reading from '%s'.", state->serial->address);
			return E_READ;
		} else if (rc > 0) {
			i += rc;
			retry = 30;
		} else if (rc == 0) {
			msleep(1);
			if (retry-- > 0 && i < (int32_t)size) continue;
		}
	};

	//Receive checksum and final confirmation.
	uint8_t csumok[3];
	i += serial_read(state->serial, csumok, 3);

	//CRC value from MCU.
	uint16_t pkcrc = ((csumok[0] << 8) & 0xFF00) | csumok[1];

	//CRC value as computed by us.
	uint16_t mycrc = crcitt(buf, size);

	//Keep copy for caller.
	if (pcrc) *pcrc = pkcrc;

	if ((i < (int32_t)(size + 3)) || (csumok[2] != KERNAL32_RESP_ACK)) {
		LOGE("Error reading from '%s'.", state->serial->address);
		return E_READ;
	}

	if (mycrc != pkcrc) {
		LOGE("ERROR: CRC mismatch. Packet CRC 0x%04X is not 0x%04X.", pkcrc, mycrc);
		return E_MSGMALFORMED;
	}

	return E_NONE;
}

int kernal32_writeflash(struct kernal32 *state, uint32_t flash_base, uint8_t *buf, uint32_t size, uint16_t *pcsum) {
	int rc;

	serial_purge(state->serial);

	uint8_t cmd[6];

	cmd[0] = KERNAL32_CMD_WRITEFLASH;

	//Address is big endian.
	cmd[1] = (flash_base & 0xFF);
	cmd[2] = (flash_base >> 8) & 0xFF;
	cmd[3] = (flash_base >> 16) & 0xFF;

	rc = serial_write(state->serial, cmd, 4);
	if (rc < 4) {
		LOGE("Could not write to: %s.", state->serial->address);
		return E_WRITE;
	}

	serial_drain(state->serial);

	//Receive 'busy' marker.
	rc = serial_read(state->serial, cmd, 1);
	if (rc < 1) {
		LOGE("Error reading from '%s'.", state->serial->address);
		return E_READ;
	}

	//Receive 'ready' marker.
	rc = serial_read(state->serial, cmd, 1);
	if (rc < 1) {
		LOGE("Error reading from '%s'.", state->serial->address);
		return E_READ;
	}

	rc = serial_write(state->serial, buf, size);
	if (rc < 0 || rc < (int32_t)size) {
		LOGE("Error writing to '%s'.", state->serial->address);
		return E_WRITE;
	}

	uint16_t crc = crcitt(buf, size);

	//Keep copy for caller.
	if (pcsum) *pcsum = crc;

	cmd[0] = crc >> 8;
	cmd[1] = crc;
	rc = serial_write(state->serial, cmd, 2);
	if (rc < 1) {
		LOGE("ERROR: Error writing checksum to '%s'.", state->serial->address);
		return E_WRITE;
	}

	// Wait a bit for the data to go down the wire and then read busy marker and confirmation.
	serial_drain(state->serial);

	memset(&cmd, 0x00, sizeof(cmd));
	rc = serial_read(state->serial, cmd, 2);

	if (rc < 2 || cmd[0] != KERNAL32_RESP_BUSY || cmd[1] != KERNAL32_RESP_ACK) {
		if (cmd[0] == KERNAL32_RESP_ERRCRC) {
			LOGE("CRC error in communication.");
		} else {
			LOGE("Error reading from '%s'.", state->serial->address);
			return E_READ;
		}
		return E_MSGMALFORMED;
	}

	return E_NONE;
}

/** @} */

