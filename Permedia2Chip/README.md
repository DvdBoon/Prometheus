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

## 7.18 (20.11.26)

Built with amiga-gcc; the tested binary has md5 8c3c1286fb2c3c7773651283d1dccf91.

1. **Engine lock shared with Warp3D.** InitChip creates a public
   SignalSemaphore "3DLabsPermedia2.chip engine" (never removed). Every
   hardware blit does AttemptSemaphore. If Warp3D holds it, or the calling
   task is the one holding it, the blit is drawn by the CPU (the Picasso96
   *Default function), so 2D never touches the 3D state and never waits.
   Memory layout, agreed with the Warp3D driver:
   offset 0 `struct SignalSemaphore`, offset 46 `ULONG dirty3D`,
   offset 50 the name. Warp3D sets dirty3D to 1 before ReleaseSemaphore;
   the next blit reloads the 34 graphics-core registers it relies on to
   their InitChip values and clears the flag.
2. **Sync polarity from the screen mode** (GMF_HPOLARITY/GMF_VPOLARITY).
   7.16 and 7.17 tested a BoardInfo flag (BIF_CACHEMODECHANGE) instead.
   The chip always outputs +HSync/+VSync, because the RAMDAC hardware
   cursor needs +VSync, and the RAMDAC MiscControl register inverts them
   for the monitor (as Linux pm2fb does).
3. **Memory sizing** no longer writes one long past the end of card
   memory and no longer counts a failing megabyte as present.
4. **Debug Alerts removed** from CalculateBytesPerRow and CalculateMemory.
5. **FIFO waits:** BlitTemplate waited for 14 slots but writes up to 16;
   BlitRectNoMaskComplete wrote FBReadMode before its FIFO wait.

Register names fixed: 0x8678 is TexelLUTMode and 0x8690 is FogMode.

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
