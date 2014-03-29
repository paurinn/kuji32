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

/** Name of the chip definition file for 32 bit MCUs. */
const char *chipdef_filename = "chipdef32.ini";

/** Map of each MCU indexed by enum mcu32_type. */
struct mcu32_tag mcu32_map[] = {
	{NULL,						MCU32_INVALID},
	{"MB91F109",				MCU32_MB91F109},
	{"MB91F127",				MCU32_MB91F127},
	{"MB91F128",				MCU32_MB91F128},
	{"MB91F133",				MCU32_MB91F133},
	{"MB91F155",				MCU32_MB91F155},
	{"MB91F158",				MCU32_MB91F158},
	{"MB91F211",				MCU32_MB91F211},
	{"MB91F213",				MCU32_MB91F213},
	{"MB91F223",				MCU32_MB91F223},
	{"MB91F224",				MCU32_MB91F224},
	{"MB91F233",				MCU32_MB91F233},
	{"MB91F248",				MCU32_MB91F248},
	{"MB91F249_S",				MCU32_MB91F249_S},
	{"MB91F264",				MCU32_MB91F264},
	{"MB91F267",				MCU32_MB91F267},
	{"MB91F272",				MCU32_MB91F272},
	{"MB91F273",				MCU32_MB91F273},
	{"MB91FV310___PROG",		MCU32_MB91FV310_PROG},
	{"MB91FV310___FONT",		MCU32_MB91FV310_FONT},
	{"MB91F312",				MCU32_MB91F312},
	{"MB91F313",				MCU32_MB91F313},
	{"MB91F314",				MCU32_MB91F314},
	{"MB91F318R",				MCU32_MB91F318R},
	{"MB91FV319A___PROG",		MCU32_MB91FV319A_PROG},
	{"MB91FV319A___FONT",		MCU32_MB91FV319A_FONT},
	{"MB91FV319R___PROG",		MCU32_MB91FV319R_PROG},
	{"MB91FV319R___FONT",		MCU32_MB91FV319R_FONT},
	{"MB91F345",				MCU32_MB91F345},
	{"MB91F346",				MCU32_MB91F346},
	{"MB91F353",				MCU32_MB91F353},
	{"MB91F355",				MCU32_MB91F355},
	{"MB91F356B",				MCU32_MB91F356B},
	{"MB91F362",				MCU32_MB91F362},
	{"MB91F365",				MB91F365},
	{"MB91F366",				MB91F366},
	{"MB91F367",				MB91F367},
	{"MB91F368",				MB91F368},
	{"MB91F369",				MB91F369},
	{"MB91F463C",				MB91F463C},
	{"MB91F463NA",				MB91F463NA},
	{"MB91F463NC",				MB91F463NC},
	{"MB91F464A",				MB91F464A},
	{"MB91F464H",				MB91F464H},
	{"MB91F465B",				MB91F465B},
	{"MB91F465C",				MB91F465C},
	{"MB91F465D",				MB91F465D},
	{"MB91F465K",				MB91F465K},
	{"MB91F465P",				MB91F465P},
	{"MB91F465X",				MB91F465X},
	{"MB91F467B",				MB91F467B},
	{"MB91F467C",				MB91F467C},
	{"MB91F467D",				MB91F467D},
	{"MB91F467M",				MB91F467M},
	{"MB91F467R",				MB91F467R},
	{"MB91F467S",				MB91F467S},
	{"MB91F467T",				MB91F467T},
	{"MB91F469G",				MB91F469G},
	{"MB91F475",				MB91F475},
	{"MB91F478",                MB91F478},
	{"MB91F479",                MB91F479},
	{"MB91F482",                MB91F482},
	{"MB91F486",                MB91F486},
	{"MB91F487",                MB91F487},
	{"MB91F492",                MB91F492},
	{"MB91F522B_D_F_J_K_L",     MB91F522B_D_F_J_K_L},
	{"MB91F523B_D_F_J_K_L",     MB91F523B_D_F_J_K_L},
	{"MB91F524B_D_F_J_K_L",     MB91F524B_D_F_J_K_L},
	{"MB91F525B_D_F_J_K_L",     MB91F525B_D_F_J_K_L},
	{"MB91F526B_D_F_J_K_L",     MB91F526B_D_F_J_K_L},
	{"MB91F575_S",              MB91F575_S},
	{"MB91F577_S",              MB91F577_S},
	{"MB91F583MG_MH_MJ_MK",     MB91F583MG_MH_MJ_MK},
	{"MB91F583SG_SH_SJ_SK",     MB91F583SG_SH_SJ_SK},
	{"MB91F584MG_MH_MJ_MK",     MB91F584MG_MH_MJ_MK},
	{"MB91F584SG_SH_SJ_SK",     MB91F584SG_SH_SJ_SK},
	{"MB91F585MG_MH_MJ_MK",     MB91F585MG_MH_MJ_MK},
	{"MB91F585SG_SH_SJ_SK",     MB91F585SG_SH_SJ_SK},
	{"MB91F585LA_LB_LC_LD",     MB91F585LA_LB_LC_LD},
	{"MB91F586LA_LB_LC_LD",     MB91F586LA_LB_LC_LD},
	{"MB91F587LA_LB_LC_LD",     MB91F587LA_LB_LC_LD},
	{"MB91F591B_BS_BH_BHS",     MB91F591B_BS_BH_BHS},
	{"MB91F592B_BS_BH_BHS",     MB91F592B_BS_BH_BHS},
	{"MB91F594B_BS_BH_BHS",     MB91F594B_BS_BH_BHS},
	{"MB91F599B_BS_BH_BHS",     MB91F599B_BS_BH_BHS},
	{"MB91F610_PROG",			MB91F610_PROG},
	{"MB91F610_FONT",           MB91F610_FONT},
	{"MB91F625",                MB91F625},
	{"MB91F627",                MB91F627},
	{"MB91F637",                MB91F637},
	{"MB91F639",                MB91F639},
	{"MB91F635A",               MB91F635A},
	{"MB91F637A",               MB91F637A},
	{"MB91F639A",               MB91F639A},
	{"MB91F644",                MB91F644},
	{"MB91F647",                MB91F647},
	{"MB91F644A",               MB91F644A},
	{"MB91F647A",               MB91F647A},
	{"MB91F662",                MB91F662},
	{"MB91F668_F669",           MB91F668_F669},
	{"MB91F725",				MB91F725},
	{"MB91F775",                MB91F775},
	{"MB91F777",                MB91F777},
	{"MB91F787",                MB91F787},
};

