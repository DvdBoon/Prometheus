/*
 * 3DLabsPermedia2.chip - 2D acceleration with the Permedia2 graphics processor
 */
#include "p2chip.h"

/* Render command bits */
#define RENDER_AREASTIPPLE	0x00000001
#define RENDER_FASTFILL		0x00000008
#define RENDER_TEXTURE		0x00002000
#define RENDER_SYNCONBITMASK	0x00000800
#define RENDER_RECTANGLE	0x000000c0	/* primitive = rectangle, ... */
#define RENDER_INCX		0x00200000
#define RENDER_INCY		0x00400000

/* FBReadMode bits */
#define FBREAD_SOURCE		0x00000200
#define FBREAD_DEST		0x00000400
#define FBREAD_PACKED		0x00080000

/* LogicalOpMode values: (op << 1) | enable */
#define LOGICOP_COPY		7
#define LOGICOP_INVERT		0x15

/* RGB formats whose pens need the byte order reversed */
#define SWAPPEDFORMATS		0x472b0

static inline UWORD PartialProducts(struct RenderInfo *ri, UWORD pw)
{
	return EnginePartProd[((LONG)ri->BytesPerRow / (LONG)(ULONG)pw + 31) >> 5];
}

static inline ULONG WindowBase(struct BoardInfo *bi, struct RenderInfo *ri, UWORD pw)
{
	return (ULONG)((UBYTE *)ri->Memory - bi->MemoryBase) / pw;
}

/* replicate a pen over 32 bits for 8 and 16 bit pixels */
static inline ULONG ReplicatePen(ULONG pen, UWORD pw)
{
	switch (pw) {
	case 1:
		pen &= 0xff;
		pen |= pen << 8;
		/* fall through */
	case 2:
		pen &= 0xffff;
		pen |= pen << 16;
		break;
	}
	return pen;
}

ULONG SwapBytes(ULONG x)
{
	return (x >> 24) | ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000) | (x << 24);
}

static inline BOOL IsSwapped(RGBFTYPE fmt)
{
	return ((1UL << (fmt & 31)) & SWAPPEDFORMATS) != 0;
}

/*--------------------------------------------------------------------------*/

void WaitBlitter(__REGA0(struct BoardInfo *bi))
{
	UBYTE *RegBase = bi->RegisterBase;

	Serialize();
	while (REG(FIFODiscon) & 0x80000000)
		;
	Serialize();
	while (REG(FIFODiscon) & 0x80000000)
		;
	Serialize();
	while (REG(FIFODiscon) & 0x80000000)
		;
}

static void FillRectEngine(__REGA0(struct BoardInfo *bi), __REGA1(struct RenderInfo *ri), __REGD0(WORD x), __REGD1(WORD y),
		__REGD2(WORD w), __REGD3(WORD h), __REGD4(ULONG pen), __REGD5(UBYTE mask), __REGD7(RGBFTYPE fmt))
{
	UBYTE *RegBase;
	UWORD pw = PixelWidth[fmt];
	UWORD pp = PartialProducts(ri, pw);
	ULONG render;

	if (pp == 0) {
		bi->FillRectDefault(bi, ri, x, y, w, h, pen, mask, fmt);
		return;
	}
	RegBase = bi->RegisterBase;
	render = 0x6000c0;
	WAITFIFO(11);
	REG(FBReadPixel) = ReadPixelValues[fmt];
	REG(FBReadMode) = pp | FBREAD_DEST;
	REG(FBWindowBase) = WindowBase(bi, ri, pw);
	pen = ReplicatePen(pen, pw);
	if (IsSwapped(fmt))
		pen = SwapBytes(pen);
	if (pw == 3) {
		REG(ColorDDAMode) = 1;
		REG(ConstantColor) = pen;
		REG(LogicalOpMode) = 0;
	} else if (pw != 1 || mask == 0xff) {
		REG(ColorDDAMode) = 0;
		REG(FBBlockColor) = pen;
		render |= RENDER_FASTFILL;
		REG(LogicalOpMode) = 0;
	} else {
		REG(ColorDDAMode) = 1;
		REG(ConstantColor) = pen;
		REG(FBSoftwareWriteMask) = Replicate8(mask);
		REG(LogicalOpMode) = LOGICOP_COPY;
	}
	REG(RectangleOrigin) = MakeLong(y, x);
	REG(RectangleSize) = MakeLong(h, w);
	REG(ScissorMode) = 0;
	Serialize();
	REG(Render) = render;
}

