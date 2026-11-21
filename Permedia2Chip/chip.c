/*
 * 3DLabsPermedia2.chip - display, DAC, interrupt and init functions
 */
#include "p2chip.h"

extern const ULONG DPMSValues[4];

/*--------------------------------------------------------------------------*/

void SetDPMSLevel(__REGA0(struct BoardInfo *bi), __REGD0(ULONG level))
{
	UBYTE *RegBase = bi->RegisterBase;

	if (level > 3)
		level = 0;
	WAITFIFO(1);
	if (level == 0) {
		REG(VideoControl) = bi->CurrentVideoControl;
	} else {
		REG(VideoControl) = (REG(VideoControl) & 0xffffff87) | DPMSValues[level];
	}
}

BOOL SetInterrupt(__REGA0(struct BoardInfo *bi), __REGD0(BOOL state))
{
	if (bi->Flags & BIF_VBLANKINTERRUPT) {
		UBYTE *RegBase = bi->RegisterBase;

		WAITFIFO(2);
		if (state) {
			REG(IntEnable) |= 0x10;
		} else {
			REG(IntFlags) = 0x10;
			REG(IntEnable) &= ~0x10;
		}
	}
	return state;
}

/* called through bi->HardInterrupt, a1 = bi; the asm entry sets the Z flag */
ULONG HardInterruptCodeC(__REGA1(struct BoardInfo *bi))
{
	UBYTE *RegBase = bi->RegisterBase;
	ULONG ret = 0;

	Serialize();
	if (REG(IntFlags) & 0x10) {
		WAITFIFO(2);
		REG(IntFlags) = 0x10;
		REG(IntEnable) &= ~0x10;
		Cause(&bi->SoftInterrupt);
		ret = 1;
	}
	return ret;
}

void SetColorArray(__REGA0(struct BoardInfo *bi), __REGD0(UWORD start), __REGD1(UWORD count))
{
	if (bi->RGBFormat == RGBFB_CLUT) {
		UBYTE *RegBase = bi->RegisterBase;
		UWORD end = start + count;
		UWORD i;

		Disable();
		for (i = start; i < end; i++) {
			struct CLUTEntry *c = &bi->CLUT[i];

			WAITFIFO(4);
			REG(RDPaletteWriteAddress) = i;
			Serialize();
			REG(RDPaletteData) = c->Red;
			Serialize();
			REG(RDPaletteData) = c->Green;
			Serialize();
			REG(RDPaletteData) = c->Blue;
		}
		Enable();
	}
}

void SetGC(__REGA0(struct BoardInfo *bi), __REGA1(struct ModeInfo *mi), __REGD0(BOOL border))
{
	UBYTE *RegBase = bi->RegisterBase;
	UWORD htotal = mi->HorTotal;
	UWORD hbend = mi->HorTotal - mi->Width;
	UWORD hsstart = mi->HorSyncStart;
	UWORD hsend = mi->HorSyncStart + mi->HorSyncSize;
	UWORD vtotal = mi->VerTotal;
	UWORD vbend = mi->VerTotal - mi->Height;
	UWORD vsstart = mi->VerSyncStart;
	UWORD vsend = mi->VerSyncStart + mi->VerSyncSize;
	ULONG vc = REG(VideoControl);
	BOOL off = (vc & 0x28) == 0;
	LONG shift, mul;

	WAITFIFO(11);
	vc &= 0xfffeff83;

	switch (mi->Depth) {
	case 15:
	case 16:
		shift = 3; mul = 2;
		break;
	case 24:
	case 32:
		shift = 3; mul = 4;
		break;
	default:
		shift = 2; mul = 1;
		break;
	}

	bi->ModeInfo = mi;
	bi->Border = 0;

	if (mi->Flags & GMF_DOUBLESCAN) {
		vtotal += vtotal;
		vbend += vbend;
		vsstart += vsstart;
		vsend += vsend;
		vc |= 0x4;
	}
	if (mi->Flags & GMF_INTERLACE) {
		vtotal = ((ULONG)vtotal + 1) >> 1;
		vbend = ((ULONG)vbend + 1) >> 1;
		vsstart = ((ULONG)vsstart + 1) >> 1;
		vsend = ((ULONG)vsend + 1) >> 1;
	}

	htotal = ((LONG)(ULONG)htotal * mul) >> shift;
	hbend = ((LONG)(ULONG)hbend * mul) >> shift;
	hsstart = ((LONG)(ULONG)hsstart * mul) >> shift;
	hsend = ((LONG)(ULONG)hsend * mul) >> shift;
	htotal--;
	vtotal--;

	Serialize();
	REG(HTotal) = htotal;
	Serialize();
	REG(HgEnd) = hbend;
	Serialize();
	REG(HbEnd) = hbend;
	Serialize();
	REG(HsStart) = hsstart;
	Serialize();
	REG(HsEnd) = hsend;
	Serialize();
	REG(VTotal) = vtotal;
	Serialize();
	REG(VbEnd) = vbend;
	Serialize();
	REG(VsStart) = vsstart;
	Serialize();
	REG(VsEnd) = vsend;
	Serialize();

	vc |= 0x28;
	/* 7.17 tests bi->Flags here, not the ModeInfo sync polarity flags */
	if (bi->Flags & BIF_CACHEMODECHANGE)
		vc |= 0x10;
	if (mi->Depth > 8)
		vc |= 0x10000;
	bi->CurrentVideoControl = vc;
	Serialize();
	if (!off) {
		REG(VideoControl) = vc;
		Serialize();
	}
}

