/**
@mainpage

<h3>Kuji32 Flash MCU Programmer</h3>
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

<h3>Introduction</h3>

The programming cycle is in two stages.
Stage 1 (Built-In-Rom on MCU) can accept executable code (the stage 2 binary) and store it in RAM. It can then execute (jump) to RAM.
Stage 2 (KERNAL) is then running from RAM and accepts the actual user application and writes it to FLASH in 512 byte blocks.

Stage 1:
	- Implemented by @link birom32 @endlink
	- Interfaces to ROM code on MCU that can only write to RAM and jump to that code.

Stage 2:
	- Implemented by @link kernal32 @endlink
	- Interface to flash loader that blank checks, erases, reads from and writes to flash.

Both stage 1 and 2 use CRC16.
	- Stage 1 only has a very simple checksum that sums up byte values of the data and negates the result (ones complement).
	- Stage 2 has a sort of standard ITT CRC16. See @link crcitt @endlink

<h3>Command line arguments</h3>
<pre>
-h           Print help and exit.
-H           Print all supported MCUs and exit.
-V           Print application version and exit.
-d           Verbose debugging, prints out all communication using @link hex_dump @endlink
-m \<mcu\>     Select MCU by name e.g. 'mb91f362'. Case-insensitive.
-c \<freq\>    Select target crystal (megahertz) e.g 4, 8, 16 etc.
-e           Erase MCU flash.
-r           Read MCU flash and write it to stdout as S-Records.
-w \<file\>    Write S-Record file to MCU flash.
-v           Verify MCU flash after programming.
-p \<com\>     Set com port Id from 1-99.
-p \<com\>     Set com port device e.g. '/dev/ttyS0'.
</pre>

<h3>Example usage</h3>
Take backup of MCU:
<pre>$./kuji32 -p1 -mmb91f362 -r backupfirmware.mhx</pre>

Erase MCU:
<pre>$./kuji32 -p1 -mmb91f362 -e</pre>

Write S-Records to MCU:
<pre>$./kuji32 -p1 -mmb91f362 -w firmware.mhx</pre>

Do it all in one step:
<pre>$./kuji32 -p1 -mmb91f362 -r backupfirmware.mhx -e -w firmware.mhx</pre>

<h3>Supported MCUs:</h3>
@see mcu32_type

@author Kari Sigurjonsson
@defgroup prog32 32 Bit Programmer
@{
*/
#ifndef __PROG32_H__
#define __PROG32_H__

