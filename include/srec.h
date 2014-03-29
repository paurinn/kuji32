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
S-Record reader and writer.
@defgroup srec S-Records
@{
*/
#ifndef __SREC_H__
#define __SREC_H__

/**
	Binary version of the S-Record.
*/
struct srec {
	uint8_t type;		/**< Record type (0-9). */
	uint8_t count;		/**< Number of bytes in data[]. */
	uint32_t address;	/**< Address associated with data. */
	uint8_t csum;		/**< Check sum of data. */
	struct srec *next;	/**< For linked listing. */
	uint8_t data[0xFF];	/**< Payload data. */
};

/**
	Read contents of filepath into a linked list and returns the head.
	@param reclist The dereferenced pointer is assigned to the newly allocated list.
	@param filepath Path to the S-Record file.
	@return On success, returns E_NONE with *reclist assigned to the new head.
	@return On failure, returns a negative error code with *reclist assigned NULL.
*/
int srec_readfile(struct srec **reclist, const char *filepath);

/**
	Print S-Record list to stdout.
	@param reclist The S-Record list to print.
	@return On success, returns E_NONE.
	@return On failure, returns a negative error code.
*/
int srec_printlist(struct srec *reclist);

/**
	Free all entries in the given S-Record list.
	@param reclist The dereferenced pointer is assigned NULL after freeing all its entries.
*/
void srec_freelist(struct srec **reclist);

/**
	Read S-Records from file and write them to a linear buffer.
	The buffer is allocated as 16 megabytes so 24 bit address from S-Record works directly.
	This also means that only S2 data records are accepted, other types are ignored.
	@param buf The dereferenced pointer is assigned to the newly allocated buffer and is filled with record data.
	@param path Path to the S-Record file (*.mhx).
	@param address_low Lower bound for address check.
	@param address_high Upper bound for address check.
	@return On success, returns E_NONE with *buf assigned to the new buffer.
	@return On failure, returns a negative error code with *buf assigned to NULL.
*/
int srec_readfilebin(uint8_t **buf, const char *path, uint32_t address_low, uint32_t address_high);

/**
	Write binary buffer to file as S-Records.
	@param buf The binary buffer.
	@param size How many bytes in buffer[].
	@param path Path to file that receives the S-Records.
	@param rectype Type of data records (1, 2 or 3).
	@param address The base address of the buffer.
	@return On success, returns E_NONE.
	@return On failure, returns a negative error code.
*/
int srec_writefilebin(uint8_t *buf, size_t size, const char *path, uint8_t rectype, uint32_t address);

/**
	Print binary buffer to stdout as S-Records.
	@param buf The binary buffer.
	@param size How many bytes in buffer[].
	@param rectype Type of data records (1, 2 or 3).
	@param address The base address of the buffer.
	@param F Optional file pointer. If NULL then output goes to stdout.
	@return On success, returns E_NONE.
	@return On failure, returns a negative error code.
*/
int srec_printbuffer(uint8_t *buf, size_t size, uint8_t rectype, uint32_t address, FILE *F);

#endif //__SREC_H__
/** @} */
