# 3DLabsPermedia2.chip 7.17 - bebbo amiga-gcc
# make            -> 3DLabsPermedia2.chip
# Needs: m68k-amigaos-gcc in PATH (D:\amiga-gcc), NDK headers from amiga-gcc.

CC      = m68k-amigaos-gcc
CFLAGS  = -m68020 -O2 -fomit-frame-pointer -fno-builtin -Wall -Wno-pointer-sign -I.
LDFLAGS = -nostartfiles -nostdlib -s
LIBS    = -lgcc

# entry.o must stay first
OBJS    = entry.o libinit.o chip.o blitter.o clocks.o sprite.o tables.o

TARGET  = 3DLabsPermedia2.chip

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

entry.o: entry.s
	$(CC) -m68020 -c $< -o $@

%.o: %.c p2chip.h boardinfo.h picasso96.h settings.h version.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
