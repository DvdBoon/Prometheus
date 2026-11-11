# Prometheus

Components for the Prometheus PCI bridge with the FireStorm firmware.

| Folder | What | Version |
|---|---|---|
| `promlib48/` | `prometheus.library`, the PCI bridge library | 4.8 |
| `PrometheusCard-prmfirst/` | `Picasso96/Prometheus.card`, the card driver for graphics cards on the bridge (Voodoo, ViRGE, Permedia2), with the DMA memory functions used by `Prm_AllocDMABuffer` | 8.0 |
| `ne2000/` | `prm-rtl8029.device`, the SANA-II driver for RTL8029 (NE2000) network cards | see `ne2000/release_notes` |

Built binaries are included next to the sources:
`promlib48/prometheus.library` (plus `.nopci`, `.nopf` and `.trace` variants),
`PrometheusCard-prmfirst/bin/vbcc-classic/Prometheus.card` and
`ne2000/prm-rtl8029.device`.

## prometheus.library 4.8

`driver_2.c` is the library; `driver_2_47_to_48.diff` shows the changes
from 4.7. `prmtrace.c` is a trace build (`-DPRMTRACE`) that logs the
library calls to `SYS:prmtrace.log`.

## Prometheus.card 8.0

Changes since 7.603:
- the DMA list always exists, so `Prm_AllocDMABuffer` returns NULL
  instead of reading address 0 when no `DMASIZE` area is set up;
- `FreeDMAMemory` releases its semaphore on every return, so a bad free
  no longer hangs later DMA calls;
- new `AddDMAMemory(memory, size)` (LVO -54): another card driver can
  hand over part of its video memory as the DMA area. Open the card with
  version 8 to use it. Caller headers (proto/inline, no clib) are in
  `PrometheusCard-prmfirst/include/`.

Built with vbcc (`vc +aos68k -cpu=68020 -cpp-comments -c99 -nostdlib -O2`).
