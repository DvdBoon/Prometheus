/*
 * $Id: emulate.h,v 1.8 2005-03-18 17:25:06 stephanr Exp $
 */

#ifndef EMULATE_H
#define EMULATE_H

#ifdef INLINE_BOARDINFO_CALLS
#include "emulate.c"
#else

#include <boardinfo.h>

BOOL biSetGammaRamp(struct BoardInfo *bi, UWORD *r, UWORD *g, UWORD *b)
BOOL biGetGammaRamp(struct BoardInfo *bi, UWORD *r, UWORD *g, UWORD *b)

VOID biSetClearMask(struct BoardInfo *bi, UBYTE mask);
BOOL biSetDisplay(struct BoardInfo *bi, BOOL boolean);
VOID biSetPanning(struct BoardInfo *bi, UBYTE *a, UWORD b, WORD c, WORD d, RGBFTYPE type);
VOID biWaitVerticalSync(struct BoardInfo *bi, BOOL boolean);
VOID biSetGC(struct BoardInfo *bi, struct ModeInfo *mi, BOOL boolean);
VOID biSetClock(struct BoardInfo *bi);
VOID biSetDAC(struct BoardInfo *bi, RGBFTYPE type);
ULONG biGetCompatibleFormats(struct BoardInfo *bi, RGBFTYPE type);
APTR biCalculateMemory(struct BoardInfo *bi, APTR memory, RGBFTYPE rgbtype);
VOID biSetWriteMask(struct BoardInfo *bi, UBYTE mask);

VOID biSetColorArray(
	struct BoardInfo *bi,
	UWORD a,
	UWORD b);

LONG biResolvePixelClock(
	struct BoardInfo *bi,
	struct ModeInfo *mi,
	ULONG clock,
	RGBFTYPE type);

VOID biSetMemoryMode(
	struct BoardInfo *bi,
	RGBFTYPE type);

UWORD biCalculateBytesPerRow(
	struct BoardInfo *bi,
	UWORD width,
	RGBFTYPE rgbtype);

BOOL biSetInterrupt(
	struct BoardInfo * bi,
	BOOL set);

void biWaitBlitter(
	struct BoardInfo * bi);

BOOL biSetSwitch(
	struct BoardInfo * bi,
	BOOL on);

void biSetDPMSLevel(
	struct BoardInfo * bi,
	ULONG level);

BOOL biGetVSyncState(
	struct BoardInfo * bi,
	BOOL on);

ULONG biGetVBeamPos(
	struct BoardInfo * bi);

void biUpdatePlanar(
	struct BoardInfo * bi,
	struct BitMap * bm,
	struct RenderInfo * ri,
	SHORT x,
	SHORT y,
	SHORT x2,
	SHORT y2,
	UBYTE mask);

void biBlitPlanar2Chunky(
	struct BoardInfo * bi,
	struct BitMap * bm,
	struct RenderInfo * ri,
	SHORT x,
	SHORT y,
	SHORT x2,
	SHORT y2,
	SHORT w,
	SHORT h,
	UBYTE mterm,
	UBYTE mask);

void biBlitPlanar2Direct(
	struct BoardInfo * bi,
	struct BitMap * bm,
	struct RenderInfo * ri,
	struct ColorIndexMapping * cim,
	SHORT x,
	SHORT y,
	SHORT x2,
	SHORT y2,
	SHORT w,
	SHORT h,
	UBYTE mterm,
	UBYTE mask);

void biBlitRectNoMaskComplete(
	struct BoardInfo * bi,
	struct RenderInfo * sri,
	struct RenderInfo * dri,
	WORD x,
	WORD y,
	WORD x2,
	WORD y2,
	WORD w,
	WORD h,
	UBYTE mterm,
	RGBFTYPE rgbformat);

void biFillRect(
	struct BoardInfo * bi,
	struct RenderInfo * ri,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	ULONG color,
	UBYTE mask,
	RGBFTYPE rgbformat);

void biInvertRect(
	struct BoardInfo * bi,
	struct RenderInfo * ri,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	UBYTE mask,
	RGBFTYPE rgbformat);

void biBlitRect(
	struct BoardInfo * bi,
	struct RenderInfo * ri,
	WORD x,
	WORD y,
	WORD x2,
	WORD y2,
	WORD w,
	WORD h,
	UBYTE mask,
	RGBFTYPE rgbformat);

void biBlitTemplate(
	struct BoardInfo * bi,
	struct RenderInfo * ri,
	struct Template * tmpl,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	UBYTE mask,
	RGBFTYPE rgbformat);

void biBlitPattern(
	struct BoardInfo * bi,
	struct RenderInfo * ri,
	struct Pattern * pat,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	UBYTE mask,
	RGBFTYPE rgbformat);

void biDrawLine(
	struct BoardInfo * bi,
	struct RenderInfo * ri,
	struct Line * li,
	UBYTE mask,
	RGBFTYPE rgbformat);

BOOL biSetSprite(
	struct BoardInfo * bi,
	BOOL on,
	RGBFTYPE rgbformat);

void biSetSpritePosition(
	struct BoardInfo * bi,
	WORD x,
	WORD y,
	RGBFTYPE rgbformat);

void biSetSpriteImage(
	struct BoardInfo * bi,
	RGBFTYPE rgbformat);

void biSetSpriteColor(
	struct BoardInfo * bi,
	UBYTE n,
	UBYTE r,
	UBYTE g,
	UBYTE b,
	RGBFTYPE rgbformat);

APTR biAllocCardMem(
	struct BoardInfo * bi,
	ULONG size,
	BOOL force,
	BOOL system);

BOOL biFreeCardMem(
	struct BoardInfo * bi,
	APTR membase);

struct BitMap * biAllocBitMap(
	struct BoardInfo * bi,
	ULONG w,
	ULONG h,
	struct TagItem * tags);

BOOL biFreeBitMap(
	struct BoardInfo * bi,
	struct BitMap * bm,
	struct TagItem * tags);

ULONG biGetBitMapAttr(
	struct BoardInfo * bi,
	struct BitMap * bm,
	ULONG attr);

APTR biCreateFeature(
	struct BoardInfo * bi,
	ULONG type,
	struct TagItem * tags);

ULONG biSetFeatureAttrs(
	struct BoardInfo * bi,
	APTR data,
	ULONG type,
	struct TagItem * tags);

BOOL biDeleteFeature(
	struct BoardInfo * bi,
	APTR data,
	ULONG type);

ULONG biGetFeatureAttrs(
	struct BoardInfo * bi,
	APTR data,
	ULONG type,
	struct TagItem * tags);

int biWriteYUVRect(
	struct BoardInfo * bi,
	APTR yuvinfo,
	SHORT x,
	SHORT y,
	struct RenderInfo * ri,
	SHORT x2,
	SHORT y2,
	SHORT w,
	SHORT h,
	struct TagItem * tags);
#endif

#endif