UWORD EngineWidth(UWORD width)
{
	ULONG i;

	for (i = ((ULONG)width + 31) >> 5; i <= 64; i++)
		if (EnginePartProd[i])
			return i << 5;
	return 0;
}

UWORD CalculateBytesPerRow(__REGA0(struct BoardInfo *bi), __REGD0(UWORD width), __REGD7(RGBFTYPE fmt))
{
	if ((LONG)fmt < RGBFB_MaxFormats && width <= 2048) {
		UWORD w = EngineWidth(width);
		if (w)
			return (UWORD)(w * PixelWidth[fmt]);
	}
	Alert(0x1d000002);
	return (UWORD)(width * PixelWidth[fmt]);
}

APTR CalculateMemory(__REGA0(struct BoardInfo *bi), __REGA1(APTR mem), __REGD7(RGBFTYPE fmt))
{
	UBYTE *m = mem;

	if (m < bi->MemoryBase || m >= bi->MemoryBase + bi->MemorySize)
		Alert(0x9d000001);
	if ((1UL << (fmt & 31)) & ((1 << RGBFB_R5G6B5) | (1 << RGBFB_R5G5B5)))
		return bi->MemoryIOBase + (m - bi->MemoryBase);
	return mem;
}

BOOL SetDisplay(__REGA0(struct BoardInfo *bi), __REGD0(BOOL state))
{
	return TRUE;
}

void SetDAC(__REGA0(struct BoardInfo *bi), __REGD7(RGBFTYPE fmt))
{
	UBYTE *RegBase;
	UWORD i;

	if ((LONG)fmt >= RGBFB_MaxFormats)
		return;
	RegBase = bi->RegisterBase;
	WAITFIFO(2);
	Serialize();
	REG(RDPaletteWriteAddress) = RDI_ColorMode;
	Serialize();
	REG(RDIndexedData) = DACValues[fmt];
	Serialize();
	Disable();
	if (fmt == RGBFB_CLUT) {
		for (i = 0; i < 256; i++) {
			struct CLUTEntry *c = &bi->CLUT[i];

			WAITFIFO(4);
			Serialize();
			REG(RDPaletteWriteAddress) = i;
			Serialize();
			REG(RDPaletteData) = c->Red;
			Serialize();
			REG(RDPaletteData) = c->Green;
			Serialize();
			REG(RDPaletteData) = c->Blue;
			Serialize();
		}
	} else {
		/* linear gamma ramp for the hi/true colour modes */
		for (i = 0; i < 256; i++) {
			WAITFIFO(4);
			Serialize();
			REG(RDPaletteWriteAddress) = i;
			Serialize();
			REG(RDPaletteData) = i;
			Serialize();
			REG(RDPaletteData) = i;
			Serialize();
			REG(RDPaletteData) = i;
			Serialize();
		}
	}
	Enable();
}

