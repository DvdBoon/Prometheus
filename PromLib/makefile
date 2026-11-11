# makefile for prometheus.library (GNU make, vbcc)
#
#   make          prometheus.library
#   make debug    prometheus.library.debug: the same plus -DDEBUG; the
#                 D(...) messages of driver_2.c go to the serial port
#                 through exec RawPutChar (driver_2.c, "Debug stuff")
#   make clean
#
# The flags are the ones that built the 4.8 binary in this folder: a build
# with them gives the same file except for the "Build date:" string.
#
# VINC is the m68k target's include folder of vbcc and NDK the NDK's
# include_h; the defaults are the Amiga assigns. On a PC cross setup, e.g.:
#   make VINC=C:/amiga-dev/targets/m68k-amigaos/include \
#        NDK=C:/amiga-dev/targets/m68k-amigaos/ndk/include_h RM="rm -f"

CC     = vc
VINC   = vincludeos3:
NDK    = include_h:
RM     = delete quiet

CFLAGS = +aos68k -cpu=68020 -cpp-comments -O2 -nostdlib \
         -I. -Iinclude -I$(VINC) -I$(NDK)

all: prometheus.library

prometheus.library: driver_2.c
	$(CC) $(CFLAGS) -o $@ driver_2.c

debug: prometheus.library.debug

prometheus.library.debug: driver_2.c
	$(CC) $(CFLAGS) -DDEBUG -o $@ driver_2.c

clean:
	-$(RM) prometheus.library prometheus.library.debug

.PHONY: all debug clean
