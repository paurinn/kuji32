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

/** Utility Routines.
@defgroup util Utility Routines
@{

Routines, macros, defines etc. that is not part of any specific subsystem.

@author Kari Sigurjonsson
*/
#ifndef __UTIL_H__
#define __UTIL_H__

/** Helper struct. */
struct ref {
	int tag;	/**< Application specific tag. */
	void *a;	/**< Reference to data A. */
	void *b;	/**< Reference to data B. */
};

//Helper macros to manipulate integers.

/** Calculates the minimum of a and b. */
#define MIN(x, y) (y ^ ((x ^ y) & -(x < y)))

/** Calculates the maximum of a and b. */
#define MAX(x, y) (x ^ ((x ^ y) & -(x < y)))

/** Ensures that x is between the limits set by low and high. If low is greater than high the result is undefined. */
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

/** Encode 16 bit value from two bytes. */
#define MAKEU16(a, b)   ((uint16_t)(((uint8_t)(a)) | ((uint16_t)((uint8_t)(b))) << 8))

/** Encode 32 bit value from two 16 bit values. */
#define MAKEU32(a, b)   ((uint32_t)(((uint16_t)(a)) | ((uint32_t)((uint16_t)(b))) << 16))

/** Return lower 16 bits of a 32 bit word. */
#define LO16(l)        ((uint16_t)(l))

/** Return upper 16 bits of a 32 bit word. */
#define HI16(l)        ((uint16_t)(((uint32_t)(l) >> 16) & 0xFFFF))

/** Return low byte of a 16 bit word. */
#define LO8(w)         ((uint8_t)(w))

/** Return high byte of a 16 bit word. */
#define HI8(w)			((uint8_t)(((uint16_t)(w) >> 8) & 0xFF))

/** Return value of a bit index e.g. BIT(0) == 1, BIT(7) == 128. */
#define BIT(_bit) (uint32_t)(1 << _bit)

/** So we can to SWBIT(1) to get the bit value of APP or SET switch 1 (first bit). */
#define SWBIT(_sw) (1 << (_sw - 1))

/**************************************************************/
#if defined(__linux__)
#  include <endian.h>
#define ntohll(x) be64toh(x)
#define htonll(x) htobe64(x)
#elif defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/endian.h>
#elif defined(__OpenBSD__)
#  include <sys/types.h>
#  define be16toh(x) betoh16(x)
#  define be32toh(x) betoh32(x)
#  define be64toh(x) betoh64(x)
#endif
/**************************************************************/

#ifndef MAX_PATH
#  ifdef PATH_MAX
/** Maximum length of paths. */
#    define MAX_PATH PATH_MAX
#  else
/** Maximum length of paths. */
#    define MAX_PATH 1024
#  endif
#endif

/** Tell number of elements in an array. */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#ifndef ntohll
/**
Swap network to host byte order.
@param input The integer to swap.
@return Returns the input swapped to host endian.
*/
uint64_t ntohll(const uint64_t input);
#endif

#ifndef htonll
/**
Swap host to network byte order.
@param input The integer to swap.
@return Returns the input swapped to network endian.
*/
uint64_t htonll(const uint64_t input);
#endif

/** Hex dump.
Dumps size bytes of *data to the given file or stdout if F is NULL. Looks like:
[0000] 75 6E 6B 6E 6F 77 6E 20 30 FF 00 00 00 00 39 00 unknown 0.....9.

Originally written by Steffen Dettmer.
Taken from his website: http://sws.dett.de

@param F			File pointer used as output. Uses stdout if it's NULL.
@param data		The data to 'dump'.
@param size		Maximum number of bytes to process.

@return Returns the number of characters written.
*/
int hex_dump(FILE *F, void *data, int size);

/**
Returns the a point in time relative to a set point in the past such as system boot up or UNIX epoch.
The resolution varies between systems. In Windows this is milliseconds i.e. GetTickCount() and
in Linux it has a nanosecond resolution via clock_gettime().
@return Returns the current tick.
*/
double get_ticks();

/**
Yield process execution for a given number of milliseconds.
@param ms The number of milliseconds to delay i.e. this function will return after ms milliseconds.
*/
void msleep(uint32_t ms);

/** Hash a given string.
dj2b general string hasher by Dan Bernstein.

@param str	The string to hash.
@param len	Maximum number of characters to compute from str.
@return		A hash computed from str.
*/
uint32_t string_hash(const char *str, int32_t len);

/** Trim whitespace left of string.
Move all characters following a sequence of whitespaces to the start of the string effectively removing
all whitespace left of the string.
The pointer returned is always within the range of the original string.

@param str	The string to trim.
@return		A trimmed string.
*/
char *str_ltrim(char *str);

/** Trim whitespace right of string.
Returns pointer to a string with any whitespace right of the first non whitespace character removed i.e.
a '\\0' replaces the first whitespace at its end.
The pointer returned is always within the range of the original string.

@param str	The string to trim.
@return		A trimmed string.
*/
char *str_rtrim(char *str);

/** Trim whitespace from boths sides of string.
Returns a string that has been run through str_ltrim() and str_rtrim() respectively.
The pointer returned is always within the range of the original string.

@param str	The string to trim.
@return		A trimmed string.
*/
char *str_trim(char *str);

