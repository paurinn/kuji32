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
@addtogroup prog32
@{
*/
#include "stdafx.h"

/**
	Main entry point.
	Process chipdef file, parse user options and run the programming state machine.
*/
int main(int argc, char *argv[]) {
	struct params32 params;

	if (process_chipdef32() != E_NONE) {
		return FAIL_CHIPDEF;
	}

	int rc;
	if ((rc = process_params32(argc, argv, &params)) != E_NONE) {
		LOGE("Fatal error!");
		return rc;
	}

	return process32(&params);
}
/** @} */