ULONG GetCompatibleFormats(__REGA0(struct BoardInfo *bi), __REGD7(RGBFTYPE fmt))
{
	return ~RGBFF_NONE;
}

BOOL GetVSyncState(__REGA0(struct BoardInfo *bi), __REGD0(BOOL expected))
{
	UBYTE *RegBase = bi->RegisterBase;
	struct ModeInfo *mi = bi->ModeInfo;

	if (mi)
		return REG(LineCount) >= (ULONG)mi->Height - 1;
	return expected;
}

void WaitVerticalSync(__REGA0(struct BoardInfo *bi), __REGD0(BOOL end))
{
	UBYTE *RegBase = bi->RegisterBase;
	struct ModeInfo *mi = bi->ModeInfo;

	if (mi)
		while (REG(LineCount) < (ULONG)mi->Height - 1)
			Serialize();
}

/* C part of SetMemoryMode; the asm entry keeps d0/d1/a0/a1 */
void SetMemoryModeC(__REGA0(struct BoardInfo *bi), __REGD7(RGBFTYPE fmt))
{
	UBYTE *RegBase = bi->RegisterBase;

	WAITFIFO(1);
	switch (fmt) {
	case RGBFB_A8R8G8B8:
		REG(ApertureOne) = 1;		/* byte swap */
		break;
	case RGBFB_R5G6B5:
	case RGBFB_R5G5B5:
	case RGBFB_YUV422:
	case RGBFB_YUV422PA:
		REG(ApertureOne) = 2;		/* half word swap */
		break;
	default:
		REG(ApertureOne) = 0;
		break;
	}
	Serialize();
}

void SetPanning(__REGA0(struct BoardInfo *bi), __REGA1(UBYTE *mem), __REGD0(UWORD width),
		__REGD1(WORD x), __REGD2(WORD y), __REGD7(RGBFTYPE fmt))
{
	UBYTE *RegBase = bi->RegisterBase;
	struct ModeInfo *mi = bi->ModeInfo;
	UWORD bpr;
	LONG xbytes;
	ULONG stride;

	bi->XOffset = x;
	bi->YOffset = y;
	if ((LONG)fmt >= RGBFB_MaxFormats)
		fmt = RGBFB_CLUT;
	bpr = bi->CalculateBytesPerRow(bi, width, fmt);
	xbytes = (LONG)x * (LONG)(ULONG)PixelWidth[fmt];
	stride = (ULONG)bpr >> 3;
	if (mi && (mi->Flags & GMF_INTERLACE))
		stride += stride;
	WAITFIFO(2);
	Serialize();
	REG(ScreenStride) = stride;
	Serialize();
	REG(ScreenBase) = ((ULONG)(mem - bi->MemoryBase) + (LONG)(ULONG)bpr * (LONG)y + xbytes) >> 3;
	Serialize();
}

/*--------------------------------------------------------------------------*/

static ULONG validateMemoryClock(ULONG clock)
{
	if (clock == 0)
		return 83000000;
	if (clock < 50000000)
		return 50000000;
	if (clock > 100000000)
		return 100000000;
	return clock;
}

static ULONG getAvailableMemory(struct BoardInfo *bi)
{
	volatile ULONG *base = (volatile ULONG *)bi->MemoryBase;
	ULONG size = 0;

	*base = 0xa5a55a5a;
	CacheClearU();
	Serialize();
	if (*base != 0xa5a55a5a)
		return 0;
	Serialize();
	while (size < bi->MemorySize) {
		volatile ULONG *p;

		size += 0x100000;
		p = (volatile ULONG *)(bi->MemoryBase + size);
		*p = 0x5a5aa5a5;
		CacheClearU();
		Serialize();
		if (*p != 0x5a5aa5a5 || *base != 0xa5a55a5a)
			break;
	}
	return size;
}

extern void SetMemoryMode(void);
extern void NoOp(void);
extern void HardInterruptCode(void);