static void InvertRectEngine(__REGA0(struct BoardInfo *bi), __REGA1(struct RenderInfo *ri), __REGD0(WORD x), __REGD1(WORD y),
		__REGD2(WORD w), __REGD3(WORD h), __REGD4(UBYTE mask), __REGD7(RGBFTYPE fmt))
{
	UBYTE *RegBase;
	UWORD pw = PixelWidth[fmt];
	UWORD pp = PartialProducts(ri, pw);

	if (pp == 0) {
		bi->InvertRectDefault(bi, ri, x, y, w, h, mask, fmt);
		return;
	}
	RegBase = bi->RegisterBase;
	WAITFIFO(11);
	REG(FBReadPixel) = ReadPixelValues[fmt];
	REG(FBWindowBase) = WindowBase(bi, ri, pw);
	REG(FBReadMode) = pp | FBREAD_DEST;
	REG(RectangleOrigin) = MakeLong(y, x);
	REG(RectangleSize) = MakeLong(h, w);
	if (pw == 1 && mask != 0xff)
		REG(FBSoftwareWriteMask) = Replicate8(mask);
	else if ((1UL << (fmt & 31)) & ((1 << RGBFB_R5G5B5PC) | (1 << RGBFB_R5G5B5)))
		REG(FBSoftwareWriteMask) = 0x7fff7fff;
	else
		REG(FBSoftwareWriteMask) = 0xffffffff;
	REG(ColorDDAMode) = 0;
	REG(FBSourceData) = 0xffffffff;
	REG(LogicalOpMode) = LOGICOP_INVERT;
	REG(ScissorMode) = 0;
	Serialize();
	REG(Render) = 0x6000c0;
}

static void BlitRectEngine(__REGA0(struct BoardInfo *bi), __REGA1(struct RenderInfo *ri), __REGD0(WORD sx), __REGD1(WORD sy),
		__REGD2(WORD dx), __REGD3(WORD dy), __REGD4(WORD w), __REGD5(WORD h), __REGD6(UBYTE mask),
		__REGD7(RGBFTYPE fmt))
{
	UBYTE *RegBase;
	UWORD pw = PixelWidth[fmt];
	UWORD pp = PartialProducts(ri, pw);
	ULONG render;

	if (pp == 0) {
		bi->BlitRectDefault(bi, ri, sx, sy, dx, dy, w, h, mask, fmt);
		return;
	}
	RegBase = bi->RegisterBase;
	WAITFIFO(12);
	REG(FBWindowBase) = WindowBase(bi, ri, pw);
	REG(FBReadPixel) = ReadPixelValues[fmt];
	if (pw > 2) {
		REG(FBReadMode) = pp | FBREAD_SOURCE;
		REG(RectangleOrigin) = MakeLong(dy, dx);
		REG(RectangleSize) = MakeLong(h, w);
		REG(FBSourceDelta) = ((((LONG)sy - dy) & 0xfff) << 16) | (((LONG)sx - dx) & 0xfff);
	} else {
		/* packed copy: 4 (8 bit) or 2 (16 bit) pixels per 32 bit word */
		UWORD shift = (WORD)2 / (WORD)pw;
		UWORD align = (1 << shift) - 1;
		LONG offset = ((LONG)dx & align) - ((LONG)sx & align);

		REG(FBReadMode) = pp | FBREAD_SOURCE | FBREAD_PACKED;
		REG(RectangleOrigin) = ((ULONG)(LONG)dy << 16) | ((LONG)dx >> shift);
		REG(RectangleSize) = ((ULONG)(LONG)h << 16) | (((LONG)w + 7) >> shift);
		REG(PackedDataLimits) = ((ULONG)offset << 29) | ((ULONG)(LONG)dx << 16) | ((LONG)dx + w);
		REG(FBSourceDelta) = ((((LONG)sy - dy) & 0xfff) << 16) |
			((((LONG)sx & ~(ULONG)align) - ((LONG)dx & ~(ULONG)align)) & 0xfff);
	}
	REG(ColorDDAMode) = 0;
	if (pw == 1 && !(bi->Flags & BIF_NOMASKBLITS) && mask != 0xff)
		REG(FBSoftwareWriteMask) = Replicate8(mask);
	else
		REG(FBSoftwareWriteMask) = 0xffffffff;
	REG(LogicalOpMode) = LOGICOP_COPY;
	REG(ScissorMode) = 0;
	Serialize();
	render = (dx < sx) ? RENDER_INCX : 0;
	render |= RENDER_RECTANGLE;
	render |= (dy < sy) ? RENDER_INCY : 0;
	REG(Render) = render;
}

