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
@addtogroup log
@{
*/
#include "stdafx.h"

#ifdef __WIN32__
#include "resource.h"
#endif

/**
Selects verbosity level.
See enum logg_type for valid values.
*/
int verbosity = LOGG_INFO;

#undef COLORING

#ifdef COLORING
/** Red foreground color. */
#define RED		"\033[31;1m"
/** Yellow foreground color. */
#define YELLOW	"\033[33;1m"
/** White foreground color. */
#define WHITE	"\033[37;1m"
/** Magenta foreground color. */
#define MAGENTA	"\033[35;1m"
/** Normal foreground color. */
#define NORM	"\033[m"
#else
/** Dummy. */
#define RED
/** Dummy. */
#define YELLOW
/** Dummy. */
#define WHITE
/** Dummy. */
#define MAGENTA
/** Dummy. */
#define NORM
#endif

/** Shared file pointer to main log file. */
FILE *flogg = NULL;

/** Can be overwritten in main via command line argument. */
char *loggpath = "main.log";

#ifdef DEBUGGING
int logg(const char *file, int line, enum logg_type type, const char *fmt, ...) {
#else
int logg(enum logg_type type, const char *fmt, ...) {
#endif
	char *fmtbuf = NULL;
	char *obuf = NULL;
	int n = 0;
	char ts[25];
	va_list ap;

	if (flogg == NULL)  {
		flogg = fopen(loggpath, "w+");
		if (flogg == NULL) {
			fprintf(stderr, "Could not open file 'main.log' for writing.");
		}
	}

	if (flogg == NULL) {
		return E_OPEN;
	}

	timestamp(ts, sizeof(ts), 0);
	ts[22] = '\0';

	va_start(ap, fmt);

	switch (type) {
		case LOGG_DEBUG:
#ifdef DEBUGGING
			n = asprintf(&fmtbuf, MAGENTA"[%s](%s:%d)[DBG]: %s\n"NORM, ts, file, line, fmt);
#else
			if (verbosity < LOGG_DEBUG) return 0;
			n = asprintf(&fmtbuf, MAGENTA"[DBG]: %s\n"NORM, fmt);
#endif
			n = vasprintf(&obuf, fmtbuf, ap);
			break;
		case LOGG_INFO:
			if (verbosity < LOGG_INFO) return 0;
#ifdef DEBUGGING
			n = asprintf(&fmtbuf, WHITE"[%s](%s:%d)[INF]: %s\n"NORM, ts, file, line, fmt);
#else
			n = asprintf(&fmtbuf, WHITE"[INF]: %s\n"NORM, fmt);
#endif

			n = vasprintf(&obuf, fmtbuf, ap);
			break;
		case LOGG_WARNING:
			if (verbosity < LOGG_WARNING) return 0;
#ifdef DEBUGGING
			n = asprintf(&fmtbuf, YELLOW"[%s](%s:%d)[WRN]: %s\n"NORM, ts, file, line, fmt);
#else
			n = asprintf(&fmtbuf, YELLOW"[WRN]: %s\n"NORM, fmt);
#endif
			n = vasprintf(&obuf, fmtbuf, ap);
			break;
		case LOGG_ERROR:
			if (verbosity < LOGG_ERROR) return 0;
#ifdef DEBUGGING
			n = asprintf(&fmtbuf, RED"[%s](%s:%d)[ERR]: %s\n"NORM, ts, file, line, fmt);
#else
			n = asprintf(&fmtbuf, RED"[ERR]: %s\n"NORM, fmt);
#endif
			n = vasprintf(&obuf, fmtbuf, ap);
			break;
		case LOGG_RAW:
			n = asprintf(&obuf, "%s", (char *)fmt);
			break;
		case LOGG_NONE:
			return 0;
	}

	fwrite(obuf, n, 1, flogg);
	fflush(flogg);

#ifdef __WIN32__
	{
		extern HWND hwndDlg;
		HWND w = GetDlgItem(hwndDlg, IDC_LBLSTATUS);
		SetWindowText(w, obuf);
	}
#else
	fwrite(obuf, n, 1, stderr);
	fflush(stderr);
#endif

	free(fmtbuf);
	free(obuf);

	va_end(ap);

	return n;
}

/** Raw log, without any formatting or newline. */
int loggr(const char *fmt, ...) {
	char *fmtbuf = NULL;
	char *obuf = NULL;
	int n = 0;
	va_list ap;

	if (flogg == NULL)  {
		flogg = fopen(loggpath, "w+");
		if (flogg == NULL) {
			fprintf(stderr, "Could not open file '%s' for writing.", loggpath);
		}
	}

	if (flogg == NULL) {
		return E_OPEN;
	}

	va_start(ap, fmt);
	n = asprintf(&fmtbuf, "%s", fmt);
	n = vasprintf(&obuf, fmtbuf, ap);

	fwrite(obuf, n, 1, flogg);
	fflush(flogg);

#ifdef __WIN32__
	{
		extern HWND hwndDlg;
		HWND w = GetDlgItem(hwndDlg, IDC_LBLSTATUS);
		SetWindowText(w, obuf);
	}
#else
	fwrite(obuf, n, 1, stderr);
	fflush(stderr);
#endif

	free(fmtbuf);
	free(obuf);

	va_end(ap);

	return n;
}

/* dumps size bytes of *data to log file. Looks like:
[0000] 75 6E 6B 6E 6F 77 6E 20 30 FF 00 00 00 00 39 00 unknown 0.....9.
*/
void logg_hex(uint8_t *data, size_t size) {
	uint8_t *p = data;
	unsigned char c;
	size_t n;
	char bytestr[4] = {0};
	char addrstr[10] = {0};
	char hexstr [16*3 + 5] = {0};
	char charstr[16*1 + 5] = {0};

	if (flogg == NULL)  {
		flogg = fopen(loggpath, "w+");
		if (flogg == NULL) {
			LOGE("Could not open file '%s' for writing.", loggpath);
			return;
		}
	}

	for(n = 1; n <= size; n++) {
		if (n % 16 == 1) {
			/* store address for this line */
			snprintf(addrstr, sizeof(addrstr), "%"PRId64, (uint64_t)(p - data));
		}

		c = *p;
		if (isalnum(c) == 0 && isgraph(c) == 0) {
			c = '.';
		}

		/* store hex str (for left side) */
		snprintf(bytestr, sizeof(bytestr), "%02X ", *p);
		strncat(hexstr, bytestr, sizeof(hexstr)-strlen(hexstr) - 1);

		/* store char str (for right side) */
		snprintf(bytestr, sizeof(bytestr), "%c", c);
		strncat(charstr, bytestr, sizeof(charstr)-strlen(charstr) - 1);

		if (n % 16 == 0) {
			/* line completed */
			fprintf(flogg, "[%4.4s] %-50.50s |%-16.16s|\n", addrstr, hexstr, charstr);
			fprintf(stderr, "[%4.4s] %-50.50s |%-16.16s|\n", addrstr, hexstr, charstr);
			hexstr[0] = 0;
			charstr[0] = 0;
		} else if (n % 8 == 0) {
			/* half line: add whitespaces */
			strncat(hexstr, "  ", sizeof(hexstr)-strlen(hexstr) - 1);
			strncat(charstr, " ", sizeof(charstr)-strlen(charstr) - 1);
		}
		p++; /* next byte */
	}

	if (strlen(hexstr) > 0) {
		/* print rest of buffer if not empty */
		fprintf(flogg, "[%4.4s] %-50.50s |%-16.16s|\n", addrstr, hexstr, charstr);
		fprintf(stderr, "[%4.4s] %-50.50s |%-16.16s|\n", addrstr, hexstr, charstr);
	}

	fflush(flogg);
}

/** @} */

