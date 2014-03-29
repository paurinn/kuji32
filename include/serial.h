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

/** Serial Port.
@defgroup serial Serial Ports.
@{

Subsystem to access serial ports.

@author Kari Sigurjonsson
*/
#ifndef __SERIAL_H__
#define __SERIAL_H__

/** Serial port state. */
struct serial {
	char address[255];	/**< Device path. */
	int32_t bytesize;	/**< Bits per byte, 7 or 8. */
	int32_t baudrate;	/**< E.g. 9600. */
	int32_t stopbits;	/**< Number of stop bits, 1 or 2. */
	char parity;		/**< 'O' for Odd, 'E' for Event and 'N' for No parity. */
	bool simulate;		/**< If true, then this acts on regular files. */
	bool debug;			/**< If true, then we print out all data sent and received. */

#ifdef __WIN32__
	HANDLE fd;			/**< Handle to serial device or file. */
#else
	int fd;				/**< File descriptor. */
#endif
};

/**
Check if serial port instance is open, that is, ready to read and write.
@param serial The serial port instance.
@return On success, returns 1.
@return On failure, returns 0.
*/
int serial_isopen(struct serial *serial);

/**
	Open a serial port state.
	@param serial Pointer to serial port state.
	@param uri Path to serial port and its configuration e.g. 'com1:9600:8N1'.
	@return On success, returns E_NONE.
	@return On failure, returns a negative error code.
*/
int serial_open(struct serial *serial, char *uri);

/**
	Close a serial port state.
	@param serial Pointer to serial port state.
	@return On success, returns E_NONE.
	@return On failure, returns a negative error code.
*/
void serial_close(struct serial *serial);

/**
	Read count number of bytes from serial port into buffer.
	@param serial The serial state.
	@param buffer The receiving buffer. It must hold at least count bytes.
	@param serial The serial state.
	@param count Maximum number of bytes to read.
	@return On success, returns the number of bytes read.
	@return On failure, returns a negative error code.
*/
int serial_read(struct serial *serial, uint8_t *buffer, int count);

/**
	Read from serial port, one character at a time, until '\n' or count many characters read.
	@param serial Pointer to serial state.
	@param buffer Pointer to destination memory.
	@param count Maximum number of bytes to read.
	@return On success, returns the number of bytes read. This can be less than count or 0.
	@return On failure, returns a negative error code.
*/
int serial_readln(struct serial *serial, uint8_t *buffer, int count);

/**
	Write count number of bytes from buffer to serial port.
	@param serial The serial state.
	@param buffer The source buffer.
	@param count Maximum number of bytes to write.
	@return On success, returns the number of bytes written.
	@return On failure, returns a negative error code.
*/
int serial_write(struct serial *serial, uint8_t *buffer, int count);

/**
	Write C string to serial port.
	@param serial The serial state.
	@param line The source buffer.
	@return On success, returns the number of bytes written.
	@return On failure, returns a negative error code.
*/
int serial_puts(struct serial *serial, char *line);

/**
	Delete all buffered data from underlying file descriptor both read and write.
	@param serial The serial state.
	@return On success, returns E_NONE.
	@return On failure, returns a negative error code.
*/
int serial_purge(struct serial *serial);

/**
	Wait until all buffered output data has been written.
	@note On USB to RS232 converters this can return almost immediately as the USB chip probably has a few kilobyte buffer that is slowly trickled out to RS232 side.
	@param serial The serial state.
	@return On success, returns E_NONE.
	@return On failure, returns a negative error code.
*/
int serial_drain(struct serial *serial);

/**
	Change to a new baud-rate.
	@param serial The serial state to change.
	@param newbaud The new baud rate.
	@return On success, returns E_NONE.
	@return On failure, returns a negative error code.
*/
int serial_setbaud(struct serial *serial, int newbaud);

#endif //__SERIAL_H__
/** @} */

