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

/** Standard Includes.
stdafx.h : include file for standard system include files,
or project specific include files that are used frequently, but are changed infrequently.

@author Kari Sigurjonsson
@defgroup stdafx Common Headers
@{
*/
#ifndef __STDAFX_H__
#define __STDAFX_H__

#ifdef __WIN32__

#include <sdkddkver.h>

// Exclude rarely-used stuff from Windows headers.
#define WIN32_LEAN_AND_MEAN

// Windows Header Files:
#include <windows.h>

#ifdef GUI
#include <windowsx.h>
#include <basetyps.h>
#include <shellapi.h>
#include <objbase.h>
#include <resource.h>
#include <commdlg.h>
#include <commctrl.h>
#endif //GUI

#endif //__WIN32__

// C RunTime Header Files
#include <unistd.h>
#include <getopt.h>
#include <float.h>
#include <limits.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <inttypes.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <signal.h>
#include <libintl.h>
#include <fcntl.h>

#ifndef __WIN32__
#include <termios.h>
#endif

//Local includes.
#include "version.h"
#include "errorcode.h"
#include "log.h"
#include "util.h"
#include "serial.h"
#include "srec.h"
#include "prog32.h"
#include "birom32.h"
#include "kernal32.h"

#endif //!__STDAFX_H__
/** @} */

