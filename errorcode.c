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
@addtogroup errorcode
@{
*/
#include "stdafx.h"

/**
	This is a list of C string representation of error codes.
	The order of this must match enum error_code.
*/
const char *error_names[ERROR_COUNT] = {
	"E_NONE",
	"E_LOGE",
	"E_ARGUMENT",
	"E_NOTFOUND",
	"E_CREATE",
	"E_OPEN",
	"E_READ",
	"E_WRITE",
	"E_SEEK",
	"E_CLOSE",
	"E_PRIORITY",
	"E_USERCHANGE",
	"E_ALREADY",
	"E_LOCK",
	"E_UNLOCK",
	"E_NOTREADY",
	"E_DEAD",
	"E_TIMEOUT",
	"E_NOMEM",
	"E_DBQUERY",
	"E_DBSWITCH",
	"E_DBOPEN",
	"E_DBQREGISTER",
	"E_FORK",
	"E_DISABLED",
	"E_CONNECT",
	"E_FLDNOTFOUND",
	"E_INVALID",
	"E_RANGE",
	"E_PIPE",
	"E_TYPE",
	"E_EMPTY",
	"E_FULL",
	"E_ENABLECOREDUMP",
	"E_LUAINIT",
	"E_NOSPACE",
	"E_SOCKETCREATE",
	"E_SOCKETCONNECT",
	"E_SOCKET",
	"E_SOCKETBIND",
	"E_SOCKETLISTEN",
	"E_SETOPTION",
	"E_WOULDBLOCK",
	"E_MSGMALFORMED",
	"E_MSGUNKNOWN",
	"E_CONVERT",
	"E_INTERRUPTED",
	"E_NOTCONNECTED",
	"E_SELECT",
	"E_SEMAPHORE",
	"E_CONFIG",
	"E_SERIALIZE",
	"E_DESERIALIZE",
	"E_TRUNCATE",
	"E_MMAP",
	"E_SIGNALINSTALL",
	"E_ADDRESS",
	"E_LOOKUP",
	"E_LUACALL",
	"E_SYSEXEC",
	"E_EOF",
	"E_DBPUSHQUERY",
	"E_OUTOFSYNC",
	"E_NOTIMPLEMENTED",
	"E_UNDERRANGE",
	"E_OVERRANGE",
	"E_EXCEPTION",
	"E_COND_INIT",
	"E_CONDATTR_INIT",
	"E_COND_WAIT",
	"E_COND_SIGNAL",
	"E_GROUPCHANGE",
	"E_MISMATCH",
	"E_NOTEXIST",
	"E_NOTLOADED",
	"E_NOTOPEN",
	"E_ACCESSDENIED",
	"E_SIZE",
	"E_CORRUPT",
	"E_CRC",
};

/**
	This is a list of helpful or not C strings associtated to an error code.
	The order of this must match enum error_code.
*/
const char *error_help[ERROR_COUNT] = {
	"Used to indicate a successful operation.",
	"Unspecified error.",
	"Given argument did not fit the operation.",
	"I could not find requested resource.",
	"I could not create resource.",
	"I could not open resource.",
	"I could not read from resource.",
	"I could not write to resource.",
	"I could not seek in resource.",
	"I could not close resource.",
	"I could not set priority level.",
	"I could not change user credentials.",
	"I already.",
	"I could not lock resource.",
	"I could not unlock resource.",
	"Resource was not ready.",
	"Subject DOA.",
	"Time passed allowed limit.",
	"I am out of memory.",
	"I could not complete a database query.",
	"I could not switch to the requested database.",
	"I could not open the requested database.",
	"I could not register the database query.",
	"I could not fork myself.",
	"Resource was disabled.",
	"I could not connect to it.",
	"Field not found.",
	"Stuff was invalid.",
	"Resource was out of range.",
	"Pipe operation failed.",
	"Resource was of the wrong type.",
	"Resource was empty.",
	"Resource is full.",
	"I could not enable core dump.",
	"I could not initialize Lua.",
	"Resource had no space left.",
	"Could not create socket.",
	"I could not make the socket listen.",
	"Unspecified socket related error.",
	"I could not bind socket.",
	"The resource was malformed.",
	"Could not set option.",
	"That operation would block.",
	"Malformed message.",
	"Unknown message type.",
	"I could not convert resource.",
	"Operation was interrupted.",
	"Remote resource is not connected.",
	"select() returned error.",
	"Semaphore related error.",
	"Invalid configuration entry.",
	"Data could not be serialized.",
	"Data could not be de-serialized.",
	"Resource could not be truncated.",
	"Error memory mapping resource.",
	"Error installing signal handler.",
	"Malformed or invalid address.",
	"Lookup failed.",
	"Error occurred in Lua context.",
	"Call to system() or exec*() failed.",
	"End of file.",
	"Could not push database query.",
	"Sequence out of sync.",
	"Operation not implemented.",
	"Under-range errors.",
	"Over-range errors.",
	"An exception occurred.",
	"Error inititalizing condition.",
	"Error inititalizing condition attribute.",
	"An error occurred when trying to wait on condition.",
	"An error occurred when trying to signal a condition.",
	"Could not change group credentials.",
	"Could not match A to B.",
	"Resource does not exist.",
	"Resource has not been loaded.",
	"Resource is not open.",
	"Access denied to resource.",
	"Size constaint error.",
	"Resource is corrupt.",
	"Data checksum (or integrity) test failed.",
};

const char *errorstr(enum error_code code) {
	if (code >= ERROR_COUNT)
		return error_names[abs(E_ERROR)];
	return error_names[abs(code)];
}

/** @} */
