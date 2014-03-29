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
@addtogroup serial
@{
*/
#include "stdafx.h"

#ifndef __WIN32__
/**
Returns given decimal baud rate as bit mask.

@param bps	The baud rate in decimal.
@return	If bps is valid, returns a corresponding bit mask.
@return	On any errors, returns B38400.
 */
static uint32_t com_bps_mask(int bps) {
	switch (bps) {
		case 4800: return(B4800);
		case 9600: return(B9600);
		case 19200: return(B19200);
		case 38400: return(B38400);
		case 57600: return(B57600);
		case 115200: return(B115200);
		default: return B38400;
	}
}
#endif

int serial_isopen(struct serial *serial) {
#ifdef __WIN32__
	if (serial && (serial->fd != 0 && serial->fd != INVALID_HANDLE_VALUE)) {
		return 1;
	}
#else
	if (serial->fd > 0) {
		return 1;
	}
#endif

	return 0;
}

//URI example for windows:	"\\.\COM1:9600:8N1".
//URI example for Linux:	"/dev/ttyS0:9600:8N1".
int serial_open(struct serial *serial, char *uri) {
	assert(serial);

	sscanf(uri, "%255[^:]:%d:%d%c%d", serial->address, &serial->baudrate, &serial->bytesize, &serial->parity, &serial->stopbits);

#ifdef __WIN32__
	if (serial->fd != INVALID_HANDLE_VALUE) {
		serial_close(serial);
	}

	serial->fd = CreateFile(serial->address, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH, 0);

	if (serial->fd == INVALID_HANDLE_VALUE) {
		return E_OPEN;
	}

	DCB dcbSerialParams;
	memset(&dcbSerialParams, 0x00, sizeof(dcbSerialParams));
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(serial->fd, &dcbSerialParams)) {
		return E_SETOPTION;
	}

	dcbSerialParams.BaudRate = serial->baudrate;
	dcbSerialParams.ByteSize = serial->bytesize;
	dcbSerialParams.StopBits = ONESTOPBIT; //serial->stopbits;

	switch (serial->parity) {
		case 'o':
		case 'O':
			dcbSerialParams.Parity = ODDPARITY;
			break;
		case 'e':
		case 'E':
			dcbSerialParams.Parity = EVENPARITY;
			break;
		case 'n':
		case 'N':
		default:
			dcbSerialParams.Parity = NOPARITY;
			break;
	}

	//Disable all flow control.
	dcbSerialParams.fOutxCtsFlow = FALSE;
	dcbSerialParams.fOutxDsrFlow = FALSE;
	dcbSerialParams.fDtrControl = DTR_CONTROL_DISABLE;
	dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE;
	dcbSerialParams.fInX = FALSE;
	dcbSerialParams.fOutX = FALSE;

	if (!SetCommState(serial->fd, &dcbSerialParams)){
		return E_SETOPTION;
	}

	COMMTIMEOUTS timeouts;
	memset(&timeouts, 0x00, sizeof(timeouts));
	timeouts.ReadIntervalTimeout=25;
	timeouts.ReadTotalTimeoutMultiplier=10;
	timeouts.ReadTotalTimeoutConstant=50;

	timeouts.WriteTotalTimeoutConstant=50;
	timeouts.WriteTotalTimeoutMultiplier=10;
	if (!SetCommTimeouts(serial->fd, &timeouts)){
		return E_SETOPTION;
	}

	//Purge both input and output buffers.
	PurgeComm (serial->fd, PURGE_TXCLEAR | PURGE_RXCLEAR);
#else
	int pflags = O_NOCTTY | O_NONBLOCK | O_RDWR;

	if (serial->fd > 0) {
		tcdrain(serial->fd);
		serial_close(serial);
	}

	serial->fd = open(serial->address, pflags);
	if (serial->fd < 0) {
		return E_OPEN;
	}

	struct termios termios;
	tcgetattr(serial->fd, &termios);

	//FIXME, set parity and bytesize!
	termios.c_cflag = CS8 | CLOCAL | CREAD;
	termios.c_iflag = IGNPAR;// | ICRNL;
	termios.c_oflag = 0;
	termios.c_lflag &= ~ICANON; /* Set non-canonical mode */

	//Set non-blocking.
	termios.c_cc[VTIME] = 0;
	termios.c_cc[VMIN] = 1;

	cfsetispeed(&termios, com_bps_mask(serial->baudrate));
	cfsetospeed(&termios, com_bps_mask(serial->baudrate));

	if (tcsetattr(serial->fd, TCSANOW, &termios) < 0) {
		LOGE("Could not set serial attributes!");
		return E_SETOPTION;
	}
#endif

	LOGD("Opened '%s:%d:%d:%c:%d'.", serial->address, serial->baudrate, serial->bytesize, serial->parity, serial->stopbits);
	return E_NONE;
}

