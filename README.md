# Prometheus

Components for the Prometheus PCI bridge with the FireStorm firmware.

| Folder | What | Version |
|---|---|---|
| `PromLib/` | `prometheus.library`, the PCI bridge library | 4.8 |
| `PrometheusCard/` | `Picasso96/Prometheus.card`, the card driver for graphics cards on the bridge (Voodoo, ViRGE, Permedia2), with the DMA memory functions used by `Prm_AllocDMABuffer` | 8.0 |
| `ne2000/` | `prm-rtl8029.device`, the SANA-II driver for RTL8029 (NE2000) network cards | see `ne2000/release_notes` |

Built binaries are included next to the sources:
`PromLib/prometheus.library`,
`PrometheusCard/bin/vbcc-classic/Prometheus.card` and
`ne2000/prm-rtl8029.device`.

## prometheus.library 4.8

`driver_2.c` is the library; `driver_2_47_to_48.diff` shows the changes
from 4.7.

Build with GNU make and vbcc in `PromLib/`:

    make          prometheus.library
    make debug    prometheus.library.debug (-DDEBUG)
    make clean

The debug build prints its `D(...)` messages to the serial port, through
exec's `RawPutChar`.

In the debug build only, the delay before each card is probed (100000
microseconds) can be set with `ENV:Prometheus/CardDelay`, in microseconds:
decimal (`setenv Prometheus/CardDelay 200000`) or hexadecimal with a `0x`
or `$` prefix (`0x30D40`). It is read once when the library loads and
printed on serial as `[LibInit] CardDelay ...`; copy it to `ENVARC:` to
keep it. The normal build always uses 100000. The makefile's flags are the ones the included 4.8
binary was built with. The include folders default to the Amiga assigns
`vincludeos3:` and `include_h:`; on a PC cross setup pass them, e.g.
`make VINC=C:/amiga-dev/targets/m68k-amigaos/include NDK=C:/amiga-dev/targets/m68k-amigaos/ndk/include_h RM="rm -f"`.

## Prometheus.card 8.0

Changes since 7.603:
- the DMA list always exists, so `Prm_AllocDMABuffer` returns NULL
  instead of reading address 0 when no `DMASIZE` area is set up;
- `FreeDMAMemory` releases its semaphore on every return, so a bad free
  no longer hangs later DMA calls;
- new `AddDMAMemory(memory, size)` (LVO -54): another card driver can
  hand over part of its video memory as the DMA area. Open the card with
  version 8 to use it. Caller headers (proto/inline, no clib) are in
  `PrometheusCard/include/`.

Built with vbcc (`vc +aos68k -cpu=68020 -cpp-comments -c99 -nostdlib -O2`).
