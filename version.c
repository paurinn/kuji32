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
@addtogroup	version
@{
*/
#include "stdafx.h"

/** Version. */
struct version version = {
	MAJORVERSION,
	MINORVERSION,
	BUILD,
	REVISION,
	BUILD_TIMESTAMP,
	"Kuji32 Flash MCU Programmer"
};

/** Version as a C string. */
const char *version_string() {
	static char _version_string[255];
	snprintf(_version_string, sizeof(_version_string), "%s v%d.%d.%d-%d Stardate %ld", version.text, version.major, version.minor, version.revision, version.build, (long)version.build_date);
	return _version_string;
}

/** @} */