static void BlitRectNoMaskCompleteEngine(__REGA0(struct BoardInfo *bi), __REGA1(struct RenderInfo *src), __REGA2(struct RenderInfo *dst),
		__REGD0(WORD sx), __REGD1(WORD sy), __REGD2(WORD dx), __REGD3(WORD dy), __REGD4(WORD w), __REGD5(WORD h),
		__REGD6(UBYTE minterm), __REGD7(RGBFTYPE fmt))
{
	UBYTE *RegBase;
	UWORD pw = PixelWidth[fmt];
	UWORD pp = PartialProducts(dst, pw);

	if (pp == 0 || src->BytesPerRow != dst->BytesPerRow || src->Memory != dst->Memory) {
		/* different bitmaps: copy line by line, both as one pixel high spans */
		ULONG srcpix, dstpix, mode, render;
		WORD srcmod, dstmod;

		RegBase = bi->RegisterBase;
		srcpix = ((ULONG)((UBYTE *)src->Memory - bi->MemoryBase) + (WORD)sy * src->BytesPerRow +
			(LONG)sx * (LONG)(ULONG)pw) / pw;
		dstpix = ((ULONG)((UBYTE *)dst->Memory - bi->MemoryBase) + (WORD)dy * dst->BytesPerRow +
			(LONG)dx * (LONG)(ULONG)pw) / pw;
		srcmod = (LONG)src->BytesPerRow / (LONG)(ULONG)pw;
		dstmod = (LONG)dst->BytesPerRow / (LONG)(ULONG)pw;
		mode = pp;
		render = RENDER_RECTANGLE;
		if (srcpix >= dstpix) {
			render |= RENDER_INCX;
			render |= RENDER_INCY;
		} else {
			srcpix += ((LONG)h - 1) * srcmod;
			dstpix += ((LONG)h - 1) * dstmod;
			srcmod = -srcmod;
			dstmod = -dstmod;
		}
		if (minterm != 0xf) {
			mode |= FBREAD_SOURCE;
			if (minterm != 0xc)
				mode |= FBREAD_DEST;
		}
		WAITFIFO(11);
		REG(FBReadMode) = mode;		/* 7.17 wrote this before the FIFO wait */
		REG(FBReadPixel) = ReadPixelValues[fmt];
		REG(RectangleOrigin) = MakeLong(0, 0);
		REG(RectangleSize) = MakeLong(1, w);
		REG(ColorDDAMode) = 0;
		REG(FBSoftwareWriteMask) = 0xffffffff;
		REG(LogicalOpMode) = (MinTerms[minterm] << 1) | 1;
		REG(ScissorMode) = 0;
		while (h-- > 0) {
			REG(FBWindowBase) = dstpix;
			REG(FBSourceOffset) = srcpix - dstpix;
			Serialize();
			REG(Render) = render;
			if (h <= 0)
				continue;
			srcpix += srcmod;
			dstpix += dstmod;
			WAITFIFO(3);
			while (REG(FIFODiscon) & 0x80000000)
				Serialize();
		}
	} else {
		ULONG render;

		RegBase = bi->RegisterBase;
		WAITFIFO(12);
		REG(FBWindowBase) = WindowBase(bi, dst, pw);
		REG(FBReadPixel) = ReadPixelValues[fmt];
		if (minterm != 0xc || pw > 2) {
			ULONG mode = pp;

			if (minterm != 0xf) {
				mode |= FBREAD_SOURCE;
				if (minterm != 0xc)
					mode |= FBREAD_DEST;
			}
			REG(FBReadMode) = mode;
			REG(RectangleOrigin) = MakeLong(dy, dx);
			REG(RectangleSize) = MakeLong(h, w);
			REG(FBSourceDelta) = ((((LONG)sy - dy) & 0xfff) << 16) | (((LONG)sx - dx) & 0xfff);
		} else {
			UWORD shift = (WORD)2 / (WORD)pw;
			UWORD align = (1 << shift) - 1;
			LONG offset = ((LONG)dx & align) - ((LONG)sx & align);

			REG(FBReadMode) = pp | FBREAD_SOURCE | FBREAD_PACKED;
			REG(RectangleOrigin) = ((ULONG)(LONG)dy << 16) | ((LONG)dx >> shift);
			REG(RectangleSize) = ((ULONG)(LONG)h << 16) | (((LONG)w + 7) >> shift);
			REG(PackedDataLimits) = ((ULONG)offset << 29) | ((ULONG)(LONG)dx << 16) | ((LONG)dx + w);
			REG(FBSourceDelta) = ((((LONG)sy - dy) & 0xfff) << 16) |
				((((LONG)sx & ~(ULONG)align) - ((LONG)dx & ~(ULONG)align)) & 0xfff);
		}
		REG(ColorDDAMode) = 0;
		REG(FBSoftwareWriteMask) = 0xffffffff;
		REG(LogicalOpMode) = (MinTerms[minterm] << 1) | 1;
		REG(ScissorMode) = 0;
		Serialize();
		render = (dx < sx) ? RENDER_INCX : 0;
		render |= RENDER_RECTANGLE;
		render |= (dy < sy) ? RENDER_INCY : 0;
		REG(Render) = render;
	}
}

