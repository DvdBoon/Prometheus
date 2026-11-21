# 3DLabsPermedia2.chip

Picasso96 chip driver for 3DLabs Permedia2 cards (TI TVP4020 RAMDAC) on the
Prometheus bridge. The original source was lost; this is a C reconstruction
from the 7.16 (gcc, with symbols) and 7.17 (vbcc) binaries.

## 7.17 (16.09.03), rebuilt in C

Checked function by function in a 68k emulator (Musashi) against the original
7.17 binary: 7,264 calls, all identical in chip register writes (order and
values), NOP serialisation, exec calls, calls to the Picasso96 *Default
functions, return values, BoardInfo and struct memory, and register
preservation. Not tested: inputs that read past the tables in the original
too (RGB format 21 or higher, minterm above 15), which no caller passes.

## Build

amiga-gcc (bebbo), in this folder:

    make          3DLabsPermedia2.chip
    make clean

`cloud_entry.s` is only used by the emulator test build, not by `make`.

## test/

Emulator test harness (Python 3, `machine68k`, `m68k-linux-gnu-gcc`).
It builds the C source with stack-argument stubs and compares every call
with the original binary. Setup, in `test/`:

    ln -s .. src
    mkdir bin && cp <original 7.17> bin/p2_717.chip
    python3 tchip.py src/chip.c src/blitter.c src/clocks.c src/sprite.c src/tables.c src/cloud_entry.s

`tblit.py`, `tclk.py`, `tspr.py` and `tlib.py` take the same arguments.
