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

/** Error codes.
@defgroup	errorcode	Error codes
@{
Enumerated error codes that are used by the whole framework and subsystems.
@author		Kari Sigurjonsson
*/
#ifndef __ERRORCODE_H__
#define __ERRORCODE_H__

/** Generic error codes. */
enum error_code {
	E_NONE				=  0,	/**< Used to indicate a successful operation. */
	E_ERROR				= -1,	/**< Generic error. */
	E_ARGUMENT			= -2,	/**< Given argument did not fit the operation. */
	E_NOTFOUND			= -3,	/**< I could not find requested resource. */
	E_CREATE			= -4,	/**< I could not create resource. */
	E_OPEN				= -5,	/**< I could not open resource. */
	E_READ				= -6,	/**< I could not read from resource. */
	E_WRITE				= -7,	/**< I could not write to resource. */
	E_SEEK				= -8,	/**< I could not seek in resource. */
	E_CLOSE				= -9,	/**< I could not close resource. */
	E_PRIORITY			= -10,	/**< I could not set priority level. */
	E_USERCHANGE		= -11,	/**< I could not change user credentials. */
	E_ALREADY			= -12,	/**< I already. */
	E_LOCK				= -13,	/**< I could not lock resource. */
	E_UNLOCK			= -14,	/**< I could not unlock resource. */
	E_NOTREADY			= -15,	/**< Resource was not ready. */
	E_DEAD				= -16,	/**< Subject DOA. */
	E_TIMEOUT			= -17,	/**< Time passed allowed limit. */
	E_NOMEM				= -18,	/**< I am out of memory. */
	E_DBQUERY			= -19,	/**< I could not complete a database query. */
	E_DBSWITCH			= -20,	/**< I could not switch to the requested database. */
	E_DBOPEN			= -21,	/**< I could not open the requested database. */
	E_DBQREGISTER		= -22,	/**< I could not register the database query. */
	E_FORK				= -23,	/**< I could not fork myself. */
	E_DISABLED			= -24,	/**< Resource was disabled. */
	E_CONNECT			= -25,	/**< I could not connect to it. */
	E_FLDNOTFOUND		= -26,	/**< Field not found. */
	E_INVALID			= -27,	/**< Stuff was invalid. */
	E_RANGE				= -28,	/**< Resource was out of range. */
	E_PIPE				= -29,	/**< Pipe operation failed. */
	E_TYPE				= -30,	/**< Resource was of the wrong type. */
	E_EMPTY				= -31,	/**< Resource was empty. */
	E_FULL				= -32,	/**< Resource is full. */
	E_ENABLECOREDUMP	= -33,	/**< I could not enable core dump. */
	E_LUAINIT			= -34,	/**< I could not initialize Lua. */
	E_NOSPACE			= -35,	/**< Resource had no space left. */
	E_SOCKETCREATE		= -36,	/**< Could not create socket. */
	E_SOCKETCONNECT		= -37,	/**< I could not make the socket listen. */
	E_SOCKET			= -38,	/**< Unspecified socket related error. */
	E_SOCKETBIND		= -39,	/**< I could not bind socket. */
	E_SOCKETLISTEN		= -40,	/**< The resource was malformed. */
	E_SETOPTION			= -41,	/**< Could not set option. */
	E_WOULDBLOCK		= -42,	/**< That operation would block. */
	E_MSGMALFORMED		= -43,	/**< Malformed message. */
	E_MSGUNKNOWN		= -44,	/**< Unknown message type. */
	E_CONVERT			= -45,	/**< I could not convert resource. */
	E_INTERRUPTED		= -46,	/**< Operation was interrupted. */
	E_NOTCONNECTED		= -47,	/**< Remote resource is not connected. */
	E_SELECT			= -48,  /**< select() returned error. */
	E_SEMAPHORE			= -49,	/**< Semaphore related error. */
	E_CONFIG			= -50,	/**< Invalid configuration entry. */
	E_SERIALIZE			= -51,	/**< Data could not be serialized. */
	E_DESERIALIZE		= -52,	/**< Data could not be de-serialized. */
	E_TRUNCATE			= -53,	/**< Resource could not be truncated. */
	E_MMAP				= -54,	/**< Error memory mapping resource. */
	E_SIGNALINSTALL		= -55,	/**< Error installing signal handler. */
	E_ADDRESS			= -56,	/**< Malformed or invalid address. */
	E_LOOKUP			= -57,	/**< Lookup failed. */
	E_LUACALL			= -58,	/**< Error occurred in Lua context. */
	E_SYSEXEC			= -59,	/**< Call to system() or exec*() failed. */
	E_EOF				= -60,	/**< End of file. */
	E_DBPUSHQUERY		= -61,	/**< Could not push database query. */
	E_OUTOFSYNC			= -62,	/**< Sequence out of sync. */
	E_NOTIMPLEMENTED	= -63,	/**< Operation not implemented. */
	E_UNDERRANGE		= -64,	/**< Under-range errors. */
	E_OVERRANGE			= -65,	/**< Over-range errors. */
	E_EXCEPTION			= -66,	/**< An exception occurred. */
	E_COND_INIT			= -67,	/**< Error inititalizing condition. */
	E_CONDATTR_INIT		= -68,	/**< Error inititalizing condition attribute. */
	E_COND_WAIT			= -69,	/**< An error occurred when trying to wait on condition. */
	E_COND_SIGNAL		= -70,	/**< An error occurred when trying to signal a condition. */
	E_GROUPCHANGE		= -71,	/**< Could not change group credentials. */
	E_MISMATCH			= -72,	/**< Could not match A to B. */
	E_NOTEXIST			= -73,	/**< Resource does not exist. */
	E_NOTLOADED			= -74,	/**< Resource does not exist. */
	E_NOTOPEN			= -75,	/**< Resource is not open. */
	E_ACCESS			= -76,	/**< Access denied to resource. */
	E_SIZE				= -77,	/**< Size constaint error. */
	E_CORRUPT			= -78,	/**< Resource is corrupt. */
	E_CRC				= -79,	/**< Data checksum (or integrity) test failed. */
	//--
	ERROR_COUNT			= 80	/**< Total number of error codes. Used to size arrays etc. */
};

/**
	Returns the string name of the given error code.
	@param code The error code as per enum error_code.
	@return If code is valid, returns the string name.
	@return If code is invalid, returns "E_ERROR".
*/
const char *errorstr(enum error_code code);

/** @} */
#endif //!__ERRORCODE_H__