static void DrawLineEngine(__REGA0(struct BoardInfo *bi), __REGA1(struct RenderInfo *ri), __REGA2(struct Line *line),
		__REGD0(UBYTE mask), __REGD7(RGBFTYPE fmt))
{
	UBYTE *RegBase;
	UWORD pw = PixelWidth[fmt];
	UWORD pp = PartialProducts(ri, pw);
	UWORD pattern = line->LinePtrn;
	ULONG fg, bg, render;
	LONG count;
	WORD xmin, xmax, ymin, ymax;

	if (line->DrawMode & INVERSVID)
		pattern = ~pattern;
	if ((line->DrawMode & 3) == JAM2)
		pattern = 0xffff;
	if (pp == 0 || pattern != 0xffff) {
		bi->DrawLineDefault(bi, ri, line, mask, fmt);
		return;
	}
	if ((LONG)fmt <= 0 || (LONG)fmt >= RGBFB_MaxFormats)
		return;

	RegBase = bi->RegisterBase;
	fg = line->FgPen;
	bg = line->BgPen;
	render = 0;
	WAITFIFO(28);
	REG(FBReadPixel) = ReadPixelValues[fmt];
	REG(FBWindowBase) = WindowBase(bi, ri, pw);
	if (fmt == RGBFB_CLUT && mask != 0xff) {
		REG(FBSoftwareWriteMask) = Replicate8(mask);
	} else {
		mask = 0xff;
		REG(FBSoftwareWriteMask) = 0xffffffff;
	}
	if (!(line->DrawMode & COMPLEMENT)) {
		REG(FBReadMode) = pp | ((mask != 0xff) ? FBREAD_DEST : 0);
		switch (pw) {
		case 1:
			fg &= 0xff; fg |= fg << 8;
			bg &= 0xff; bg |= bg << 8;
			/* fall through */
		case 2:
			fg &= 0xffff; fg |= fg << 16;
			bg &= 0xffff; bg |= bg << 16;
			break;
		}
		if (IsSwapped(fmt)) {
			fg = SwapBytes(fg);
			bg = SwapBytes(bg);
		}
		REG(ColorDDAMode) = 1;
		REG(ConstantColor) = fg;
		/* never true since 7.17: patterned lines go to DrawLineDefault */
		if ((line->DrawMode & JAM2) && pattern != 0xffff) {
			REG(Texel0) = bg;
			REG(TextureMapFormat) = ((ULONG)ReadPixelValues[fmt] << 19) | pp;
			render |= RENDER_TEXTURE;
		}
		REG(LogicalOpMode) = LOGICOP_COPY;
	} else {
		REG(FBReadMode) = pp | FBREAD_DEST;
		REG(ColorDDAMode) = 0;
		REG(LogicalOpMode) = LOGICOP_INVERT;
		if (line->DrawMode & JAM2)
			pattern = 0xffff;
	}

	/* scissor to the bounding box of the line */
	ymin = ymax = line->Y;
	if (line->dY < 0)
		ymin = (LONG)(WORD)line->Y - (ULONG)line->Length;
	else
		ymax = (LONG)(WORD)line->Y + (ULONG)line->Length;
	if (ymin < 0)
		ymin = 0;
	ymax++;
	xmin = xmax = line->X;
	if (line->dX < 0)
		xmin = (LONG)(WORD)line->X - (ULONG)line->Length;
	else
		xmax = (LONG)(WORD)line->X + (ULONG)line->Length;
	if (xmin < 0)
		xmin = 0;
	xmax++;

	REG(StartXDom) = (ULONG)line->Xorigin << 16;
	REG(StartY) = (ULONG)line->Yorigin << 16;
	if (line->Horizontal) {
		if (line->dX > 0) {
			count = line->dX;
			REG(dXDomReg) = 0x10000;
			REG(dYReg) = ((LONG)line->dY << 16) / (LONG)line->dX;
		} else {
			count = -(LONG)line->dX;
			REG(dXDomReg) = 0xffff0000;
			if (line->dX == 0)
				REG(dYReg) = 0;
			else
				REG(dYReg) = ((LONG)line->dY << 16) / -(LONG)line->dX;
		}
	} else {
		if (line->dY > 0) {
			count = line->dY;
			REG(dYReg) = 0x10000;
			REG(dXDomReg) = ((LONG)line->dX << 16) / (LONG)line->dY;
		} else {
			count = -(LONG)line->dY;
			REG(dYReg) = 0xffff0000;
			if (line->dY == 0)
				REG(dXDomReg) = 0;
			else
				REG(dXDomReg) = ((LONG)line->dX << 16) / -(LONG)line->dY;
		}
	}
	REG(RasterizerMode) = 8;
	REG(ScissorMode) = 1;
	REG(ScissorMinXY) = MakeLong(ymin, xmin);
	REG(ScissorMaxXY) = MakeLong(ymax, xmax);
	REG(GLINTCount) = 0;
	Serialize();
	REG(Render) = render;
	while (REG(FIFODiscon) & 0x80000000)
		Serialize();
	WAITFIFO(1);
	Serialize();
	REG(ContinueNewLine) = count + 1;
}

