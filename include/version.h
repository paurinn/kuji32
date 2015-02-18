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

/** Version Information.
@defgroup	version Version
@{

The file VERSION contains the project version. Version is available at compile time as
	- REVISION - A simple revision number from for example Subversion.
	- BUILD_TIMESTAMP - Unix timestamp when the source unit was built.
	- MAJORVERSION - Major version number.
	- MINORVERSION - Minor version number.
	- BUILD - Build counter incremented through make dist.

On run-time these numbers are globally available symbol 'struct version version'.

@author		Kari Sigurjonsson
*/
#ifndef __VERSION_H__
#define __VERSION_H__

/** Size in bytes, of 'text' field in struct version. */
#define VERSION_TEXT_LENGTH		100

/** Object to hold version information. */
struct version {
	int major;		/**< Major version. */
	int minor;		/**< Minor version. */
	int build;		/**< Build count i.e. 'make dist' increments this on each build. */
	time_t build_date;	/**< Time when library was built (unix epoch). */
	char text[VERSION_TEXT_LENGTH];	/**< Arbitrary string identifier. */
};

extern struct version version;	/**< Externally visible version. */

/** Return version as string. */
const char *version_string();

#endif //!__VERSION_H__
/** @} */

