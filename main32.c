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

/** License clause. */
const char *license = "\
\n\
Copyright (C) 2014  Kari Sigurjonsson\n\
\n\
This program is free software: you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation, either version 3 of the License, or\n\
(at your option) any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program.  If not, see <http://www.gnu.org/licenses/>.\n";

/** Help clause. */
const char *help = "\
\n\
--------------------------------\n\
Usage: ./kuji32 -m <mcu> -p <com> [-t <seconds>] [-v] [-d] [-c <freq>] [-r <file>] [-e] [-w <file>]\n\
  -h         Print help and exit.\n\
  -H         Print all supported MCUs and exit.\n\
  -V         Print application version and exit.\n\
  -d         Hex-dump communication to console.\n\
  -v         Select verbosity level: 0-none, 1-errors, 2-warnings, 3-info, 4-debug.\n\
  -t <sec>   Select discovery timeout in seconds. Default is 5 seconds.\n\
  -l <file>  Write log to <file> instead of main.log.\n\
  -p <com>   Set com port Id from 1-99 on Windows or com port device e.g. '/dev/ttyS0' on Linux.\n\
  -m <mcu>   Select MCU by name e.g. 'mb90f598g'. Case-insensitive.\n\
  -c <freq>  Select target crystal (megahertz) e.g 4, 8, 16 etc. Default is 4 Mhz.\n\
  -b         Blank-check and exit immediately after.\n\
  -r <file>  Read MCU flash and write it file as S-Records.\n\
  -e         Erase MCU flash.\n\
  -w <file>  Write S-Record file to MCU flash.\n\
\n\
Example: ./kuji32 -m mb91f362 -p1 -e -w firmware.mhx\n\
\n\
To print out supported MCUs: ./kuji32 -H\n\
\n\
Note: The file must be standard Motorola S-Record.\n\
\n\
Exit codes:\n\
--------------\n\
1 : Timed out waiting for processor. Check cable and stuff.\n\
2 : Was asked to blank check or write and the MCU was NOT blank.\n\
3 : Was asked to blank check or read and the MCU was blank.\n\
4 : Error reading from MCU.\n\
5 : Error writing to MCU.\n\
6 : Error in S-Record either file I/O or its data.\n\
7 : Error in communication detected by CRC.\n\
8 : Error in serial port such as access restrictions or errors in reading or writing.\n\
9 : Error in 'chipdef32.ini' either reading from it or in its data.\n\
10 : Error in one of the arguments either missing or invalid.\n\
11 : Error initializing Birom32 interface.\n\
12 : Error initializing Kernal interface.\n\
13 : Error blank-checking MCU.\n\
14 : Error erasing MCU.\n\
";

/** This structure contains the results of command line argument parser. */
struct params32 {
	char *argstr;		/**< Control string for getopt(). */
	char *srecpath;		/**< Parameter given to '-w'. */
	char *savepath;		/**< Parameter given to '-r'. */
	char *comarg;		/**< Parameter given to '-p'. */

	bool erase;			/**< User requested erase with '-e'. */
	bool read;			/**< User requested read with '-r'. */
	bool write;			/**< User requested write with '-w'. */
	bool blankcheck;	/**< User requested blank with '-b'. */
	bool debugging;		/**< User requested debugging output with '-d'. */

	int timeoutsec;		/**< Parameter given to '-t'. */
	enum frequency freq;	/**< Currently selected target crystal frequency. */
	struct chipdef32 *chip;	/**< MCU descriptor. */
	int freqid;			/**< Index into chip->clock[], chip->bps[] and chip->bps2[]. */
};

/**
Print out usage information to stdout.
*/
void print_help() {
	fprintf(stderr, "%s%s%s",
		version_string(),
		help,
		license
	);
}

/**
	Test if all bytes in a given buffer are 0xFF.
	@param buf Buffer of bytes to test.
	@param size Size of buf[].
	@return If buffer only contains 0xFF, returns true.
*/
bool isflashbufempty(uint8_t *buf, int size);

inline bool isflashbufempty(uint8_t *buf, int size) {
	while (size--) {
		if (*buf != 0xFF) return false;
		buf++;
	}
	return true;
}