/** Convert string to int32.
@param *s		The string to convert.
@param base		Numerical base.
@param *status	Optional destination for status
@return			On success, returns converted value.
@return			On failure, returns 0.
*/
int32_t strtoint32(char *s, int base, int *status);

/** Convert string to int64.
@param *s		The string to convert.
@param base		Numerical base.
@param *status	Optional destination for status
@return			On success, returns converted value.
@return			On failure, returns 0.
*/
int64_t strtoint64(char *s, int base, int *status);

/** Convert string to float.
@param *s		The string to convert.
@param *status	Optional destination for status
@return			On success, returns converted value.
@return			On failure, returns 0.
*/
float strtofloat(char *s, int *status);

/** Convert string to double.
@param *s		The string to convert.
@param *status	Optional destination for status
@return			On success, returns converted value.
@return			On failure, returns 0.
*/
double strtodouble(char *s, int *status);

#ifndef HAVE_ASPRINTF
/**
Local implementation of vasprintf.
This formats a string like usual but allocates for as large a buffer as is needed.
The pointer *ret must be freed by the called.
@param ret The dereferenced pointer will be assigned to the newly allocated character buffer. You must free *dest after use.
@param format The format string.
@param args Variable argument list.
@return On success, returns the number of characters printed.
@return On failure, returns a negative error.
*/
int vasprintf(char **ret, const char *format, va_list args);

/**
Local implementation of asprintf.
This formats a string like usual but allocates for as large a buffer as is needed.
The pointer *ret must be freed by the called.
@param ret The dereferenced pointer will be assigned to the newly allocated character buffer. You must free *dest after use.
@param format The format string.
@param ... Variable argument list.
@return On success, returns the number of characters printed.
@return On failure, returns a negative error.
*/
int asprintf(char **ret, const char *format, ...);
#endif

/**
Writes the current date formatted as 'DD.MM.YYYY hh:mm:ss.ms' into dest (which must hold 22 or more bytes).
@param dest Destination memory, must be 22 bytes or more.
@return Returns the number of bytes written (should be 22 bytes exactly).
*/
int timestamp(char *dest);

/**
Writes the current date formatted as 'DD.MM.YYYY' into dest (which must hold 10 or more bytes).
@param dest Destination memory, must be 10 bytes or more.
@return Returns dest.
*/
char *daystamp(char *dest);

/**
Write byte values from data into out as hexadecimal groups enclosed in brackets.
The variable arguments are group sizes as int and the last argument must be 0.

Example usage:

@code
char *data = "Some bytes";
char out[255];
int n = hexgroup((uint8_t *)data, 10, out, (sizeof(out) - 1), 1, 2, 3, 4, 0);
out[n] = '\0';

fprintf(stderr, "%s\n", out);
@endcode

The above code will print out the following:
@verbatim
[53][6F6D][652062][79746573]
@endverbatim

@param data The data to hex dump.
@param size Size of data.
@param out Output buffer.
@param osize Size of output buffer.

@return Returns the number of bytes written to out.
*/
int hexgroup(uint8_t *data, int size, char *out, int osize, ...);

/** Macro to define string split callback. */
#define STRSPLIT(_name) void (_name)(char *str, void *argument)

/** Function pointer to string split callback. */
typedef STRSPLIT(*strsplit_t);

/**
	Split the subject string at delimiter and call function for each segment.

	@param str	The subject string.
	@param delim	An array of character where each character is a delimiter.
					Repeated delimiters are grouped.
	@param callback	The function to call for each split segment.
	@param argument	Optional argument passed to the callback.

	@return On success, returns the number of segments processed.
	@return On failure, returns a negative error code.
*/
int strsplit(char *str, char *delim, strsplit_t callback, void *argument);

#ifndef strtok_r
/**
public domain strtok_r() by Charlie Gordon
from comp.lang.c  9/14/2007
http://groups.google.com/group/comp.lang.c/msg/2ab1ecbb86646684
(Declaration that it's public domain):
http://groups.google.com/group/comp.lang.c/msg/7c7b39328fefab9c
*/
char *strtok_r(char *str, const char *delim, char **nextp);
#endif

/**
Simple 8 bit checksum. Returns accumulated bytes values.
@param buf The buffer to accumulate.
@param len Total number of bytes in buf[].
@return Returns the lowest 8 bits of sum.
*/
uint8_t checksum8(uint8_t *buf, int len);

/**
Simple 16 bit checksum. Returns accumulated bytes values.
@param buf The buffer to accumulate.
@param len Total number of bytes in buf[].
@return Returns the 16 bit sum.
*/
uint16_t checksum16(uint8_t *buf, int len);

/**
	Calculate CRC16 value from buf[0] to buf[len-1].
	@param buf Data to checksum.
	@param len Number of bytes in buf[].
	@return Returns the checksum value.
*/
unsigned int crcitt(uint8_t *buf, int len);

/**
Allocate for and read a file into memory.
@param path Path to the file.
@param buf The dereferenced pointer will be assigned to the newly allocated buffer.
@return On success, returns the number of bytes allocated i.e. the file size.
@return On failure, returns a negative error code.
*/
long filedata(const char *path, uint8_t **buf);

#endif //__UTIL_H__
/** @} */

