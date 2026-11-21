/*
 * 3DLabsPermedia2.chip - hardware mouse sprite (TVP4020 64x64 cursor)
 */
#include "p2chip.h"

extern const UBYTE NibbleZoom[16];

#define CURSOR_ON	0x41		/* 2 bit cursor, three colours + transparent */

BOOL SetSprite(__REGA0(struct BoardInfo *bi), __REGD0(BOOL activate), __REGD7(RGBFTYPE fmt))
{
	UBYTE *RegBase = bi->RegisterBase;

	WAITFIFO(2);
	REG(RDPaletteWriteAddress) = RDI_CursorControl;
	Serialize();
	REG(RDIndexedData) = activate ? CURSOR_ON : 0;
	bi->SpriteActive = (LONG)activate;
	return activate;
}

void SetSpritePosition(__REGA0(struct BoardInfo *bi), __REGD0(WORD x), __REGD1(WORD y), __REGD7(RGBFTYPE fmt))
{
	UBYTE *RegBase = bi->RegisterBase;
	struct ModeInfo *mi = bi->ModeInfo;

	x -= bi->XOffset;
	y -= bi->YOffset;
	if (mi) {
		if (mi->Flags & GMF_INTERLACE)
			y >>= 1;
		if (mi->Flags & GMF_DOUBLESCAN)
			y += y;
	}
	/* the cursor position is its bottom right corner */
	x = (x < -63) ? 0 : x + 64;
	y = (y < -63) ? 0 : y + 64;
	WAITFIFO(6);
	REG(RDCursorXLow) = (LONG)x;
	REG(RDCursorYLow) = (LONG)y;
	REG(RDCursorXHigh) = (LONG)x >> 8;
	REG(RDCursorYHigh) = (LONG)y >> 8;
	REG(RDPaletteWriteAddress) = RDI_CursorControl;
	Serialize();
	REG(RDIndexedData) = bi->SpriteActive ? CURSOR_ON : 0;
}

/* BIF_BIGSPRITE: every pixel doubled */
static void RenderSpriteZoomed(struct BoardInfo *bi)
{
	UBYTE *RegBase = bi->RegisterBase;
	volatile ULONG *data = &REG(RDCursorData);
	UWORD *image = bi->MouseImage;
	ULONG rows = bi->MouseHeight;
	UWORD *src;
	UWORD i;
	int plane;

	WAITFIFO(1);
	REG(RDPaletteWriteAddress) = 0;
	Serialize();
	rows >>= 1;
	if (rows > 32)
		rows = 32;
	for (plane = 0; plane < 2; plane++) {
		src = image + 2 + plane;
		for (i = 0; i < rows; i++) {
			UWORD w;
			UBYTE b3, b2, b1, b0;
			int r;

			WAITFIFO(16);
			w = *src;
			src += 2;
			b3 = NibbleZoom[(w >> 12) & 15];
			b2 = NibbleZoom[(w >> 8) & 15];
			b1 = NibbleZoom[(w >> 4) & 15];
			b0 = NibbleZoom[w & 15];
			for (r = 0; r < 2; r++) {
				*data = b3;
				*data = b2;
				*data = b1;
				*data = b0;
				*data = 0;
				*data = 0;
				*data = 0;
				*data = 0;
			}
		}
		for (i = 32 - rows; i > 0; i--) {
			int n;

			WAITFIFO(16);
			for (n = 0; n < 16; n++)
				*data = 0;
		}
	}
}

static void RenderSpriteNormal(struct BoardInfo *bi)
{
	UBYTE *RegBase = bi->RegisterBase;
	volatile ULONG *data = &REG(RDCursorData);
	UWORD *image = bi->MouseImage;
	ULONG rows = bi->MouseHeight;
	UWORD *src;
	UWORD i;
	int plane;

	WAITFIFO(1);
	REG(RDPaletteWriteAddress) = 0;
	Serialize();
	if (rows > 64)
		rows = 64;
	for (plane = 0; plane < 2; plane++) {
		src = image + 2 + plane;
		for (i = 0; i < rows; i++) {
			UWORD w;

			WAITFIFO(8);
			w = *src;
			src += 2;
			*data = (ULONG)w >> 8;
			*data = w;
			*data = 0;
			*data = 0;
			*data = 0;
			*data = 0;
			*data = 0;
			*data = 0;
		}
		for (i = 64 - rows; i > 0; i--) {
			int n;

			WAITFIFO(8);
			for (n = 0; n < 8; n++)
				*data = 0;
		}
	}
}

/* BIF_HIRESSPRITE: 32 pixels wide source */
static void RenderSpriteHires(struct BoardInfo *bi)
{
	UBYTE *RegBase = bi->RegisterBase;
	volatile ULONG *data = &REG(RDCursorData);
	ULONG *image = (ULONG *)bi->MouseImage;
	ULONG rows = bi->MouseHeight;
	ULONG *src;
	UWORD i;
	int plane;

	WAITFIFO(1);
	REG(RDPaletteWriteAddress) = 0;
	Serialize();
	if (rows > 64)
		rows = 64;
	for (plane = 0; plane < 2; plane++) {
		src = image + 2 + plane;
		for (i = 0; i < rows; i++) {
			ULONG l;

			WAITFIFO(8);
			l = *src;
			src += 2;
			*data = l >> 24;
			*data = l >> 16;
			*data = l >> 8;
			*data = l;
			*data = 0;
			*data = 0;
			*data = 0;
			*data = 0;
		}
		for (i = 64 - rows; i > 0; i--) {
			int n;

			WAITFIFO(8);
			for (n = 0; n < 8; n++)
				*data = 0;
		}
	}
}

void SetSpriteImage(__REGA0(struct BoardInfo *bi), __REGD7(RGBFTYPE fmt))
{
	if (bi->Flags & BIF_HIRESSPRITE)
		RenderSpriteHires(bi);
	else if (bi->Flags & BIF_BIGSPRITE)
		RenderSpriteZoomed(bi);
	else
		RenderSpriteNormal(bi);
}

void SetSpriteColor(__REGA0(struct BoardInfo *bi), __REGD0(UBYTE index), __REGD1(UBYTE red), __REGD2(UBYTE green),
		__REGD3(UBYTE blue), __REGD7(RGBFTYPE fmt))
{
	UBYTE *RegBase = bi->RegisterBase;

	WAITFIFO(4);
	REG(RDCursorColorAddress) = (ULONG)index + 1;
	Serialize();
	REG(RDCursorColorData) = red;
	Serialize();
	REG(RDCursorColorData) = green;
	Serialize();
	REG(RDCursorColorData) = blue;
}

void setupSprite(struct BoardInfo *bi)
{
	/* the last 12 KB of card memory hold the sprite save and image buffers */
	bi->MemorySize -= 0x3000;
	bi->SetSprite = SetSprite;
	bi->SetSpritePosition = SetSpritePosition;
	bi->SetSpriteImage = SetSpriteImage;
	bi->SetSpriteColor = SetSpriteColor;
	bi->Flags |= BIF_HARDWARESPRITE | BIF_DBLSCANDBLSPRITEY | BIF_ILACEHALFSPRITEY;
	if (bi->Flags & BIF_BLITTER) {
		bi->Flags |= BIF_HASSPRITEBUFFER;
		bi->MouseSaveBuffer = bi->MemoryBase + bi->MemorySize;
		bi->MouseImageBuffer = bi->MemoryBase + bi->MemorySize + 0x1800;
	}
}