static void BlitPatternEngine(__REGA0(struct BoardInfo *bi), __REGA1(struct RenderInfo *ri), __REGA2(struct Pattern *pat),
		__REGD0(WORD x), __REGD1(WORD y), __REGD2(WORD w), __REGD3(WORD h), __REGD4(UBYTE mask),
		__REGD7(RGBFTYPE fmt))
{
	UBYTE *RegBase;
	BOOL fallback = FALSE;
	UWORD pw, pp;
	ULONG fg, bg, lo, hi, render, stipple;
	UBYTE *m;
	LONG rows;

	/* the area stipple is 8x8 bits: only patterns with up to 8 rows and */
	/* identical left and right halves can be drawn by the chip */
	if (pat->Size > 3) {
		fallback = TRUE;
	} else {
		UBYTE *p = pat->Memory;
		LONG n = 1L << pat->Size;

		while (n-- > 0) {
			if (p[0] != p[1]) {
				fallback = TRUE;
				break;
			}
			p += 2;
		}
	}
	if (fallback) {
		bi->BlitPatternDefault(bi, ri, pat, x, y, w, h, mask, fmt);
		return;
	}
	pw = PixelWidth[fmt];
	pp = PartialProducts(ri, pw);
	if (pp == 0) {
		bi->BlitPatternDefault(bi, ri, pat, x, y, w, h, mask, fmt);
		return;
	}
	if ((LONG)fmt <= 0 || (LONG)fmt >= RGBFB_MaxFormats)
		return;

	RegBase = bi->RegisterBase;
	fg = pat->FgPen;
	bg = pat->BgPen;
	lo = 0;
	hi = 0;
	render = 0x6000c0 | RENDER_AREASTIPPLE;
	stipple = 0xc0001;
	m = pat->Memory;
	rows = 1L << pat->Size;
	WAITFIFO(22);
	REG(FBReadPixel) = ReadPixelValues[fmt];
	REG(FBWindowBase) = WindowBase(bi, ri, pw);
	stipple |= ((((ULONG)pat->XOffset - x) & 7) << 7) | ((((ULONG)pat->YOffset - y) & 7) << 12);
	if (rows == 8) {
		lo = ((ULONG)m[0] << 24) | ((ULONG)m[2] << 16) | ((ULONG)m[4] << 8) | m[6];
		hi = ((ULONG)m[8] << 24) | ((ULONG)m[10] << 16) | ((ULONG)m[12] << 8) | m[14];
	} else {
		if (rows == 4)
			lo = ((ULONG)m[0] << 24) | ((ULONG)m[2] << 16) | ((ULONG)m[4] << 8) | m[6];
		else if (rows == 2)
			lo = ((ULONG)m[0] << 24) | ((ULONG)m[2] << 16) | ((ULONG)m[0] << 8) | m[2];
		else
			lo = Replicate8(m[0]);
		hi = lo;
	}
	REG(AreaStipplePattern(0)) = hi & 0xff;
	REG(AreaStipplePattern(1)) = (hi >> 8) & 0xff;
	REG(AreaStipplePattern(2)) = (hi >> 16) & 0xff;
	REG(AreaStipplePattern(3)) = (hi >> 24) & 0xff;
	REG(AreaStipplePattern(4)) = lo & 0xff;
	REG(AreaStipplePattern(5)) = (lo >> 8) & 0xff;
	REG(AreaStipplePattern(6)) = (lo >> 16) & 0xff;
	REG(AreaStipplePattern(7)) = (lo >> 24) & 0xff;

	if (fmt == RGBFB_CLUT && mask != 0xff) {
		REG(FBSoftwareWriteMask) = Replicate8(mask);
	} else {
		mask = 0xff;
		REG(FBSoftwareWriteMask) = 0xffffffff;
	}
	if (!(pat->DrawMode & COMPLEMENT)) {
		REG(FBReadMode) = pp | ((mask != 0xff) ? FBREAD_DEST : 0);
		switch (pw) {
		case 1:
			fg &= 0xff; fg |= fg << 8;
			bg &= 0xff; bg |= bg << 8;
			/* fall through */
		case 2:
			fg &= 0xffff; fg |= fg << 16;
			bg &= 0xffff; bg |= bg << 16;
			break;
		}
		if (IsSwapped(fmt)) {
			fg = SwapBytes(fg);
			bg = SwapBytes(bg);
		}
		if (pat->DrawMode & INVERSVID) {
			ULONG t = fg;
			fg = bg;
			bg = t;
		}
		if ((pat->DrawMode & JAM2) || mask != 0xff || pw == 3) {
			REG(ColorDDAMode) = 1;
			REG(ConstantColor) = fg;
			if (pat->DrawMode & JAM2) {
				/* background through a 1x1 texture */
				REG(Texel0) = bg;
				REG(TextureMapFormat) = ((ULONG)ReadPixelValues[fmt] << 19) | pp;
				render |= RENDER_TEXTURE;
				stipple |= 0x100000;
			}
		} else {
			render |= RENDER_FASTFILL;
			REG(ColorDDAMode) = 0;
			REG(FBBlockColor) = fg;
		}
		REG(LogicalOpMode) = LOGICOP_COPY;
	} else {
		REG(FBReadMode) = pp | FBREAD_DEST;
		REG(ColorDDAMode) = 0;
		REG(LogicalOpMode) = LOGICOP_INVERT;
	}
	REG(RectangleOrigin) = MakeLong(y, x);
	REG(RectangleSize) = MakeLong(h, w);
	REG(AreaStippleMode) = stipple;
	REG(ScissorMode) = 0;
	Serialize();
	REG(Render) = render;
}