/**
	Enumeration of 32 bit Fujitsu MCU types.
*/
enum mcu32_type {
	MCU32_INVALID = 0x00,	/**< Not a valid MCU Id. Do not remove. */
	MCU32_MB91F109,
	MCU32_MB91F127,
	MCU32_MB91F128,
	MCU32_MB91F133,
	MCU32_MB91F155,
	MCU32_MB91F158,
	MCU32_MB91F211,
	MCU32_MB91F213,
	MCU32_MB91F223,
	MCU32_MB91F224,
	MCU32_MB91F233,
	MCU32_MB91F248,
	MCU32_MB91F249_S,
	MCU32_MB91F264,
	MCU32_MB91F267,
	MCU32_MB91F272,
	MCU32_MB91F273,
	MCU32_MB91FV310_PROG,
	MCU32_MB91FV310_FONT,
	MCU32_MB91F312,
	MCU32_MB91F313,
	MCU32_MB91F314,
	MCU32_MB91F318R,
	MCU32_MB91FV319A_PROG,
	MCU32_MB91FV319A_FONT,
	MCU32_MB91FV319R_PROG,
	MCU32_MB91FV319R_FONT,
	MCU32_MB91F345,
	MCU32_MB91F346,
	MCU32_MB91F353,
	MCU32_MB91F355,
	MCU32_MB91F356B,
	MCU32_MB91F362,
	MB91F365,
	MB91F366,
	MB91F367,
	MB91F368,
	MB91F369,
	MB91F463C,
	MB91F463NA,
	MB91F463NC,
	MB91F464A,
	MB91F464H,
	MB91F465B,
	MB91F465C,
	MB91F465D,
	MB91F465K,
	MB91F465P,
	MB91F465X,
	MB91F467B,
	MB91F467C,
	MB91F467D,
	MB91F467M,
	MB91F467R,
	MB91F467S,
	MB91F467T,
	MB91F469G,
	MB91F475,
	MB91F478,
	MB91F479,
	MB91F482,
	MB91F486,
	MB91F487,
	MB91F492,
	MB91F522B_D_F_J_K_L,
	MB91F523B_D_F_J_K_L,
	MB91F524B_D_F_J_K_L,
	MB91F525B_D_F_J_K_L,
	MB91F526B_D_F_J_K_L,
	MB91F575_S,
	MB91F577_S,
	MB91F583MG_MH_MJ_MK,
	MB91F583SG_SH_SJ_SK,
	MB91F584MG_MH_MJ_MK,
	MB91F584SG_SH_SJ_SK,
	MB91F585MG_MH_MJ_MK,
	MB91F585SG_SH_SJ_SK,
	MB91F585LA_LB_LC_LD,
	MB91F586LA_LB_LC_LD,
	MB91F587LA_LB_LC_LD,
	MB91F591B_BS_BH_BHS,
	MB91F592B_BS_BH_BHS,
	MB91F594B_BS_BH_BHS,
	MB91F599B_BS_BH_BHS,
	MB91F610_PROG,
	MB91F610_FONT,
	MB91F625,
	MB91F627,
	MB91F637,
	MB91F639,
	MB91F635A,
	MB91F637A,
	MB91F639A,
	MB91F644,
	MB91F647,
	MB91F644A,
	MB91F647A,
	MB91F662,
	MB91F668_F669,
	MB91F725,
	MB91F775,
	MB91F777,
	MB91F787,
	//--
	MAX_MCU32_TYPE			/**< Sentinel. */
};

/** Enumeration of failure codes this program returns. */
enum failures32 {
	/**
		Timed out waiting for processor.
		This usually means the MCU has no power, program jumper not in place or
		connection failure in RS232 cable.
	*/
	EXIT_FAIL_TIMEOUT		= 1,	/**< Error time-out waiting for MCU. */
	EXIT_FAIL_NOTBLANK		= 2,	/**< Was asked to blank check or write and the MCU was NOT blank. */
	EXIT_FAIL_ISBLANK		= 3,	/**< Was asked to blank check or read and the MCU was blank. */
	EXIT_FAIL_READ			= 4,	/**< Error reading from MCU. */
	EXIT_FAIL_WRITE			= 5,	/**< Error writing to MCU. */
	EXIT_FAIL_SRECORD		= 6,	/**< Error in S-Record either file I/O or its data. */
	EXIT_FAIL_CRC			= 7,	/**< Error in communication detected by CRC. */
	EXIT_FAIL_SERIAL		= 8,	/**< Error in serial port such as access restrictions or errors in reading or writing. */
	EXIT_FAIL_CHIPDEF		= 9,	/**< Error in 'chipdef32.ini' either reading from it or in its data. */
	EXIT_FAIL_ARGUMENT		= 10,	/**< Error in one of the arguments either missing or invalid. */
	EXIT_FAIL_INITBIROM		= 11,	/**< Error initializing Birom32 interface. */
	EXIT_FAIL_INITKERNAL	= 12,	/**< Error initializing Kernal interface. */
	EXIT_FAIL_BLANK			= 13,	/**< Error blank-checking MCU. */
	EXIT_FAIL_ERASE			= 14,	/**< Error erasing MCU. */
};

/** MCU nametag. */
struct mcu32_tag {
	char *name;				/**< MCU name as single word. */
	enum mcu32_type type;	/**< MCU type Id. */
};