/**
Process command line parameters.
@param argc Argument count.
@param argv Argument vector.
@param params Destination for the parsed parameters.
@return On success, returns E_NONE.
@return On failure, returns an exit() status.
*/
int process_params32(int argc, char *argv[], struct params32 *params) {
	int id;
	int opt;

	memset(params, 0x00, sizeof(struct params32));
	params->argstr = "hHVdt:l:v:p:m:c:ber:w:";

	while ((opt = getopt(argc, argv, params->argstr)) != -1) {
		switch (opt) {
			case 'h':
				print_help();
				return EXIT_SUCCESS;

			case 'H':
				LOGI("\nSupported MCUs:");
				for (id = 1; id < MAX_MCU32_TYPE; id++) {
					LOGI("%s", mcu32_map[id].name);
				}
				LOGI("--------------------------");
				return EXIT_SUCCESS;

			case 'V':
				fprintf(stderr, "%s\n%s\n", version_string(), license);
				return EXIT_SUCCESS;

			case 'd':
				params->debugging = true;
				break;

			case 't':
				params->timeoutsec = strtoint32(optarg, 10, NULL);
				break;

			case 'v':
				verbosity = strtoint32(optarg, 10, NULL);
				break;

			case 'l':
				if (optarg) {
					loggpath = optarg;
				} else if (argv[optind] && argv[optind][0] && argv[optind][0] != '-') {
					loggpath = argv[optind];
					optind++;
				}
				LOGD("Using '%s' for log.", loggpath);
				break;

			case 'p':
				params->comarg = optarg;
				break;

			case 'm':
				if (optarg) {
					id = find_mcu32_by_name(optarg);
					if (id > 0) {
						params->chip = &chipdefs[id];
					} else {
						LOGE("ERROR: Invalid option '%s' to -m. This is not a MCU name.", optarg);
						return EXIT_FAIL_ARGUMENT;
					}
				}
				break;

			case 'c':
				if (optarg) {
					id = strtoint32(optarg, 10, NULL);
					id *= 1000000;
					for (int i = 1; i < N_FREQUENCY; i++) {
						if (frequencies[i] == id) {
							params->freq = frequencies[i];
						}
					}
				}
				break;

			case 'b':
				params->blankcheck = true;
				break;

			case 'r':
				if (optarg && optarg[0]) {
					params->savepath = optarg;
					params->savepath = NULL;
					params->read = true;
				}
				break;

			case 'e':
				params->erase = true;
				break;

			case 'w':
				if (optarg && optarg[0]) {
					params->srecpath = optarg;
					params->write = true;
				}
				break;

			case '?':
				LOGE("Argument error!");
				return EXIT_FAIL_ARGUMENT;
		}
	}

	if (params->comarg == NULL) {
		LOGE("Missing or invalid option '-p'.");
		print_help();
		return EXIT_FAIL_ARGUMENT;
	}

	if (params->timeoutsec < 1 || params->timeoutsec > 60) {
		LOGW("Invalid timeout, setting default 5 seconds.");
		params->timeoutsec = 5;
	}

	if (params->chip == NULL) {
		LOGE("ERROR: Missing or invalid option '-m'.\n");
		print_help();
		return EXIT_FAIL_ARGUMENT;
	}

	//Morph frequency into MCU specific index that we select bps by.
	//This is because serial port bitrate follows clock selection.
	if (params->freq <= 0) {
		params->freq = FREQ_4MHZ;
		LOGD("Using default frequency %dHz.", params->freq);
	}

	for (id = 0; id < N_FREQUENCY; id++) {
		if (params->chip->clock[id] == params->freq) {
			params->freqid = id;
			break;
		}
	}

	if (params->freqid < 0) {
		LOGE("The clock frequency '%d' is not supported on MCU '%s'.", params->freq, mcu32_name(params->chip->mcu));
		return EXIT_FAIL_ARGUMENT;
	}


	return E_NONE;
}

