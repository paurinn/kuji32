#Kuji32 Flash MCU Programmer
#Copyright (C) 2014 Kari Sigurjonsson

#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <http://www.gnu.org/licenses/>.

###############################################################################

#To get a fully verbose make output do declare 'AT' at the command line like so: 'make AT='.
#By default AT is undefined and thus assigned the string '@'.
AT ?= @

# Attach debugging symbols and activate debug printout.
DEBUGGING	= false

# Base name of the binary executable. Version numbers are appended to final binary name.
OUTPUT		= kuji32

# Any platform neutral options to the compiler.
# Platform specific stuff is in makefile.$(shell uname -s).
CFLAGS = -Wall -Wextra -Werror -Wfatal-errors -Wno-unused-parameter -Wno-unused-variable --std=gnu99 -I./include -DCONSOLE

# Any platform neutral options to the linker.
#LDFLAGS =

#RULES += <rules_list>		# List of make rules to run but after the default.

#AFTER_INSTALL	+= <shell commands> # Command line to execute as the last step in the install directive.

# List of source files to compile.
SRCS =	\
		stdafx.c \
		errorcode.c \
		version.c \
		log.c \
		util.c \
		serial.c \
		srec.c \
		prog32.c \
		birom32.c \
		kernal32.c \
		main32.c

###############################################################################

#Version is stored in the file VERSION.
MAJOR	= $(shell sed -n '1p' VERSION | cut -d= -f2)
MINOR	= $(shell sed -n '2p' VERSION | cut -d= -f2)
BUILD	= $(shell sed -n '3p' VERSION | cut -d= -f2)
COMPANY	= $(shell sed -n '4p' VERSION | cut -d= -f2)

SVNREVISION = $(shell svn info 2>/dev/null | grep '^Revision' | sed -e 's/Revision: *//')
ifeq "$(SVNREVISION)" ""
SVNREVISION = 0
endif

CFLAGS += -DREVISION=$(SVNREVISION) -DBUILD_TIMESTAMP=$(shell date '+%s') -DMAJORVERSION=$(MAJOR) -DMINORVERSION=$(MINOR) -DBUILD=$(BUILD)

# Select debugging or optimized compiler flags.
ifeq ($(DEBUGGING), true)
CFLAGS += -O0 -g3 -ggdb3 -DDEBUGGING
else
CFLAGS += -O3
endif

INSTALLDIR = $(PREFIX)/bin

#Stage directory is used to build a zip package of the final binary + documentation.
STAGEDIR = $(OUTPUT)_stage

#Name of zipped file with executable and documentation.
ZIPOUT = $(OUTPUT).$(MAJOR).$(MINOR).zip

CLEANFILES += $(OUTPUT)$(EXT) *.core gmon.out $(OBJS) $(OUTPUT).sha1 $(EXTRACLEAN) doc/*.tmp *.tmp
MRPROPERFILES += $(CLEANFILES) doc/latex *.log
DISTCLEANFILES += $(MRPROPERFILES) html $(STAGEDIR) $(ZIPOUT) kernal32/m_flash.* chipdef32.ini

STAGEFILES += $(OUTPUT)$(EXT) html LICENSE README HISTORY VERSION doc *.c include Makefile makefile.* buildcounter.lua

#Include system specific Makefile. This is based on kernel name from 'uname -s'.
#The basic declarations can be overwritten to suit each system.
KERNEL=$(shell uname -s)
TOP := $(dir $(lastword $(MAKEFILE_LIST)))
include $(TOP)/makefile.$(KERNEL)

### Rules
.SUFFIXES : .c .o
.PHONY: info doc clean mrproper dist distclean help prep buildcounter cloc

RULES += $(OBJS) $(OUTPUT)$(EXT)

OBJS = $(SRCS:.c=.o)

all: $(RULES)

.c.o:
	$(ECHO) "[COMPILE] $@"
	$(AT)$(CC) $(CFLAGS) -c -o $*.o $<

$(OUTPUT)$(EXT): $(OBJS)
	$(ECHO) "[LINKING] $(OUTPUT)$(EXT)"
	$(AT)$(LD) $(OBJS) $(LDFLAGS) -o $(OUTPUT)$(EXT)

info:
	$(ECHO) "Source to build for OUTPUT):"
	$(AT)ls -1lh $(SRCS)
	$(ECHO) ""
	$(ECHO) "Files included in $(ZIPOUT):"
	$(ECHO) $(STAGEFILES)

doc:
	$(ECHO) "[DOXYGEN] doc"
	$(AT)doxygen doc/Doxyfile

clean:
	$(ECHO) "[CLEAN] $(CLEANFILES)"
	$(AT)$(RM) -rf $(CLEANFILES)

mrproper:
	$(ECHO) "[MRPROPER] $(MRPROPERFILES)"
	$(AT)$(RM) -rf $(MRPROPERFILES)

cloc:
	$(AT)cloc .

buildcounter:
	$(AT)./buildcounter.lua

distclean:
	$(ECHO) "[DISTCLEAN] $(DISTCLEANFILES)"
	$(AT)$(RM) -rf $(DISTCLEANFILES)

dist: distclean buildcounter cloc $(RCOBJ) $(OUTPUT)$(EXT) doc
	$(AT)mkdir -p $(STAGEDIR)
	$(AT)$(CP) -r $(STAGEFILES) $(STAGEDIR)
	$(ECHO) "[ZIP] $(STAGEFILES) > $(ZIPOUT)"
	$(AT)$(ZIP) -qr9 $(ZIPOUT) $(STAGEDIR)

prep:
	$(AT)$(CP) /c/Program\ Files\ \(x86\)/FUJITSU/FUJITSU\ FLASH\ MCU\ Programmer/FR/m_flash.* kernal32/
	$(AT)$(CP) /c/Program\ Files\ \(x86\)/FUJITSU/FUJITSU\ FLASH\ MCU\ Programmer/FR/chipdef.ini chipdef32.ini

help:
	$(AT)$(CAT) readme.txt