/** Array of MCU names and associated type identifiers. */
extern struct mcu32_tag mcu32_map[];

/** Enumeration of supported crystal frequencies. */
enum frequency {
	FREQ_3MHZ		= 3000000,	/**< 3 megahertz. */
	FREQ_4MHZ		= 4000000,	/**< 4 megahertz. */
	FREQ_5MHz		= 5000000,	/**< 5 megahertz. */
	FREQ_6MHz		= 8000000,	/**< 8 megahertz. */
	FREQ_8MHZ		= 8000000,	/**< 8 megahertz. */
	FREQ_10MHz		= 10000000,	/**< 10 megahertz. */
	FREQ_12MHz		= 12000000,	/**< 12 megahertz. */
	FREQ_16MHZ		= 16000000,	/**< 16 megahertz. */
	FREQ_20MHz		= 20000000, /**< 20 megahertz. */
	FREQ_24MHz		= 24000000,	/**< 24 megahertz. */
};

/** Total number of frequencies supported. This is to size arrays and such. */
#define N_FREQUENCY		10

/** List of all available crystal frequencies. */
int frequencies[N_FREQUENCY];

/**
	A list of valid bit rates.
*/
enum bps {
	BPS_INVALID	= 0,
	BPS_110		= 110,
	BPS_150		= 150,
	BPS_300		= 300,
	BPS_1200	= 1200,
	BPS_2400	= 2400,
	BPS_4800	= 4800,
	BPS_9600	= 9600,
	BPS_19200	= 19200,
	BPS_38400	= 38400,
	BPS_57600	= 57600,
	BPS_115200	= 115200,
	BPS_230400	= 230400,
	BPS_460800	= 460800,
	BPS_921600	= 921600,
};

/** Total number of bit rates supported. This is to size arrays and such. */
#define N_BPS	15

/** Configuration of a 32 bit MCU. */
struct chipdef32 {
	enum mcu32_type mcu;				/**< Tell what MCU this entry is for. */
	char kernal[256];					/**< Base name of the kernal file aka stage 2 boot loader. */
	enum frequency clock[N_FREQUENCY];	/**< Array of valid crystal frequencies when stage 1 is running. */
	enum bps bps[N_FREQUENCY];			/**< Array of valid baud rates for stage 1. This should complement clock[] item by item. */
	enum bps bps2[N_FREQUENCY];			/**< Array of valid baud rates for stage 2. This should complement clock[] item by item. */
	uint32_t address_load;				/**< Where in RAM to store stage 2 binary. */
	uint32_t flash_start;				/**< Where in FLASH to store user firmware. 24 bit range. */
	uint32_t flash_end;					/**< Last address of flash. 24 bit range. */
	uint32_t flash_size;				/**< Flash size (flash_end - flash_start). 24 bit range. */
	uint32_t baud1;						/**< Baud rate for stage 1 Built-In-ROM. */
	uint32_t baud2;						/**< Baud rate for stage 2 Kernal. */
};

/**
	Holds configuration for each 32 bit MCU type.
	This array is initialized with data from 'chipdef32.xml'.
*/
extern struct chipdef32 chipdefs[MAX_MCU32_TYPE];

/**
This mambo processes 'chipdef32.xml' file that describes each supported processor.
The global array chipdefs[] receives the processed data.
@return On success, returns E_NONE.
@return On failure, returns a negative error code.
*/
int process_chipdef32();

/**
	Find MCU type by name.
	@param s MCU name.
	@return On success, returns the MCU identifier.
	@return On failure i.e. no such MCU, returns a negative error code.
*/
int find_mcu32_by_name(char *s);

/**
	Look up common MCU name given a type identifier.
	@param type MCU type identifier.
	@return On success, returns the name.
	@return On failure i.e. invalid type, returns NULL.
*/
const char *mcu32_name(enum mcu32_type type);

#endif //__PROG32_H__

/** @} */
