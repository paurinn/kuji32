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
@addtogroup srec
@{
*/
#include "stdafx.h"

/**************************************************** Private *****************************************************/

//Forward declarations of inline methods.
char *srec_parse_header(char *s, struct srec *rec);
char *srec_parse_count(char *s, struct srec *rec);
char *srec_parse_address(char *s, struct srec *rec);
char *srec_parse_data(char *s, struct srec *rec);
char *srec_parse_checksum(char *s, struct srec *rec);

/** Convert a single digit from base 16 to base 10. */
uint8_t hexval(uint8_t c) {
	uint8_t values[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	if (c >= '0' && c <= '9') c -= 48; else if (c >= 'A' && c <= 'F') c -= 55; else if (c >= 'a' && c <= 'f') c -= 87; else return 0;
	return values[c];
}

/** Convert two hexadecimal digits into one 8 bit decimal value. */
uint8_t fromhex2(uint8_t a, uint8_t b) { return (hexval(a) * 16) + hexval(b); }

/** Convert four hexadecimal digits into one 16 bit decimal value. */
uint16_t fromhex4(uint8_t a, uint8_t b, uint8_t c, uint8_t d) { return (hexval(a) * 4096) + (hexval(b) * 256) + (hexval(c) * 16) + hexval(d); }

/** Convert four hexadecimal digits into one 24 bit decimal value. */
uint32_t fromhex6(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f) { return (hexval(a) * 1048576) + (hexval(b) * 65536) + (hexval(c) * 4096) + (hexval(d) * 256) + (hexval(e) * 16) + hexval(f); }

/** Convert eight hexadecimal digits into one 32 bit decimal value. */
uint32_t fromhex8(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h) { return (hexval(a) * 268435456) + (hexval(b) * 16777216) + (hexval(c) * 1048576) + (hexval(d) * 65536) + (hexval(e) * 4096) + (hexval(f) * 256) + (hexval(g) * 16) + hexval(h); }

/**
	Parse header from S-Record string.
	@param s Source string.
	@param rec Destination S-Record.
	@return On success, returns a pointer to the next field.
	@return On failure, returns a negative error code.
*/
inline char *srec_parse_header(char *s, struct srec *rec) {
	if (s == NULL || rec == NULL || s[0] != 'S' || s[1] == '\0') {
		LOGD("Invalid S-Record header: '%s'", s);
		return NULL;
	}
	rec->type = s[1] - 48;
	return s + 2;
}

/**
	Parse count from S-Record string.
	@param s Source string.
	@param rec Destination S-Record.
	@return On success, returns a pointer to the next field.
	@return On failure, returns a negative error code.
*/
inline char *srec_parse_count(char *s, struct srec *rec) {
	if (s == NULL || rec == NULL || s[0] == '\0' || s[1] == '\0') {
		LOGD("Invalid S-Record count: '%s'");
		return NULL;
	}

	rec->count = fromhex2(s[0], s[1]);
	return s + 2;
}

/**
	Parse address from S-Record string.
	@param s Source string.
	@param rec Destination S-Record.
	@return On success, returns a pointer to the next field.
	@return On failure, returns a negative error code.
*/
inline char *srec_parse_address(char *s, struct srec *rec) {
	if (s == NULL || rec == NULL || s[0] == '\0' || s[1] == '\0' || s[2] == '\0' || s[3] == '\0') {
		LOGD("Invalid S-Record address: '%s'", s);
		return NULL;
	}

	switch (rec->type) {
		case 0:
		case 1:
		case 5:
		case 9:
			//2 byte address.
			rec->address = fromhex4(s[0], s[1], s[2], s[3]);
			rec->count -= 2;
			return s + 4;
		case 2:
		case 8:
			//3 byte address.
			if (s[4] == '\0' || s[5] == '\0') {
				LOGD("Invalid S-Record address: '%s'", s);
				return NULL;
			}
			rec->address = fromhex6(s[0], s[1], s[2], s[3], s[4], s[5]);
			rec->count -= 3;
			return s + 6;
		case 3:
		case 7:
			if (s[4] == '\0' || s[5] == '\0' || s[6] == '\0' || s[7] == '\0') {
				LOGD("Invalid S-Record address: '%s'", s);
				return NULL;
			}
			//4 byte address.
			rec->address = fromhex8(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7]);
			rec->count -= 4;
			return s + 8;
	}

	return NULL;
}

/**
	Parse data from S-Record string.
	@param s Source string.
	@param rec Destination S-Record.
	@return On success, returns a pointer to the next field.
	@return On failure, returns a negative error code.
*/
inline char *srec_parse_data(char *s, struct srec *rec) {
	if (s == NULL || rec == NULL || s[0] == '\0' || rec->count > strlen(s) * 2) {
		LOGD("Invalid S-Record data: '%s'", s);
		return NULL;
	}

	//Skip last byte, checksum.
	rec->count--;

	//Read and interpret data.
	for (int b = 0; b < rec->count; b++, s += 2) {
		rec->data[b] = fromhex2(s[0], s[1]);
	};

	return s;
}

/**
	Parse checksum from S-Record string.
	@param s Source string.
	@param rec Destination S-Record.
	@return On success, returns a pointer to the next field.
	@return On failure, returns a negative error code.
*/
inline char *srec_parse_checksum(char *s, struct srec *rec) {
	if (s == NULL || rec == NULL || s[0] == '\0' || s[1] == '\0') {
		LOGD("Invalid S-Record checksum: '%s'", s);
		return NULL;
	}

	rec->csum = fromhex2(s[0], s[1]);
	return s + 2;
}

/**************************************************** Public *****************************************************/

int srec_readfile(struct srec **reclist, const char *filepath) {
	static char line[256];

	FILE *F = fopen(filepath, "r");
	if (F == NULL) {
		LOGE("ERROR: Could not open file '%s' for reading.", filepath);
		return E_OPEN;
	}

	memset(line, 0x00, sizeof(line));

	int len;
	int linecount = 0;
	char *s;
	struct srec *tail = NULL;
	struct srec *nextitem = NULL;
	struct srec item;
	while (fgets(line, sizeof(line) - 1, F)) {
		linecount++;

		s = str_trim(line);
		len = strlen(s);
		if (len < 10) {
			LOGE("Line %d in '%s' is not a valid S-Record.", linecount, filepath);
			continue;
		}

		memset(&item, 0x00, sizeof(struct srec));

		s = srec_parse_header(s, &item);
		if (s == NULL) {
			LOGE("Line %d in '%s' is not a valid S-Record, malformed header.", linecount, filepath);
			continue;
		}

		s = srec_parse_count(s, &item);
		if (s == NULL) {
			LOGE("Line %d in '%s' is not a valid S-Record, invalid count.", linecount, filepath);
			continue;
		}

		s = srec_parse_address(s, &item);
		if (s == NULL) {
			LOGE("Line %d in '%s' is not a valid S-Record, invalid address.", linecount, filepath);
			continue;
		}

		s = srec_parse_data(s, &item);
		if (s == NULL) {
			LOGE("Line %d in '%s' is not a valid S-Record, invalid data.", linecount, filepath);
			continue;
		}

		s = srec_parse_checksum(s, &item);
		if (s == NULL) {
			LOGE("Line %d in '%s' is not a valid S-Record, invalid checksum.", linecount, filepath);
			continue;
		}

		//Checksum is calculated over count, address and data.
		uint8_t csum = 0;
		for (int n = 2; n < (int)strlen(line) - 2; n += 2) {
			csum += fromhex2(line[n], line[n+1]);
		}
		csum = (~csum);

		if (csum != item.csum) {
			LOGE("Line %d in '%s' has invalid checksum (0x%02X != 0x%02X)", linecount, filepath, csum, item.csum);
			return E_ERROR;
		}

		//Only add valid records to list.
		nextitem = (struct srec *)calloc(1, sizeof(struct srec));
		assert(nextitem);
		memcpy(nextitem, &item, sizeof(struct srec));

		if (*reclist == NULL) {
			*reclist = nextitem;
			tail = *reclist;
		} else {
			tail->next = nextitem;
			tail = nextitem;
		}
	}

	fclose(F);
	F = NULL;

	return E_NONE;
}

int srec_printlist(struct srec *reclist) {
	assert(reclist);

	for (struct srec *sr = reclist; sr; sr = sr->next) {
		//Header, count and address.
		switch (sr->type) {
			case 0: printf("S0%02X%04X", sr->count + 3, sr->address); break;
			case 1: printf("S1%02X%04X", sr->count + 3, sr->address); break;
			case 2: printf("S2%02X%06X", sr->count + 4, sr->address); break;
			case 3: printf("S3%02X%08X", sr->count + 5, sr->address); break;
			case 5: printf("S5%02X%04X", sr->count + 3, sr->address); break;
			case 7: printf("S7%02X%08X", sr->count + 5, sr->address); break;
			case 8: printf("S8%02X%06X", sr->count + 4, sr->address); break;
			case 9: printf("S9%02X%04X", sr->count + 3, sr->address); break;
		}

		//Data
		for (int b = 0; b < sr->count; b++) {
			printf("%02X", sr->data[b]);
		}
		printf("%02X\r\n", sr->csum);
	}

	return E_NONE;
}

void srec_freelist(struct srec **reclist) {
	struct srec *item, *next;
	if (reclist && *reclist) {
		item = *reclist;
		while (item) {
			next = item->next;
			free(item);
			item = next;
		}
		*reclist = NULL;
	}
}

int srec_readfilebin(uint8_t **buf, const char *path, uint32_t address_low, uint32_t address_high) {
	char line[600];	//Enough for full S8 record.
	char *s;
	int len;

	FILE *F = fopen(path, "rb");
	if (F == NULL) {
		return E_OPEN;
	}

	//Allocate for full flash, it's only 16 megabytes.
	uint32_t size = 0xFFFFFF + 1;

	*buf = calloc(1, size);
	assert(*buf);

	memset(*buf, 0xFF, size);
	memset(line, 0x00, sizeof(line));

	struct srec rec;

	while (fgets(line, sizeof(line) - 1, F)) {
		s = str_trim(line);
		len = strlen(s);
		if (len < 10) {
			LOGE("Line '%s' in '%s' is not a valid S-Record.", line, path);
			continue;
		}

		memset(&rec, 0x00, sizeof(struct srec));

		s = srec_parse_header(s, &rec);
		if (s == NULL) {
			LOGE("Line '%s' in '%s' is not a valid S-Record, malformed header.", line, path);
			return E_ERROR;
		}

		if (rec.type != 2) {
			//LOGD("Skipping S-Record: '%s'.", line);
			continue;
		}

		s = srec_parse_count(s, &rec);
		if (s == NULL) {
			LOGE("Line '%s' in '%s' is not a valid S-Record, invalid count.", line, path);
			return E_ERROR;
		}

		s = srec_parse_address(s, &rec);
		if (s == NULL) {
			LOGE("Line '%s' in '%s' is not a valid S-Record, invalid address.", line, path);
			return E_ERROR;
		}

		if (rec.address < address_low || rec.address > address_high) {
			LOGE("Line '%s' in '%s' has out of bounds data. Address 0x%06X is outside of (0x%06X - 0x%06X", line, path, rec.address, address_low, address_high);
			return E_RANGE;
		}

		s = srec_parse_data(s, &rec);
		if (s == NULL) {
			LOGE("Line '%s' in '%s' is not a valid S-Record, invalid data.", line, path);
			return E_ERROR;
		}

		s = srec_parse_checksum(s, &rec);
		if (s == NULL) {
			LOGE("Line '%s' in '%s' is not a valid S-Record, invalid checksum.", line, path);
			continue;
		}

		//Checksum is calculated over count, address and data.
		uint8_t csum = 0;
		for (int n = 2; n < (int)strlen(line) - 2; n += 2) {
			csum += fromhex2(line[n], line[n+1]);
		}
		csum = (~csum);

		if (csum != rec.csum) {
			LOGE("Line '%s' in '%s' has invalid checksum (0x%02X != 0x%02X)", line, path, csum, rec.csum);
			return E_ERROR;
		}

		//If all checks out, copy only type 2 data records into buffer.
		//LOGD("Processing data record %d for address 0x%06X, %d bytes.", rec.type, rec.address, rec.count);
		for (uint8_t i = 0; i < rec.count; i++) {
			(*buf)[rec.address + i] = rec.data[i];
		}
	}

	return E_NONE;
}

int srec_printbuffer(uint8_t *buf, size_t size, uint8_t rectype, uint32_t address, FILE *F) {
	uint32_t bl;
	int n;
	int i;
	uint8_t csum;
	char out[256];
	bool empty;

	if (F == NULL) F = stdout;

	assert(buf);

	if (rectype != 1 && rectype != 2 && rectype != 3) {
		LOGE("Invalid record type %d.", rectype);
		return E_ARGUMENT;
	}

	fprintf(F, "S00700004B756A6965\n");

	for (bl = 0; bl < size; bl += 16) {
		memset(out, 0x00, sizeof(out));
		n = 0;

		//Header, count and address.
		switch (rectype) {
			case 1: n += snprintf(out + n, sizeof(out) - n, "S114%04X", address + bl); break;
			case 2: n += snprintf(out + n, sizeof(out) - n, "S214%06X", address + bl); break;
			case 3: n += snprintf(out + n, sizeof(out) - n, "S214%08X", address + bl); break;
		}

		empty = true;
		for (i = 0; i < 16; i++) {
			n += snprintf(out + n, sizeof(out) - n, "%02X", buf[bl + i]);
			if (buf[bl + i] != 0xFF) empty = false;
		}

		if (empty) continue;

		//Checksum is calculated over count, address and data.
		csum = 0;
		for (i = 2; i < (int)strlen(out); i += 2) {
			csum += fromhex2(out[i], out[i+1]);
		}
		csum = (~csum);

		n += snprintf(out + n, sizeof(out) - n, "%02X", csum);
		out[n] = '\0';

		fprintf(F, "%s\n", out);
	}

	return E_NONE;
}

int srec_writefilebin(uint8_t *buf, size_t size, const char *path, uint8_t rectype, uint32_t address) {
	FILE *F = fopen(path, "w+");

	if (F == NULL) {
		LOGE("Could not open file '%s' for writing.", path);
		return E_OPEN;
	}

	int rc = srec_printbuffer(buf, size, rectype, address, F);
	fclose(F);

	if (rc != E_NONE) {
		LOGE("Error writing to file '%s'.", path);
	}

	return rc;
}

/** @} */