static void BlitTemplateEngine(__REGA0(struct BoardInfo *bi), __REGA1(struct RenderInfo *ri), __REGA2(struct Template *tmpl),
		__REGD0(WORD x), __REGD1(WORD y), __REGD2(WORD w), __REGD3(WORD h), __REGD4(UBYTE mask),
		__REGD7(RGBFTYPE fmt))
{
	UBYTE *RegBase;
	UWORD pw = PixelWidth[fmt];
	UWORD pp = PartialProducts(ri, pw);
	volatile ULONG *fifo;
	ULONG fg, bg, render, rastmode, *mem;
	WORD xoff;
	UWORD words;
	LONG skip;

	if (pp == 0) {
		bi->BlitTemplateDefault(bi, ri, tmpl, x, y, w, h, mask, fmt);
		return;
	}
	RegBase = bi->RegisterBase;
	fifo = &REG(BitMaskPattern);
	fg = tmpl->FgPen;
	bg = tmpl->BgPen;
	xoff = tmpl->XOffset;
	words = (ULONG)((LONG)xoff + w + 31) >> 5;
	skip = tmpl->BytesPerRow - ((ULONG)words << 2);
	mem = tmpl->Memory;
	render = 0x6008c0;
	rastmode = 0x201;
	WAITFIFO(16);			/* 7.17 waited for 14 but writes up to 16 */
	REG(FBReadPixel) = ReadPixelValues[fmt];
	REG(FBWindowBase) = WindowBase(bi, ri, pw);
	if (fmt == RGBFB_CLUT && mask != 0xff) {
		REG(FBSoftwareWriteMask) = Replicate8(mask);
	} else {
		mask = 0xff;
		REG(FBSoftwareWriteMask) = 0xffffffff;
	}
	if (!(tmpl->DrawMode & COMPLEMENT)) {
		REG(FBReadMode) = pp | ((mask != 0xff) ? FBREAD_DEST : 0);
		switch (pw) {
		case 1:
			fg &= 0xff; fg |= fg << 8;
			bg &= 0xff; bg |= bg << 8;
			/* fall through */
		case 2:
			fg &= 0xffff; fg |= fg << 16;
			bg &= 0xffff; bg |= bg << 16;
			break;
		}
		if (IsSwapped(fmt)) {
			fg = SwapBytes(fg);
			bg = SwapBytes(bg);
		}
		if ((tmpl->DrawMode & JAM2) || mask != 0xff) {
			REG(ColorDDAMode) = 1;
			REG(ConstantColor) = fg;
			if (tmpl->DrawMode & JAM2) {
				REG(Texel0) = bg;
				REG(TextureMapFormat) = ((ULONG)ReadPixelValues[fmt] << 19) | pp;
				render |= RENDER_TEXTURE;
				rastmode |= 0x40;
			}
		} else {
			render |= RENDER_FASTFILL;
			REG(ColorDDAMode) = 0;
			REG(FBBlockColor) = fg;
		}
		REG(LogicalOpMode) = LOGICOP_COPY;
	} else {
		REG(FBReadMode) = pp | FBREAD_DEST;
		REG(ColorDDAMode) = 0;
		REG(LogicalOpMode) = LOGICOP_INVERT;
	}
	REG(RectangleOrigin) = MakeLong(y, (LONG)x - xoff);
	REG(RectangleSize) = MakeLong(h, (LONG)w + xoff);
	if (tmpl->DrawMode & INVERSVID)
		rastmode |= 2;
	REG(RasterizerMode) = rastmode;
	REG(ScissorMode) = 1;
	REG(ScissorMinXY) = MakeLong(y, x);
	REG(ScissorMaxXY) = MakeLong((LONG)y + h, (LONG)x + w);
	Serialize();
	REG(Render) = render;
	while (h-- > 0) {
		WORD n;

		while (REG(InFIFOSpace) < (ULONG)words)
			Serialize();
		for (n = words; n > 0; n--)
			*fifo = *mem++;
		mem = (ULONG *)((UBYTE *)mem + skip);
	}
}

static void BlitPlanar2ChunkyEngine(__REGA0(struct BoardInfo *bi), __REGA1(struct BitMap *bm), __REGA2(struct RenderInfo *ri),
		__REGD0(SHORT sx), __REGD1(SHORT sy), __REGD2(SHORT dx), __REGD3(SHORT dy), __REGD4(SHORT w),
		__REGD5(SHORT h), __REGD6(UBYTE minterm), __REGD7(UBYTE mask))
{
	UBYTE *RegBase;
	UWORD pp = EnginePartProd[((LONG)ri->BytesPerRow + 31) >> 5];
	volatile ULONG *fifo;
	ULONG render, rastmode, bitoffset, dstoff, words, srcoff;

	if (pp == 0) {
		bi->BlitPlanar2ChunkyDefault(bi, bm, ri, sx, sy, dx, dy, w, h, minterm, mask);
		return;
	}
	RegBase = bi->RegisterBase;
	fifo = &REG(BitMaskPattern);
	bitoffset = (LONG)sx & 31;
	render = 0x6028c0;
	rastmode = 0x241;
	WAITFIFO(11);
	REG(FBReadPixel) = 0;
	REG(FBReadMode) = pp | FBREAD_DEST;
	REG(LogicalOpMode) = (MinTerms[minterm & 15] << 1) | 1;
	REG(ColorDDAMode) = 1;
	REG(ConstantColor) = 0xffffffff;
	REG(Texel0) = 0;
	REG(TextureMapFormat) = pp;
	dstoff = (ULONG)((UBYTE *)ri->Memory - bi->MemoryBase) + (WORD)ri->BytesPerRow * dy + (LONG)dx;
	REG(RectangleOrigin) = MakeLong(0, 0);
	REG(RectangleSize) = MakeLong(1, w);
	REG(RasterizerMode) = rastmode | ((bitoffset & 31) << 10);
	REG(ScissorMode) = 0;
	words = (((LONG)sx + w - 1) >> 5) - ((LONG)sx >> 5) + 1;
	srcoff = (ULONG)bm->BytesPerRow * (LONG)sy + (((LONG)sx >> 5) << 2);
	mask &= DepthMask[bm->Depth];

	while (h-- > 0) {
		ULONG planebit = 0x01010101;
		UBYTE m = mask;
		PLANEPTR *planes = bm->Planes;

		do {
			if (m & 1) {
				ULONG *plane = (ULONG *)*planes;
				ULONG n = words;

				while (REG(InFIFOSpace) < n + 3)
					Serialize();
				while (REG(FIFODiscon) & 0x80000000)
					Serialize();
				REG(FBSoftwareWriteMask) = planebit;
				REG(FBWindowBase) = dstoff;
				Serialize();
				REG(Render) = render;
				if (plane == NULL || plane == (ULONG *)-1) {
					/* all zero or all one plane */
					while (n--)
						*fifo = (ULONG)plane;
				} else {
					ULONG *p = (ULONG *)((UBYTE *)plane + srcoff);

					while (n--)
						*fifo = *p++;
				}
			}
			planebit += planebit;
			planes++;
			m = (ULONG)m >> 1;
		} while (m);
		dstoff += (WORD)ri->BytesPerRow;
		srcoff += bm->BytesPerRow;
	}
}

