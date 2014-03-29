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

/** Logging.
@defgroup	log		Logging
@{

Define macros LOGI(), LOGW() and LOGE() to print a formatted string to console and log file.
The I, W, E are for Information, Warnings and Errors respectively.
A prefix is injected to each log line to indicate the type: "[INF]", "[WRN]", "[ERR]".

If you define DEBUGGING then the C source file and line number is prepended to the string.
To define debugging you can alter Makefile or if this is out of that context
give it to gcc as argument "-DDEBUGGING".

@author		Kari Sigurjonsson
*/
#ifndef __LOG_H__
#define __LOG_H__

/** Enumeration of all logg types. */
enum logg_type {
	LOGG_NONE = 0,		/**< No output. */
	LOGG_ERROR,			/**< Errors. */
	LOGG_WARNING,		/**< Warnings. */
	LOGG_INFO,			/**< Information. */
	LOGG_DEBUG,			/**< Debugging information. */
	LOGG_RAW			/**< Raw output, no formatting is applied and no newline appended. */
};

/** Hex dump to file and console.
Dumps size bytes of *data to log file. Looks like:
[0000] 75 6E 6B 6E 6F 77 6E 20 30 FF 00 00 00 00 39 00 unknown 0.....9.
@param data The data to hex dump.
@param size Number of bytes in data.
*/
void logg_hex(uint8_t *data, size_t size);

/**
	Log raw (no formatting or newline) text to output and log file.
	The text can be a printf format string with variable associated arguments.
	@param fmt Printf format string.
	@param ... Variable number of argument as per fmt.
	@return On success, returns the number of bytes printed.
	@return On failure, returns a negative error code.

*/
int loggr(const char *fmt, ...);

#ifdef DEBUGGING

/** Logs to file and console with added file and line information. */
int logg(const char *file, int line, enum logg_type type, const char *fmt, ...);


/** Write formatted 'information' to log file and console. */
#define LOGI(fmt, ...) logg(__FILE__, __LINE__, LOGG_INFO, fmt, ## __VA_ARGS__)

/** Write formatted 'warnings' to log file and console. */
#define LOGW(fmt, ...) logg(__FILE__, __LINE__, LOGG_WARNING, fmt, ## __VA_ARGS__)

/** Write formatted 'errors' to log file and console. */
#define LOGE(fmt, ...) logg(__FILE__, __LINE__, LOGG_ERROR, fmt, ## __VA_ARGS__)

/** Debugging. */
#define LOGD(fmt, ...) logg(__FILE__, __LINE__, LOGG_DEBUG, fmt, ## __VA_ARGS__)

/** Write raw string to output and log file. */
#define LOGR(fmt, ...) loggr(fmt, ## __VA_ARGS__)

#else

/** Logs to file and console. */
int logg(enum logg_type type, const char *fmt, ...);

/** Write formatted 'information' to log file and console. */
#define LOGI(fmt, ...) logg(LOGG_INFO, fmt, ## __VA_ARGS__)

/** Write formatted 'warnings' to log file and console. */
#define LOGW(fmt, ...) logg(LOGG_WARNING, fmt, ## __VA_ARGS__)

/** Write formatted 'errors' to log file and console. */
#define LOGE(fmt, ...) logg(LOGG_ERROR, fmt, ## __VA_ARGS__)

/** Debugging. */
#define LOGD(fmt, ...) logg(LOGG_DEBUG, fmt, ## __VA_ARGS__)

/** Write raw string to output and log file. */
#define LOGR(fmt, ...) loggr(fmt, ## __VA_ARGS__)

#endif	//DEBUGGING

/** Reference to the main log file. */
extern FILE *flogg;

/** Reference to path to main log file. */
extern char *loggpath;

/** Set this to a value from enum logg_type to allow output of that type and below. */
extern int verbosity;

#endif //!__LOG_H__
/** @} */