int serial_readln(struct serial *serial, uint8_t *buffer, int count) {
	int i, n = 0;
	double timeout = 0.1;

	assert(serial);
	assert(buffer);

	timeout += get_ticks();
	for (i = 0; i < count; i++) {
#ifdef __WIN32__
		if (serial->fd == INVALID_HANDLE_VALUE) return E_NOTOPEN;
		DWORD n = 0;
		if(!ReadFile(serial->fd, &buffer[i], 1, &n, NULL)) {
			return E_READ;
		}
#else
		int rc;
		if (serial->fd < 0) return E_NOTOPEN;
		rc = read(serial->fd, &buffer[i], 1);
		if (rc < 0) {
			return (E_READ);
		}
#endif
		if (timeout < get_ticks()) break;
		if (buffer[i] == '\n') break;
	}
	return n;
}

void serial_close(struct serial *serial) {
	assert(serial);

#ifdef __WIN32__
	if (serial && serial->fd != INVALID_HANDLE_VALUE) {
		CloseHandle(serial->fd);
		serial->fd = INVALID_HANDLE_VALUE;
	}
#else
	if (serial->fd > 0) {
		close(serial->fd);
		serial->fd = -1;
	}
#endif
	LOGD("Closed '%s'.", serial->address);
}

int serial_read(struct serial *serial, uint8_t *buffer, int count) {
	assert(serial);
	assert(buffer);

#ifdef __WIN32__
	if (serial->fd == INVALID_HANDLE_VALUE) return E_NOTOPEN;
	DWORD n = 0;
	if(!ReadFile(serial->fd, buffer, count, &n, NULL)){
		return E_READ;
	}
#else
	if (serial->fd < 0) return E_NOTOPEN;
	int n;
	n = read(serial->fd, buffer, count);
	if (n < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) return 0;
		return (E_READ);
	}
#endif

	if (serial->debug && n > 0) {
		LOGI("[%s READ]", serial->address);
		//fprintf(stderr, "READ:\n");
		hex_dump(stderr, buffer, n);
	}
	return n;
}

int serial_write(struct serial *serial, uint8_t *buffer, int count) {
	assert(serial);
	assert(buffer);

#ifdef __WIN32__
	if (serial->fd == INVALID_HANDLE_VALUE) return E_NOTOPEN;
	DWORD n = 0;
	if (!WriteFile(serial->fd, buffer, count, &n, NULL)) {
		return E_WRITE;
	}
#else
	if (serial->fd < 0) return E_NOTOPEN;
	int n;
	if ((n = write(serial->fd, buffer, count)) < count) {
		return E_WRITE;
	}
#endif

	if (serial->debug) {
		LOGI("[%s WRITE]", serial->address);
		hex_dump(stderr, buffer, n);
	}

	return n;
}

//Write a C string to serial port.
int serial_puts(struct serial *serial, char *line) {
	assert(serial);
	assert(line);

#ifdef __WIN32__
	if (serial->fd == INVALID_HANDLE_VALUE) return E_NOTOPEN;
	DWORD n = 0;
	if (!WriteFile(serial->fd, line, strlen(line), &n, NULL)) {
		return E_WRITE;
	}
#else
	if (serial->fd < 0) return E_NOTOPEN;
	int n;
	if ((n = write(serial->fd, line, strlen(line))) < 0) {
		return E_WRITE;
	}
#endif

	if (serial->debug) {
		LOGD("[%s WRITE] '%s'", serial->address, line);
	}

	return n;
}

//Purge both input and output buffers.
int serial_purge(struct serial *serial) {
	assert(serial);

#ifdef __WIN32__
	PurgeComm (serial->fd, PURGE_TXCLEAR | PURGE_RXCLEAR);
#else
	tcflush(serial->fd, TCIOFLUSH);
#endif
	return E_NONE;
}

//Wait until all output data has been written.
int serial_drain(struct serial *serial) {
	assert(serial);

#ifdef __WIN32__
	FlushFileBuffers(serial->fd);
#else
	tcdrain(serial->fd);
#endif
	return E_NONE;
}

int serial_setbaud(struct serial *serial, int newbaud) {
	assert(serial);

#ifdef __WIN32__
	DCB dcbSerialParams;
	memset(&dcbSerialParams, 0x00, sizeof(dcbSerialParams));
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(serial->fd, &dcbSerialParams)) {
		return E_SETOPTION;
	}

	dcbSerialParams.BaudRate = newbaud;
	if (!SetCommState(serial->fd, &dcbSerialParams)){
		LOGE("Error %d: Could not set serial attributes!", GetLastError());
		return E_SETOPTION;
	}
#else
	struct termios termios;
	tcgetattr(serial->fd, &termios);

	cfsetispeed(&termios, com_bps_mask(newbaud));
	cfsetospeed(&termios, com_bps_mask(newbaud));

	if (tcsetattr(serial->fd, TCSADRAIN, &termios) < 0) {
		LOGE("Error %d: Could not set serial attributes!", errno);
		return E_SETOPTION;
	}
#endif

	serial->baudrate = newbaud;
	LOGD("Parameters changed to '%s:%d:%d%c%d'.", serial->address, serial->baudrate, serial->bytesize, serial->parity, serial->stopbits);
	return E_NONE;
}

/** @} */