/*
 * 7.18: the 2D functions share the graphics processor with Warp3D.
 * Warp3D holds the public semaphore P2_ENGINE_LOCK while it has the hardware
 * locked. A 2D call that finds it taken draws with the CPU through the
 * Picasso96 default function instead of touching the 3D state, so 2D never
 * waits for 3D and cannot deadlock with it.
 */
/*
 * Warp3D no longer restores the chip registers when it unlocks (it relies on
 * this driver), so the first engine blit after Warp3D used the chip puts back
 * the 2D state that InitChip set up. Registers InitChip never writes get their reset value.
 * The blit functions then write their own registers on top.
 */
static void ReloadEngineState(struct BoardInfo *bi)
{
	UBYTE *RegBase = bi->RegisterBase;

	WAITFIFO(17);
	REG(RasterizerMode) = 0;
	REG(ScissorMode) = 0;
	REG(AreaStippleMode) = 0;
	REG(WindowOrigin) = 0;
	REG(TextureAddressMode) = 0;
	REG(TextureDataFormat) = 0;
	REG(TextureReadMode) = 0;
	REG(TexelLUTMode) = 0;
	REG(TextureColorMode) = 0;
	REG(FogMode) = 0;
	REG(ColorDDAMode) = 0;
	REG(AlphaBlendMode) = 0;
	REG(DitherMode) = 0;
	REG(FBSoftwareWriteMask) = 0xffffffff;
	REG(LogicalOpMode) = 0;
	REG(LBReadMode) = 0;
	REG(LBReadFormat) = 0;
	WAITFIFO(17);
	REG(LBSourceOffset) = 0;
	REG(LBWindowBase) = 0;
	REG(LBWriteMode) = 0;
	REG(LBWriteFormat) = 0;
	REG(Window) = 0x40000;
	REG(StencilMode) = 0;
	REG(DepthMode) = 0;
	REG(FBReadMode) = 0;
	REG(FBSourceOffset) = 0;
	REG(FBPixelOffset) = 0;
	REG(FBWindowBase) = 0;
	REG(FBWriteMode) = 1;
	REG(FBHardwareWriteMask) = 0xffffffff;
	REG(FilterMode) = 0;
	REG(StatisticMode) = 0;
	REG(YUVMode) = 0;
	REG(DeltaMode) = 0;
}

static inline BOOL LockEngine(struct BoardInfo *bi)
{
	struct SignalSemaphore *sem = (struct SignalSemaphore *)bi->EngineLock;

	if (sem == NULL)
		return TRUE;
	/* AttemptSemaphore succeeds for the owner: the task that holds the
	   Warp3D lock must not draw on the engine in the middle of a frame */
	if (sem->ss_Owner == FindTask(NULL))
		return FALSE;
	if (!AttemptSemaphore(sem))
		return FALSE;
	/* reload only after Warp3D used the chip (no flag: always) */
	if (bi->EngineDirty == 0 || *(ULONG *)bi->EngineDirty) {
		ReloadEngineState(bi);
		if (bi->EngineDirty)
			*(ULONG *)bi->EngineDirty = 0;
	}
	return TRUE;
}

static inline void UnlockEngine(struct BoardInfo *bi)
{
	struct SignalSemaphore *sem = (struct SignalSemaphore *)bi->EngineLock;

	if (sem)
		ReleaseSemaphore(sem);
}

void FillRect(__REGA0(struct BoardInfo *bi), __REGA1(struct RenderInfo *ri), __REGD0(WORD x), __REGD1(WORD y), __REGD2(WORD w), __REGD3(WORD h), __REGD4(ULONG pen), __REGD5(UBYTE mask), __REGD7(RGBFTYPE fmt))
{
	if (LockEngine(bi)) {
		FillRectEngine(bi, ri, x, y, w, h, pen, mask, fmt);
		UnlockEngine(bi);
	} else {
		bi->FillRectDefault(bi, ri, x, y, w, h, pen, mask, fmt);
	}
}

