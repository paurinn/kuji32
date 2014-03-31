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
Interface to Fujitsu FR32 MCU family built-in ROM bootstrap.
The bootstrap can only write to RAM so we implement a flash writer
and use the bootstrap to receive and call the executable.

Only a few commands are supported:
	- 0x56 - Initialize communication (doubles as PING).
	- 0x01 - Check if ROM is ready to rock.
	- 0x03 - Write code to any address within MCU IRAM.
	- 0x04 - Call code at any address within MCU IRAM.

This module relies on @link serial @endlink to abstract serial port access from architecture and OS but can be replaced with whatever easily.

@defgroup birom32 Stage 1 Interface to Built-In-ROM.
@{
*/
#ifndef __BIROM32_H__
#define __BIROM32_H__

/** Command identifiers. */
enum birom32_cmdid {
	BIROM32_CMD_PROBE	= 0x56,	/**< A probe for the initial ROM state. It must respond with 0x46 to indicate the correct state (directly after power-on). */
	BIROM32_CMD_CHECK	= 0x01,	/**< Check if ROM is ready. */
	/* Is BIROM32_CMD_READ = 0x02 ? */
	BIROM32_CMD_WRITE	= 0x03,	/**< Write data to RAM. Four byte address follows. */
	BIROM32_CMD_CALL	= 0x04,	/**< Call RAM address. Four byte address follows. */
};

/** Response identifiers. */
enum birom32_respid {
	BIROM32_RESP_INVALID	= 0x00, /**< Invalid response. Usually means time-out in communication. */
	BIROM32_RESP_PROBE		= 0x46,	/**< Acknowledge from the initial ROM state. ROM only responds with this directly after power-on. */
	BIROM32_RESP_CHECK		= 0xF1,	/**< Acknowledge for BIROM32_CMD_CHECK. */
	BIROM32_RESP_WRITE		= 0x83,	/**< Acknowledge for BIROM32_CMD_WRITE. */
	BIROM32_RESP_CALL		= 0x84,	/**< Acknowledge for BIROM32_CMD_CALL. */
	BIROM32_RESP_CALL_DONE	= 0x31,	/**< Sent just before stage 2 is called. After this is received the baud rate has changed on the MCU side. */
};

/**
	Run-time state for birom32.
*/
struct birom32_state {
	struct chipdef32 *chip;		/**< Configuration of MCU such as crystal frequency, path to kernal file etc. */
	struct serial *serial;		/**< Serial communication object. */
	uint8_t *kernaldata;		/**< Contentes of kernal file. */
	long kernalsize;			/**< Size of kernal file. */
};

/**
	Allocate for a new birom32 state and initialize connection to the given serial port at the given baud rate.
	@param state The dereferenced pointer is assigned to the newly allocated state.
	@param chipdef Prepared chip definition describing the chip being programmed.
	@param serial The serial port object to use.
	@return On success, returns E_NONE.
	@return On failure, returns a negative error code.
*/
int birom32_new(struct birom32_state **state, struct chipdef32 *chipdef, struct serial *serial);

/**
	Close serial port and free the given birom32 state.
	@param state The dereferenced pointer is freed and its value set to NULL.
*/
void birom32_free(struct birom32_state **state);

/**
	Connect to processor i.e. send BIROM32_CMD_OPEN and expect BIROM32_RESP_OK back.
	@param state The birom32 state.
	@param timeoutsec Time-out in seconds.
	@return On success, returns E_NONE.
	@return On failure, returns a negative error code.
*/
int birom32_connect(struct birom32_state *state, int timeoutsec);

/**
	Check if MCU ROM is in certain state.
	@param state The birom32 state.
	@return On success, returns E_NONE.
	@return On failure, returns a negative error code.
*/
int birom32_check(struct birom32_state *state);

/**
	Write size number of bytes to memory address in RAM.
	Ideal to dump flash loader executable.
	@param state The birom32 state.
	@param address Memory address.
	@param data Array of the bytes to write.
	@param size Number of bytes to write.
	@return On success, returns the number of bytes written.
	@return On failure, returns a negative error code.
*/
int birom32_write(struct birom32_state *state, uint32_t address, uint8_t *data, uint32_t size);

/**
	Make MCU jump to address. There is no return value from MCU as jump is immediate.
	@param state The birom32 state.
	@param address Dummy. The address is fixed by the built-in-ROM.
*/
int birom32_call(struct birom32_state *state, uint32_t address);

#endif //__BIROM32_H__
/** @} */