/* returns a full 32 bit 0 or 1 (callers may test d0 as a long) */
ULONG InitChip(__REGA0(struct BoardInfo *bi))
{
	UBYTE *RegBase = bi->RegisterBase;

	bi->Flags |= BIF_NOMEMORYMODEMIX;
	bi->RGBFormats = RGBFF_CLUT | RGBFF_B8G8R8A8 | RGBFF_R5G6B5 | RGBFF_R5G5B5;
	bi->PaletteChipType = PCT_TIPermedia2;
	bi->GraphicsControllerType = GCT_TIPermedia2;

	bi->SetGC = SetGC;
	bi->SetPanning = SetPanning;
	bi->CalculateBytesPerRow = CalculateBytesPerRow;
	bi->CalculateMemory = CalculateMemory;
	bi->GetCompatibleFormats = GetCompatibleFormats;
	bi->SetDAC = SetDAC;
	bi->SetColorArray = SetColorArray;
	bi->SetDPMSLevel = SetDPMSLevel;
	bi->SetDisplay = SetDisplay;
	bi->SetMemoryMode = (APTR)SetMemoryMode;
	bi->SetWriteMask = (APTR)NoOp;
	bi->SetReadPlane = (APTR)NoOp;
	bi->SetClearMask = (APTR)NoOp;
	bi->WaitVerticalSync = WaitVerticalSync;
	bi->GetVSyncState = GetVSyncState;
	bi->HardInterrupt.is_Code = (APTR)HardInterruptCode;
	bi->SetInterrupt = SetInterrupt;

	setupClocks(bi);
	setupBlitter(bi);

	bi->BitsPerCannon = 8;
	bi->MaxHorValue[PLANAR] = bi->MaxHorValue[CHUNKY] = bi->MaxHorValue[HICOLOR] =
		bi->MaxHorValue[TRUECOLOR] = bi->MaxHorValue[TRUEALPHA] = 0xff8;
	bi->MaxVerValue[PLANAR] = bi->MaxVerValue[CHUNKY] = bi->MaxVerValue[HICOLOR] =
		bi->MaxVerValue[TRUECOLOR] = bi->MaxVerValue[TRUEALPHA] = 0x7ff;
	bi->MaxHorResolution[PLANAR] = bi->MaxHorResolution[CHUNKY] = bi->MaxHorResolution[HICOLOR] =
		bi->MaxHorResolution[TRUECOLOR] = bi->MaxHorResolution[TRUEALPHA] = 2048;
	bi->MaxVerResolution[PLANAR] = bi->MaxVerResolution[CHUNKY] = bi->MaxVerResolution[HICOLOR] =
		bi->MaxVerResolution[TRUECOLOR] = bi->MaxVerResolution[TRUEALPHA] = 1536;

	/* software reset */
	REG(ResetStatus) = 0;
	Serialize();
	while (REG(ResetStatus) & 0x80000000)
		Serialize();

	REG(BypassWriteMask) = 0xffffffff;
	Serialize();
	REG(FramebufferWriteMask) = 0xffffffff;
	Serialize();
	REG(ChipConfig) &= 0xfff9;
	Serialize();
	REG(ChipConfig) |= 0x20;
	Serialize();

	REG(RDPaletteWriteAddress) = RDI_MiscControl;
	Serialize();
	REG(RDIndexedData) = 2;
	Serialize();
	REG(RDPaletteWriteAddress) = RDI_ModeControl;
	Serialize();
	REG(RDIndexedData) = 0;
	Serialize();
	REG(RDPaletteWriteAddress) = RDI_CursorControl;
	Serialize();
	REG(RDIndexedData) = 0;
	Serialize();
	REG(RDPixelMask) = 0xff;
	Serialize();
	REG(RDPaletteWriteAddress) = 0;
	Serialize();

	REG(FBWriteMode) = 1;
	Serialize();
	REG(FilterMode) = 0;
	REG(FBBlockColor) = 0;
	Serialize();
	REG(MemControl) = 0;
	Serialize();
	REG(BootAddress) = 0x30;
	Serialize();
	REG(MemConfig) = 0xed415c32;
	Serialize();
	REG(Reboot) = 0;
	Serialize();

	WAITFIFO(35);
	REG(FBReadMode) = 0;
	Serialize();
	REG(LBReadMode) = 0;
	Serialize();
	REG(DitherMode) = 0;
	Serialize();
	REG(AreaStippleMode) = 0;
	Serialize();
	REG(DepthMode) = 0;
	Serialize();
	REG(StencilMode) = 0;
	Serialize();
	REG(TextureAddressMode) = 0;
	Serialize();
	REG(TextureReadMode) = 0;
	Serialize();
	REG(TextureColorMode) = 0;
	Serialize();
	REG(TextureDataFormat) = 0;
	Serialize();
	REG(YUVMode) = 0;
	Serialize();
	REG(ColorDDAMode) = 0;
	Serialize();
	REG(TexelLUTMode) = 0;
	Serialize();
	REG(AlphaBlendMode) = 0;
	Serialize();
	REG(LogicalOpMode) = 0;
	Serialize();
	REG(StatisticMode) = 0;
	Serialize();
	REG(Window) = 0x40000;
	Serialize();
	REG(ScissorMode) = 0;
	Serialize();
	REG(RasterizerMode) = 0;
	Serialize();
	REG(FBReadPixel) = 0;
	Serialize();

	REG(ApertureOne) = 0;
	Serialize();
	REG(ApertureTwo) = 2;
	Serialize();

	/* 640x480 default timing */
	REG(ScreenBase) = 0;
	Serialize();
	REG(ScreenStride) = 0x50;
	Serialize();
	REG(HTotal) = 0xcf;
	Serialize();
	REG(HsStart) = 0xc;
	Serialize();
	REG(HsEnd) = 0x20;
	Serialize();
	REG(HbEnd) = 0x30;
	Serialize();
	REG(HgEnd) = 0x30;
	Serialize();
	REG(VTotal) = 0x207;
	Serialize();
	REG(VsStart) = 9;
	Serialize();
	REG(VsEnd) = 0xc;
	Serialize();
	REG(VbEnd) = 0x28;
	Serialize();
	REG(VideoControl) = bi->CurrentVideoControl = 0x229;
	Serialize();

	bi->MemoryClock = validateMemoryClock(bi->MemoryClock);
	setMemoryClock(bi, bi->MemoryClock);
	setPixelClock(bi, 0x68, 0x1b);

	WAITFIFO(11);
	REG(WindowOrigin) = 0;
	Serialize();
	REG(LBReadFormat) = 0;
	Serialize();
	REG(LBWriteFormat) = 0;
	Serialize();
	REG(LBSourceOffset) = 0;
	Serialize();
	REG(FBSourceOffset) = 0;
	Serialize();
	REG(FBPixelOffset) = 0;
	Serialize();
	REG(FBWindowBase) = 0;
	Serialize();
	REG(LBWindowBase) = 0;
	Serialize();
	REG(FBSoftwareWriteMask) = 0xffffffff;
	Serialize();
	REG(FBHardwareWriteMask) = 0xffffffff;

	WAITFIFO(12);
	REG(RDPaletteWriteAddress) = RDI_ColorMode;
	Serialize();
	REG(RDIndexedData) = DACValues[RGBFB_NONE];
	REG(RDPaletteWriteAddress) = RDI_ColorKeyControl;
	Serialize();
	REG(RDIndexedData) = 0;
	Serialize();
	REG(RDPaletteWriteAddress) = 0x41;
	Serialize();
	REG(RDIndexedData) = 0;
	REG(RDPaletteWriteAddress) = 0x42;
	Serialize();
	REG(RDIndexedData) = 0;
	REG(RDPaletteWriteAddress) = 0x43;
	Serialize();
	REG(RDIndexedData) = 0;
	REG(RDPaletteWriteAddress) = 0x44;
	Serialize();
	REG(RDIndexedData) = 0;
	Serialize();

	if ((bi->MemorySize = getAvailableMemory(bi)) != 0) {
		setupSprite(bi);
		return TRUE;
	}
	return FALSE;
}