/**
	Main entry point.
	Process chipdef file, parse user options and run the programming state machine.
*/
int main(int argc, char *argv[]) {
	int bps = 0;
	int id = 0;
	int rc;
	int bytes = 0;
	uint16_t csum = 0;
	struct serial serial;

	char compath[256];

	bool isblank;

#ifdef __WIN32__
	int comid = 0;
#endif

	struct params32 params;
	if ((rc = process_params32(argc, argv, &params)) != E_NONE) {
		LOGE("Fatal error!");
		return rc;
	}

	if (process_chipdef32() != E_NONE) {
		return EXIT_FAIL_CHIPDEF;
	}

	memset(compath, 0x00, sizeof(compath));

	//NOTE, bps follows the clock, this is configured in 'chipdef32.ini'.
	bps = params.chip->bps[params.freqid];

	if (bps <= 0) {
		LOGE("Internal error: Invalid bitrate.");
		return EXIT_FAIL_ARGUMENT;
	}

#ifdef __WIN32__
	comid = strtoint32(params.comarg, 10, &rc);
	if (rc != E_NONE || comid <= 0 || comid >= 100) {
		LOGE("Missing or invalid option '-p'.");
		print_help();
		return EXIT_FAIL_ARGUMENT;
	}
	snprintf(compath, sizeof(compath) - 1, "\\\\.\\COM%d:%d:8N1", comid, params.chip->bps[params.freqid]);
#else
	snprintf(compath, sizeof(compath) - 1, "%s:%d:8N1", params.comarg, params.chip->bps[params.freqid]);
#endif

	if (params.write && params.srecpath == NULL) {
		LOGE("ERROR: Missing or malformed option to '-w'.");
		print_help();
		return EXIT_FAIL_ARGUMENT;
	}

	if (params.read && params.savepath == NULL) {
		LOGE("ERROR: Missing or malformed option to '-r'.");
		print_help();
		return EXIT_FAIL_ARGUMENT;
	}

	//Open up serial port for birom.
	memset(&serial, 0x00, sizeof(struct serial));
	LOGD("Compath: '%s'", compath);
	rc = serial_open(&serial, compath);
	if (rc != E_NONE) {
		LOGE("Error opening serial port.");
		return EXIT_FAIL_SERIAL;
	}

	serial.debug = params.debugging;

	/****************************************************************************
	  Stage 1 BIROM (Built-In-ROM).
	 ****************************************************************************/

	struct birom32_state *birom = NULL;

	LOGD("---------- BIROM32 START ----------");

	rc = birom32_new(&birom, params.chip, &serial);
	if (rc != E_NONE) {
		birom32_free(&birom);
		return EXIT_FAIL_INITBIROM;
	}

	//First handshake is to give user chance to power up MCU.
	LOGI("Probing for MCU. Please apply power to board...");

	//Is the audience listening?
	rc = birom32_connect(birom, params.timeoutsec);
	if (rc != E_NONE) {
		birom32_free(&birom);
		return EXIT_FAIL_TIMEOUT;
	}

	//Check if ROM is ready.
	rc = birom32_check(birom);
	if (rc != E_NONE) {
		birom32_free(&birom);
		return EXIT_FAIL_TIMEOUT;
	}

	//Dump stage 2 binary into MCU RAM.
	rc = birom32_write(birom, params.chip->address_load, birom->kernaldata, birom->kernalsize);
	if (rc != E_NONE) {
		birom32_free(&birom);
		return EXIT_FAIL_WRITE;
	}

	//Check if birom is still there.
	rc = birom32_check(birom);
	if (rc != E_NONE) {
		birom32_free(&birom);
		return EXIT_FAIL_TIMEOUT;
	}

	// Call and transfer control to stage 2 kernal.
	rc = birom32_call(birom, params.chip->address_load);
	if (rc != E_NONE) {
		birom32_free(&birom);
		return EXIT_FAIL_TIMEOUT;
	}

	birom32_free(&birom);

	LOGD("---------- BIROM32 DONE ----------\n");

	//Up the baud rate up a notch. BAM!
	rc = serial_setbaud(&serial, params.chip->bps2[params.freqid] > 0 ? params.chip->bps2[params.freqid] : 115200);
	if (rc != E_NONE) {
		LOGE("Error setting baudrate.");
		return EXIT_FAIL_SERIAL;
	}

	/****************************************************************************
	  Stage 2 KERNAL
	 ****************************************************************************/

	struct kernal32 *kernal = NULL;

	LOGD("========== KERNAL32 START ==========");

	rc = kernal32_new(&kernal, params.chip, &serial);
	if (rc != E_NONE) {
		kernal32_free(&kernal);
		return EXIT_FAIL_INITKERNAL;
	}

	//Test for Stage 2 presence.
	rc = kernal32_intro(kernal);
	if (rc != E_NONE) {
		kernal32_free(&kernal);
		return EXIT_FAIL_TIMEOUT;
	}

	//Always blank-check.
	rc = kernal32_blankcheck(kernal, params.chip->flash_start);
	if (rc < 0) {
		LOGE("ERROR: Could not perform blank check!");
		kernal32_free(&kernal);
		return EXIT_FAIL_BLANK;
	}
	isblank = (rc == 1);

	//Exit early if chip is full and there are no operations or
	//if chip is empty and operations would not be possible.
	if (
		(params.blankcheck)
		|| (isblank && (!params.write))
		|| ((isblank && (params.read || params.erase) && !params.write))
		|| (params.read && isblank)
		|| (!isblank && (!params.write && !params.read && !params.erase))
	) {
		LOGI("== Chip Is %s ==", (isblank) ? "Blank" : "Not Blank");
		LOGD("========== KERNAL32 DONE ==========");
		kernal32_free(&kernal);
		return isblank ? EXIT_FAIL_ISBLANK: EXIT_FAIL_NOTBLANK;
	}

	//Read flash contents
	if (params.read) {
		if (isblank) {
			LOGI("== Chip Is Blank ==");
			LOGD("========== KERNAL32 DONE ==========");
			kernal32_free(&kernal);
			return EXIT_FAIL_BLANK;
		}

		//Sector by sector!
		uint8_t *buff = calloc(1, params.chip->flash_size * 2);
		assert(buff);

		bytes = 0;
		csum = 0;

		LOGR("[INF]: Reading ");
		for (uint32_t addr = params.chip->flash_start; addr < params.chip->flash_start + params.chip->flash_size; addr += 0x200, bytes += 512) {
			rc = kernal32_readflash(kernal, addr, buff + bytes, 512, &csum);
			if (rc != E_NONE) {
				LOGE("Error receiving flash contents.");
				kernal32_free(&kernal);
				return EXIT_FAIL_READ;
			}

#ifdef DEBUGGING
			LOGR("\rReceiving 512 bytes from sector 0x%06X, last CRC16 0x%04X", addr, csum);
#else
			if ((bytes % 32768) == 0) {
				LOGR("#");
			}
#endif
		}

		LOGR("\n");

		rc = srec_writefilebin(buff, bytes, params.savepath, 2, params.chip->flash_start);
		if (rc != E_NONE) {
			LOGE("Error serializing S-Record.");
			kernal32_free(&kernal);
			return EXIT_FAIL_SRECORD;
		}

		free(buff);
		buff = NULL;

		LOGI("== Chip Read Successfully ==");
	}

	//Return early is chip is full and trying to write without erasing first.
	if (!isblank && params.write && !params.erase) {
		LOGE("Error: Trying to write into an already full MCU. Did you forget to add '-e' argument?");
		LOGD("========== KERNAL32 DONE ==========");
		kernal32_free(&kernal);
		return EXIT_FAIL_NOTBLANK;
	}

	//Erase chip.
	if (params.erase && !isblank) {
		LOGR("[INF]: Erasing ");
		rc = kernal32_erasechip(kernal, params.chip->flash_start);
		if (rc != E_NONE) {
			LOGR("\n");
			LOGE("ERROR: Could not erase flash!");
			kernal32_free(&kernal);
			return EXIT_FAIL_ERASE;
		}
		LOGR("\n");
		LOGI("== Chip Erased ==");
	}

	//Program S-Records.
	if (params.write) {
		uint8_t *buf = NULL;

		//Copy flash data from S-Records in file into a linear buffer.
		//The buffer is already 2^24 bytes so we can index it directly from params.chip->flash_start to params.chip->flash_end inclusively.
		rc = srec_readfilebin(&buf, params.srecpath, params.chip->flash_start, params.chip->flash_end);
		if (rc != E_NONE || buf == NULL) {
			LOGE("ERROR: Could not interpret S-Records from file '%s'.", params.srecpath);
			kernal32_free(&kernal);
			return EXIT_FAIL_SRECORD;
		}

		LOGD("Loaded S-Records from '%s'.", params.srecpath);

		//Calculate rough size of transfer.
		uint32_t size = 0;
		for (uint32_t addr = params.chip->flash_start; addr < params.chip->flash_end; addr += 512, bytes += 512) {
			if (isflashbufempty(buf + addr, 512) == false) {
				size += 512;
			}
		}
#ifdef DEBUGGING
		LOGD("Writing %d bytes...", size);
#else
		LOGR("[INF]: Writing ");
#endif

		//Write out linear buffer into MCU flash in 512 byte chunks.
		bytes = 0;
		uint16_t crc;
		for (uint32_t addr = params.chip->flash_start; addr < params.chip->flash_end; addr += 512, bytes += 512) {
			if (isflashbufempty(buf + addr, 512) == false) {
				rc = kernal32_writeflash(kernal, addr, buf + addr, 512, &crc);
				if (rc != E_NONE) {
					kernal32_free(&kernal);
					return EXIT_FAIL_WRITE;
				}

#ifdef DEBUGGING
				LOGR("\rSector 0x%06X CRC: %04X", addr, crc);
#else
				if ((bytes % 32768) == 0) {
					LOGR("#");
				}
#endif
			}
		}
		LOGR("\n");

		free(buf);
		buf = NULL;

		LOGI("== Chip Programmed Successfully ==");

	}

	kernal32_free(&kernal);

	LOGD("========== KERNAL32 DONE ==========");

	return EXIT_SUCCESS;
}
/** @} */