int find_mcu32_by_name(char *s) {
	int rc = 0;
	for (int i = 1; i < MAX_MCU32_TYPE; i++) {
		if ((rc = strcasecmp(mcu32_map[i].name, s)) == 0) {
			return mcu32_map[i].type;
		}
	}
	return E_NOTFOUND;
}

const char *mcu32_name(enum mcu32_type type) {
	for (int i = 1; i < MAX_MCU32_TYPE; i++) {
		if (mcu32_map[i].type == type) {
			return mcu32_map[i].name;
		}
	}
	return NULL;
}

/** List of all available crystal frequencies. */
int frequencies[N_FREQUENCY] = {
	FREQ_3MHZ,
	FREQ_4MHZ,
	FREQ_5MHz,
	FREQ_6MHz,
	FREQ_8MHZ,
	FREQ_10MHz,
	FREQ_12MHz,
	FREQ_16MHZ,
	FREQ_20MHz,
	FREQ_24MHz
};

/**
	Holds configuration for each 32 bit MCU type.
*/
struct chipdef32 chipdefs[MAX_MCU32_TYPE];

/* Example entry:
DownloadFile=m_flash.128
LoadAddress=0x80400
StartAddress=0xC0000
EndAddress=0xFFFFF
FlashSize=0x40000
Clock=10MHz,12.5MHz,13.5MHz,17MHz
Baud=9600,9600,9600,9600
Baud2=57600,38400,57600,115200
*/
int process_chipdef32() {
	FILE *F = NULL;
	char *s;
	char line[256];
	char key[256];
	char value[256];
	int id = 0;
	int rc;

	F = fopen(chipdef_filename, "r");
	if (F == NULL) {
		LOGE("Could not open file '%s' for reading.", chipdef_filename);
		return E_OPEN;
	}

	while ((s = fgets(line, sizeof(line) - 1, F))) {
		s = str_trim(s);
		if (s[0] == '\0' || s[0] == '\n') continue;
		if (s && s[0] == '[' && s[strlen(s) - 1] == ']') {
			s++;
			s[strlen(s) - 1] = '\0';
			for (char *p = s; p && *p; p++) {
				if (*p == '/' || *p == ':' || *p == ' ') {
					*p = '_';
				}
			}
			id = find_mcu32_by_name(s);
			if (id > 0) {
				//LOGD("Configuring MCU %d '%s'", id, mcu32_name(id));
				chipdefs[id].mcu = id;
			} else {
				LOGE("Unknown chipdef heading: '%s'", s);
			}
		} else {
			if (id) {
				if (sscanf(s, "%[^=]=%s", key, value) < 2) {
					LOGE("Invalid chipdef line: '%s'.", s);
					continue;
				}

				if (strcasecmp(key, "DownloadFile") == 0) {
					//Base name of the kernal file aka stage 2 boot loader.
					strncpy(chipdefs[id].kernal, value, sizeof(chipdefs[id].kernal) - 1);
					//LOGD("\t%s.kernal = %s", mcu32_name(id), chipdefs[id].kernal);
				} else if (strcasecmp(key, "LoadAddress") == 0) {
					//Where in RAM to store stage 2 binary. This is usually 0x0990 or 0x0190.
					chipdefs[id].address_load = strtoint32(value, 16, &rc);
					if (rc != E_NONE) {
						LOGE("Conversion error on entry '%s' = '%s' is not a hexadecimal number.", key, value);
						continue;
					}
					//LOGD("\t%s.address_load = 0x%X", mcu32_name(id), chipdefs[id].address_load);
				} else if (strcasecmp(key, "StartAddress") == 0) {
					chipdefs[id].flash_start = strtoint32(value, 16, &rc);
					if (rc != E_NONE) {
						LOGE("Conversion error on entry '%s' = '%s' is not a hexadecimal number.", key, value);
						continue;
					}
					//LOGD("\t%s.flash_start = 0x%X", mcu32_name(id), chipdefs[id].flash_start);
				} else if (strcasecmp(key, "EndAddress") == 0) {
					chipdefs[id].flash_end = strtoint32(value, 16, &rc);
					if (rc != E_NONE) {
						LOGE("Conversion error on entry '%s' = '%s' is not a hexadecimal number.", key, value);
						continue;
					}
					//LOGD("\t%s.flash_end = 0x%X", mcu32_name(id), chipdefs[id].flash_end);
				} else if (strcasecmp(key, "FlashSize") == 0) {
					chipdefs[id].flash_size = strtoint32(value, 16, &rc);
					if (rc != E_NONE) {
						LOGE("Conversion error on entry '%s' = '%s' is not a hexadecimal number.", key, value);
						continue;
					}
					//LOGD("\t%s.flash_size = 0x%X", mcu32_name(id), chipdefs[id].flash_size);
				} else if (strcasecmp(key, "Clock") == 0) {
					char *saveptr = value;
					char *seg;
					char *s = value;
					int c = 0;
					int cv = 0;

					for (c = 0; ; c++, s = NULL) {
						seg = strtok_r(s, ",", &saveptr);
						if (seg == NULL) break;
						//NOTE, assuming value is megahertz
						sscanf(seg, "%d", &cv);
						cv *= 1000000;
						chipdefs[id].clock[c] = cv;
						//LOGD("\t%s.clock[%d] = %u", mcu32_name(id), c, chipdefs[id].clock[c]);
					}
				} else if (strcasecmp(key, "Baud") == 0) {	//List to match clocks.
					char *saveptr = value;
					char *seg;
					char *s = value;
					int b = 0;
					int br = 0;

					for (b = 0; ; b++, s = NULL) {
						seg = strtok_r(s, ",", &saveptr);
						if (seg == NULL) break;
						sscanf(seg, "%d", &br);
						chipdefs[id].bps[b] = br;
						//LOGD("\t%s.bps[%d] = %u", mcu32_name(id), b, chipdefs[id].bps[b]);
					}
				} else if (strcasecmp(key, "Baud2") == 0) {	//List to match clocks.
					char *saveptr = value;
					char *seg;
					char *s = value;
					int b = 0;
					int br = 0;

					for (b = 0; ; b++, s = NULL) {
						seg = strtok_r(s, ",", &saveptr);
						if (seg == NULL) break;
						sscanf(seg, "%d", &br);
						chipdefs[id].bps2[b] = br;
						//LOGD("\t%s.bps[%d] = %u", mcu32_name(id), b, chipdefs[id].bps[b]);
					}
				}
			} else {
				//LOGD("Ignoring '%s'", s);
			}
		}
	}

	fclose(F);
	F = NULL;

	return E_NONE;
}

/** @} */