void InvertRect(__REGA0(struct BoardInfo *bi), __REGA1(struct RenderInfo *ri), __REGD0(WORD x), __REGD1(WORD y), __REGD2(WORD w), __REGD3(WORD h), __REGD4(UBYTE mask), __REGD7(RGBFTYPE fmt))
{
	if (LockEngine(bi)) {
		InvertRectEngine(bi, ri, x, y, w, h, mask, fmt);
		UnlockEngine(bi);
	} else {
		bi->InvertRectDefault(bi, ri, x, y, w, h, mask, fmt);
	}
}

void BlitRect(__REGA0(struct BoardInfo *bi), __REGA1(struct RenderInfo *ri), __REGD0(WORD sx), __REGD1(WORD sy), __REGD2(WORD dx), __REGD3(WORD dy), __REGD4(WORD w), __REGD5(WORD h), __REGD6(UBYTE mask), __REGD7(RGBFTYPE fmt))
{
	if (LockEngine(bi)) {
		BlitRectEngine(bi, ri, sx, sy, dx, dy, w, h, mask, fmt);
		UnlockEngine(bi);
	} else {
		bi->BlitRectDefault(bi, ri, sx, sy, dx, dy, w, h, mask, fmt);
	}
}

void BlitRectNoMaskComplete(__REGA0(struct BoardInfo *bi), __REGA1(struct RenderInfo *src), __REGA2(struct RenderInfo *dst), __REGD0(WORD sx), __REGD1(WORD sy), __REGD2(WORD dx), __REGD3(WORD dy), __REGD4(WORD w), __REGD5(WORD h), __REGD6(UBYTE minterm), __REGD7(RGBFTYPE fmt))
{
	if (LockEngine(bi)) {
		BlitRectNoMaskCompleteEngine(bi, src, dst, sx, sy, dx, dy, w, h, minterm, fmt);
		UnlockEngine(bi);
	} else {
		bi->BlitRectNoMaskCompleteDefault(bi, src, dst, sx, sy, dx, dy, w, h, minterm, fmt);
	}
}

void DrawLine(__REGA0(struct BoardInfo *bi), __REGA1(struct RenderInfo *ri), __REGA2(struct Line *line), __REGD0(UBYTE mask), __REGD7(RGBFTYPE fmt))
{
	if (LockEngine(bi)) {
		DrawLineEngine(bi, ri, line, mask, fmt);
		UnlockEngine(bi);
	} else {
		bi->DrawLineDefault(bi, ri, line, mask, fmt);
	}
}

void BlitPattern(__REGA0(struct BoardInfo *bi), __REGA1(struct RenderInfo *ri), __REGA2(struct Pattern *pat), __REGD0(WORD x), __REGD1(WORD y), __REGD2(WORD w), __REGD3(WORD h), __REGD4(UBYTE mask), __REGD7(RGBFTYPE fmt))
{
	if (LockEngine(bi)) {
		BlitPatternEngine(bi, ri, pat, x, y, w, h, mask, fmt);
		UnlockEngine(bi);
	} else {
		bi->BlitPatternDefault(bi, ri, pat, x, y, w, h, mask, fmt);
	}
}

void BlitTemplate(__REGA0(struct BoardInfo *bi), __REGA1(struct RenderInfo *ri), __REGA2(struct Template *tmpl), __REGD0(WORD x), __REGD1(WORD y), __REGD2(WORD w), __REGD3(WORD h), __REGD4(UBYTE mask), __REGD7(RGBFTYPE fmt))
{
	if (LockEngine(bi)) {
		BlitTemplateEngine(bi, ri, tmpl, x, y, w, h, mask, fmt);
		UnlockEngine(bi);
	} else {
		bi->BlitTemplateDefault(bi, ri, tmpl, x, y, w, h, mask, fmt);
	}
}

void BlitPlanar2Chunky(__REGA0(struct BoardInfo *bi), __REGA1(struct BitMap *bm), __REGA2(struct RenderInfo *ri), __REGD0(SHORT sx), __REGD1(SHORT sy), __REGD2(SHORT dx), __REGD3(SHORT dy), __REGD4(SHORT w), __REGD5(SHORT h), __REGD6(UBYTE minterm), __REGD7(UBYTE mask))
{
	if (LockEngine(bi)) {
		BlitPlanar2ChunkyEngine(bi, bm, ri, sx, sy, dx, dy, w, h, minterm, mask);
		UnlockEngine(bi);
	} else {
		bi->BlitPlanar2ChunkyDefault(bi, bm, ri, sx, sy, dx, dy, w, h, minterm, mask);
	}
}

void setupBlitter(struct BoardInfo *bi)
{
	bi->WaitBlitter = WaitBlitter;
	bi->BlitRect = BlitRect;
	bi->InvertRect = InvertRect;
	bi->FillRect = FillRect;
	bi->BlitPattern = BlitPattern;
	bi->BlitTemplate = BlitTemplate;
	bi->DrawLine = DrawLine;
	bi->BlitPlanar2Chunky = BlitPlanar2Chunky;
	bi->BlitRectNoMaskComplete = BlitRectNoMaskComplete;
	bi->Flags |= BIF_BLITTER;
}
