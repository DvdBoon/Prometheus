/*
 * 3DLabsPermedia2.chip - pixel and memory clock (TVP4020 PLLs)
 */
#include "p2chip.h"

#define NUMCLOCKS	669

extern const UBYTE Numerators[NUMCLOCKS];
extern const UBYTE Denominators[NUMCLOCKS];
extern const ULONG PixelClocks[NUMCLOCKS];

/* index of the table clock nearest to clock, searching the first max entries */
static LONG findClock(ULONG clock, LONG max, ULONG *found)
{
	ULONG c = 0, prev = 0;
	LONG i;

	for (i = 0; i < max; i++) {
		c = PixelClocks[i];
		if (c == clock)
			break;
		if (c > clock) {
			if (i > 0 && c + prev > clock + clock) {
				i--;
				c = prev;
			}
			break;
		}
		prev = c;
	}
	if (i == max)
		i--;
	*found = c;
	return i;
}

ULONG GetPixelClock(__REGA0(struct BoardInfo *bi), __REGA1(struct ModeInfo *mi), __REGD0(ULONG index),
		__REGD7(RGBFTYPE fmt))
{
	return PixelClocks[index];
}

LONG ResolvePixelClock(__REGA0(struct BoardInfo *bi), __REGA1(struct ModeInfo *mi), __REGD0(ULONG clock),
		__REGD7(RGBFTYPE fmt))
{
	LONG max, i;
	ULONG found;

	switch (fmt) {
	case RGBFB_R8G8B8:
	case RGBFB_B8G8R8:
		max = 0x1ff;
		break;
	case RGBFB_A8R8G8B8:
	case RGBFB_A8B8G8R8:
	case RGBFB_R8G8B8A8:
	case RGBFB_B8G8R8A8:
		max = 0x16a;
		break;
	default:
		max = 0x289;
		break;
	}
	i = findClock(clock, max, &found);
	mi->PixelClock = found;
	mi->pll1.Numerator = Numerators[i];
	mi->pll2.Denominator = Denominators[i];
	return i;
}

void setPixelClock(struct BoardInfo *bi, UBYTE num, UBYTE den)
{
	UBYTE *RegBase = bi->RegisterBase;
	LONG timeout = 0x7000;

	WAITFIFO(9);
	REG(RDPaletteWriteAddress) = RDI_PixelClockA3;
	Serialize();
	REG(RDIndexedData) = 0;
	REG(RDPaletteWriteAddress) = RDI_PixelClockA1;
	Serialize();
	REG(RDIndexedData) = num;
	REG(RDPaletteWriteAddress) = RDI_PixelClockA2;
	Serialize();
	REG(RDIndexedData) = (ULONG)den >> 2;
	REG(RDPaletteWriteAddress) = RDI_PixelClockA3;
	Serialize();
	REG(RDIndexedData) = (den & 3) | 8;
	/* wait for the PLL to lock */
	do {
		REG(RDPaletteWriteAddress) = RDI_PixelClockStatus;
		Serialize();
	} while (timeout-- > 0 && !(REG(RDIndexedData) & 0x10));
}

void SetClock(__REGA0(struct BoardInfo *bi))
{
	struct ModeInfo *mi = bi->ModeInfo;

	setPixelClock(bi, mi->pll1.Numerator, mi->pll2.Denominator);
}

void setMemoryClock(struct BoardInfo *bi, ULONG clock)
{
	UBYTE *RegBase = bi->RegisterBase;
	LONG timeout = 0x7000;
	ULONG found;
	LONG i = findClock(clock, NUMCLOCKS, &found);

	WAITFIFO(9);
	REG(RDPaletteWriteAddress) = RDI_MemoryClock3;
	Serialize();
	REG(RDIndexedData) = 0;
	REG(RDPaletteWriteAddress) = RDI_MemoryClock1;
	Serialize();
	REG(RDIndexedData) = Numerators[i];
	REG(RDPaletteWriteAddress) = RDI_MemoryClock2;
	Serialize();
	REG(RDIndexedData) = (ULONG)Denominators[i] >> 2;
	REG(RDPaletteWriteAddress) = RDI_MemoryClock3;
	Serialize();
	REG(RDIndexedData) = (Denominators[i] & 3) | 8;
	do {
		REG(RDPaletteWriteAddress) = RDI_MemoryClockStatus;
		Serialize();
	} while (timeout-- > 0 && !(REG(RDIndexedData) & 0x10));
}

void setupClocks(struct BoardInfo *bi)
{
	bi->SetClock = SetClock;
	bi->ResolvePixelClock = ResolvePixelClock;
	bi->GetPixelClock = GetPixelClock;
	bi->PixelClockCount[PLANAR] = NUMCLOCKS;
	bi->PixelClockCount[CHUNKY] = NUMCLOCKS;
	bi->PixelClockCount[HICOLOR] = NUMCLOCKS;
	bi->PixelClockCount[TRUECOLOR] = NUMCLOCKS;
	bi->PixelClockCount[TRUEALPHA] = NUMCLOCKS;
}
