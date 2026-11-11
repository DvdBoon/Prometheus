/*
 * $Id: emulate.c,v 1.17 2005-10-10 15:53:35 stephanr Exp $
 */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef EXEC_EMULATION_H
#include <exec/emulation.h>
#endif

#ifndef PROTO_EXEC_H
#include <proto/exec.h>
#endif

#ifndef boardinfo_H
#include <boardinfo.h>
#endif

#define SAVEREGS
#define RESTOREREGS

#ifndef INLINE_BOARDINFO_CALLS
#include "emulate.h"
#define INLINE_ME
#else
#define INLINE_ME		STATIC INLINE
#endif

#define ISNATIVE(x)	IsNative(x)

/*****************************************************************************/

INLINE_ME ULONG biGetPixelClock(struct BoardInfo *bi, struct ModeInfo *mi, ULONG cl, RGBFTYPE type)
{
	if (ISNATIVE(bi->GetPixelClock))
	{
		return bi->GetPixelClock(bi,mi,cl,type);
	}
	else
	{
		ULONG result;

		SAVEREGS;

		result = EmulateTags((APTR)bi->GetPixelClock,
			ET_RegisterA0, bi,
			ET_RegisterA1, mi,
			ET_RegisterD0, cl,
			ET_RegisterD7, type,
   ET_SaveRegs, TRUE,
			TAG_DONE);

		RESTOREREGS;

		return result;
	}
}

/*****************************************************************************/

INLINE_ME UWORD biCalculateBytesPerRow(struct BoardInfo *bi, UWORD width, RGBFTYPE rgbtype)
{
	if (ISNATIVE(bi->CalculateBytesPerRow))
	{
		return bi->CalculateBytesPerRow(bi, width, rgbtype);
	}
	else
	{
		UWORD	result;

		SAVEREGS;

		result = EmulateTags((APTR)bi->CalculateBytesPerRow,
			ET_RegisterA0, bi,
			ET_RegisterD0, width,
			ET_RegisterD7, rgbtype,
			ET_SaveRegs, TRUE,
			TAG_DONE);

		RESTOREREGS;

		return result;
	}
}

/*****************************************************************************/

INLINE_ME APTR biCalculateMemory(struct BoardInfo *bi, APTR memory, RGBFTYPE rgbtype)
{
	if (ISNATIVE(bi->CalculateMemory))
	{
		return bi->CalculateMemory(bi, memory, rgbtype);
	}
	else
	{
		APTR	result;

		SAVEREGS;

		result = (APTR) EmulateTags((APTR)bi->CalculateMemory,
			ET_RegisterA0, bi,
			ET_RegisterA1, memory,
			ET_RegisterD7, rgbtype,
			ET_SaveRegs, TRUE,
			TAG_DONE);

		RESTOREREGS;

		return result;
	}
}

/*****************************************************************************/

INLINE_ME ULONG biGetCompatibleFormats(struct BoardInfo *bi, RGBFTYPE type)
{
	if (ISNATIVE(bi->GetCompatibleFormats))
	{
		return bi->GetCompatibleFormats(bi, type);
	}
	else
	{
		ULONG	result;

		SAVEREGS;

		result = EmulateTags((APTR)bi->GetCompatibleFormats,
			ET_RegisterA0, bi,
			ET_RegisterD4, type, /* FIXME: bug in PIV driver */
			ET_RegisterD7, type,
			ET_SaveRegs, TRUE,
			TAG_DONE);

		RESTOREREGS;

		return result;
	}
}

/*****************************************************************************/

INLINE_ME LONG biResolvePixelClock(struct BoardInfo *bi, struct ModeInfo *mi, ULONG clock, RGBFTYPE type)
{
	if (ISNATIVE(bi->ResolvePixelClock))
	{
		return bi->ResolvePixelClock(bi, mi, clock, type);
	}
	else
	{
		LONG	result;

		SAVEREGS;

		result = EmulateTags((APTR)bi->ResolvePixelClock,
			ET_RegisterA0, bi,
			ET_RegisterA1, mi,
			ET_RegisterD0, clock,
			ET_RegisterD7, type,
			ET_SaveRegs, TRUE,
			TAG_DONE);

		RESTOREREGS;

		return result;
	}
}

/*****************************************************************************/

INLINE_ME VOID biSetClearMask(struct BoardInfo *bi, UBYTE mask)
{
	if (ISNATIVE(bi->SetClearMask))
	{
		bi->SetClearMask(bi, mask);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->SetClearMask,
			ET_RegisterA0, bi,
			ET_RegisterD0, mask,
			ET_SaveRegs, TRUE,
			TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME VOID biSetClock(struct BoardInfo *bi)
{
	if (ISNATIVE(bi->SetClock))
	{
		bi->SetClock(bi);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->SetClock,
			ET_RegisterA0, bi,
			ET_SaveRegs, TRUE,
			TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME VOID biSetColorArray(struct BoardInfo *bi, UWORD a, UWORD b)
{
	if (ISNATIVE(bi->SetColorArray))
	{
		bi->SetColorArray(bi,a,b);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->SetColorArray,
			ET_RegisterA0, bi,
			ET_RegisterD0, a,
			ET_RegisterD1, b,
			ET_SaveRegs, TRUE,
			TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME VOID biSetDAC(struct BoardInfo *bi, RGBFTYPE type)
{
	if (ISNATIVE(bi->SetDAC))
	{
		bi->SetDAC(bi,type);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->SetDAC,
			ET_RegisterA0, bi,
			ET_RegisterD7, type,
			ET_SaveRegs, TRUE,
			TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME BOOL biSetDisplay(struct BoardInfo *bi, BOOL boolean)
{
	if (ISNATIVE(bi->SetDisplay))
	{
		return bi->SetDisplay(bi,boolean);
	}
	else
	{
		BOOL result;

		SAVEREGS;

		result = EmulateTags((APTR)bi->SetDisplay,
			ET_RegisterA0, bi,
			ET_RegisterD0, boolean,
			ET_SaveRegs, TRUE,
			TAG_DONE);

		RESTOREREGS;

		return result;
	}
}

/*****************************************************************************/

INLINE_ME VOID biSetGC(struct BoardInfo *bi, struct ModeInfo *mi, BOOL boolean)
{
	if (ISNATIVE(bi->SetGC))
	{
		bi->SetGC(bi,mi,boolean);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->SetGC,
			ET_RegisterA0, bi,
			ET_RegisterA1, mi,
			ET_RegisterD0, boolean,
			ET_SaveRegs, TRUE,
			TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME VOID biSetMemoryMode(struct BoardInfo *bi, RGBFTYPE type)
{
	if (ISNATIVE(bi->SetMemoryMode))
	{
		bi->SetMemoryMode(bi,type);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->SetMemoryMode,
			ET_RegisterA0, bi,
			ET_RegisterD7, type,
			ET_SaveRegs, TRUE,
			TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME VOID biSetPanning(struct BoardInfo *bi, UBYTE *a, UWORD b, WORD c, WORD d, RGBFTYPE type)
{
	if (ISNATIVE(bi->SetPanning))
	{
		bi->SetPanning(bi,a,b,c,d,type);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->SetPanning,
			ET_RegisterA0, bi,
			ET_RegisterA1, a,
			ET_RegisterD0, b,
			ET_RegisterD1, c,
			ET_RegisterD2, d,
			ET_RegisterD7, type,
			ET_SaveRegs, TRUE,
			TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME VOID biSetWriteMask(struct BoardInfo *bi, UBYTE mask)
{
	if (ISNATIVE(bi->SetWriteMask))
	{
		(bi->SetWriteMask)(bi,mask);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->SetWriteMask,
			ET_RegisterA0, bi,
			ET_RegisterD0, mask,
			ET_SaveRegs, TRUE,
			TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME VOID biWaitVerticalSync(struct BoardInfo *bi, BOOL boolean)
{
	if (ISNATIVE(bi->WaitVerticalSync))
	{
		bi->WaitVerticalSync(bi,boolean);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->WaitVerticalSync,
			ET_RegisterA0, bi,
			ET_RegisterD0, boolean,
			ET_SaveRegs, TRUE,
			TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME BOOL biSetInterrupt(
	struct BoardInfo * bi,
	BOOL set)
{
	if (ISNATIVE(bi->SetInterrupt))
	{
		return bi->SetInterrupt(bi, set);
	}
	else
	{
		BOOL retval;

		SAVEREGS;

		retval = (BOOL)EmulateTags((APTR)bi->SetInterrupt,
			ET_RegisterA0,	bi,
			ET_RegisterD0,	set,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;

		return retval;
	}
}

/*****************************************************************************/

INLINE_ME void biWaitBlitter(
	struct BoardInfo * bi)
{
	if (ISNATIVE(bi->WaitBlitter))
	{
		bi->WaitBlitter(bi);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->WaitBlitter,
			ET_RegisterA0,	bi,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME BOOL biSetSwitch(
	struct BoardInfo * bi,
	BOOL on)
{
	if (ISNATIVE(bi->SetSwitch))
	{
		return bi->SetSwitch(bi,on);
	}
	else
	{
		BOOL retval;

		SAVEREGS;

		retval = (BOOL)EmulateTags((APTR)bi->SetSwitch,
			ET_RegisterA0,	bi,
			ET_RegisterD0,	on,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;

		return retval;
	}
}

/*****************************************************************************/

INLINE_ME void biSetDPMSLevel(
	struct BoardInfo * bi,
	ULONG level)
{
	if (ISNATIVE(bi->SetDPMSLevel))
	{
		bi->SetDPMSLevel(bi, level);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->SetDPMSLevel,
			ET_RegisterA0,	bi,
			ET_RegisterD0,	level,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME BOOL biGetVSyncState(
	struct BoardInfo * bi,
	BOOL on)
{
	if (ISNATIVE(bi->GetVSyncState))
	{
		return bi->GetVSyncState(bi, on);
	}
	else
	{
		BOOL retval;

		SAVEREGS;

		retval = (BOOL)EmulateTags((APTR)bi->GetVSyncState,
			ET_RegisterA0,	bi,
			ET_RegisterD0,	on,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;

		return retval;
	}
}

/*****************************************************************************/

INLINE_ME ULONG biGetVBeamPos(
	struct BoardInfo * bi)
{
	if (ISNATIVE(bi->GetVBeamPos))
	{
		return bi->GetVBeamPos(bi);
	}
	else
	{
		ULONG retval;

		SAVEREGS;

		retval = (ULONG)EmulateTags((APTR)bi->GetVBeamPos,
			ET_RegisterA0,	bi,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;

		return retval;
	}
}

/*****************************************************************************/

INLINE_ME void biUpdatePlanar(
	struct BoardInfo * bi,
	struct BitMap * bm,
	struct RenderInfo * ri,
	SHORT x,
	SHORT y,
	SHORT x2,
	SHORT y2,
	UBYTE mask)
{
	if (ISNATIVE(bi->UpdatePlanar))
	{
		bi->UpdatePlanar( bi, bm, ri, x,y, x2,y2, mask );
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->UpdatePlanar,
			ET_RegisterA0,	bi,
			ET_RegisterA1,	bm,
			ET_RegisterA2,	ri,
			ET_RegisterD0,	x,
			ET_RegisterD1,	y,
			ET_RegisterD2,	x2,
			ET_RegisterD3,	y2,
			ET_RegisterD4,	mask,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME void biBlitPlanar2Chunky(
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
	UBYTE mask)
{
	if (ISNATIVE(bi->BlitPlanar2Chunky))
	{
		bi->BlitPlanar2Chunky(bi,bm,ri,x,y,x2,y2,w,h,mterm,mask);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->BlitPlanar2Chunky,
			ET_RegisterA0,	bi,
			ET_RegisterA1,	bm,
			ET_RegisterA2,	ri,
			ET_RegisterD0,	x,
			ET_RegisterD1,	y,
			ET_RegisterD2,	x2,
			ET_RegisterD3,	y2,
			ET_RegisterD4,	w,
			ET_RegisterD5,	h,
			ET_RegisterD6,	mterm,
			ET_RegisterD7,	mask,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME void biBlitPlanar2Direct(
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
	UBYTE mask)
{
	if (ISNATIVE(bi->BlitPlanar2Direct))
	{
		bi->BlitPlanar2Direct(bi, bm,ri, cim, x,y, x2,y2, w,h, mterm, mask);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->BlitPlanar2Direct,
			ET_RegisterA0,	bi,
			ET_RegisterA1,	bm,
			ET_RegisterA2,	ri,
			ET_RegisterA3,	cim,
			ET_RegisterD0,	x,
			ET_RegisterD1,	y,
			ET_RegisterD2,	x2,
			ET_RegisterD3,	y2,
			ET_RegisterD4,	w,
			ET_RegisterD5,	h,
			ET_RegisterD6,	mterm,
			ET_RegisterD7,	mask,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME void biBlitRectNoMaskComplete(
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
	RGBFTYPE rgbformat)
{
	if (ISNATIVE(bi->BlitRectNoMaskComplete))
	{
		bi->BlitRectNoMaskComplete(bi, sri,dri, x,y, x2,y2, w,h, mterm, rgbformat);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->BlitRectNoMaskComplete,
			ET_RegisterA0,	bi,
			ET_RegisterA1,	sri,
			ET_RegisterA2,	dri,
			ET_RegisterD0,	x,
			ET_RegisterD1,	y,
			ET_RegisterD2,	x2,
			ET_RegisterD3,	y2,
			ET_RegisterD4,	w,
			ET_RegisterD5,	h,
			ET_RegisterD6,	mterm,
			ET_RegisterD7,	rgbformat,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME void biFillRect(
	struct BoardInfo * bi,
	struct RenderInfo * ri,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	ULONG color,
	UBYTE mask,
	RGBFTYPE rgbformat)
{
	if (ISNATIVE(bi->FillRect))
	{
		bi->FillRect(bi,ri,x,y,w,h,color,mask,rgbformat);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->FillRect,
			ET_RegisterA0,	bi,
			ET_RegisterA1,	ri,
			ET_RegisterD0,	x,
			ET_RegisterD1,	y,
			ET_RegisterD2,	w,
			ET_RegisterD3,	h,
			ET_RegisterD4,	color,
			ET_RegisterD5,	mask,
			ET_RegisterD7,	rgbformat,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME void biInvertRect(
	struct BoardInfo * bi,
	struct RenderInfo * ri,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	UBYTE mask,
	RGBFTYPE rgbformat)
{
	if (ISNATIVE(bi->InvertRect))
	{
		bi->InvertRect(bi,ri,x,y,w,h,mask,rgbformat);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->InvertRect,
			ET_RegisterA0,	bi,
			ET_RegisterA1,	ri,
			ET_RegisterD0,	x,
			ET_RegisterD1,	y,
			ET_RegisterD2,	w,
			ET_RegisterD3,	h,
			ET_RegisterD4,	mask,
			ET_RegisterD7,	rgbformat,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME void biBlitRect(
	struct BoardInfo * bi,
	struct RenderInfo * ri,
	WORD x,
	WORD y,
	WORD x2,
	WORD y2,
	WORD w,
	WORD h,
	UBYTE mask,
	RGBFTYPE rgbformat)
{
	if (ISNATIVE(bi->BlitRect))
	{
		bi->BlitRect( bi,ri, x,y,x2,y2,w,h,mask,rgbformat );
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->BlitRect,
			ET_RegisterA0,	bi,
			ET_RegisterA1,	ri,
			ET_RegisterD0,	x,
			ET_RegisterD1,	y,
			ET_RegisterD2,	x2,
			ET_RegisterD3,	y2,
			ET_RegisterD4,	w,
			ET_RegisterD5,	h,
			ET_RegisterD6,	mask,
			ET_RegisterD7,	rgbformat,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME void biBlitTemplate(
	struct BoardInfo * bi,
	struct RenderInfo * ri,
	struct Template * tmpl,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	UBYTE mask,
	RGBFTYPE rgbformat)
{
	if (ISNATIVE(bi->BlitTemplate))
	{
		bi->BlitTemplate( bi, ri, tmpl, x,y, w,h, mask, rgbformat );
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->BlitTemplate,
			ET_RegisterA0,	bi,
			ET_RegisterA1,	ri,
			ET_RegisterA2,	tmpl,
			ET_RegisterD0,	x,
			ET_RegisterD1,	y,
			ET_RegisterD2,	w,
			ET_RegisterD3,	h,
			ET_RegisterD4,	mask,
			ET_RegisterD7,	rgbformat,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME void biBlitPattern(
	struct BoardInfo * bi,
	struct RenderInfo * ri,
	struct Pattern * pat,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	UBYTE mask,
	RGBFTYPE rgbformat)
{
	if (ISNATIVE(bi->BlitPattern))
	{
		bi->BlitPattern(bi,ri,pat,x,y,w,h,mask,rgbformat);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->BlitPattern,
			ET_RegisterA0,	bi,
			ET_RegisterA1,	ri,
			ET_RegisterA2,	pat,
			ET_RegisterD0,	x,
			ET_RegisterD1,	y,
			ET_RegisterD2,	w,
			ET_RegisterD3,	h,
			ET_RegisterD4,	mask,
			ET_RegisterD7,	rgbformat,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME void biDrawLine(
	struct BoardInfo * bi,
	struct RenderInfo * ri,
	struct Line * li,
	UBYTE mask,
	RGBFTYPE rgbformat)
{
	if (ISNATIVE(bi->DrawLine))
	{
		bi->DrawLine(bi,ri,li,mask,rgbformat);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->DrawLine,
			ET_RegisterA0,	bi,
			ET_RegisterA1,	ri,
			ET_RegisterA2,	li,
			ET_RegisterD0,	mask,
			ET_RegisterD7,	rgbformat,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME BOOL biSetSprite(
	struct BoardInfo * bi,
	BOOL on,
	RGBFTYPE rgbformat)
{
	if (ISNATIVE(bi->SetSprite))
	{
		return bi->SetSprite(bi,on,rgbformat);
	}
	else
	{
		BOOL retval;

		SAVEREGS;

		retval = (BOOL)EmulateTags((APTR)bi->SetSprite,
			ET_RegisterA0,	bi,
			ET_RegisterD0,	on,
			ET_RegisterD7,	rgbformat,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;

		return retval;
	}
}

/*****************************************************************************/

INLINE_ME void biSetSpritePosition(
	struct BoardInfo * bi,
	WORD x,
	WORD y,
	RGBFTYPE rgbformat)
{
	if (ISNATIVE(bi->SetSpritePosition))
	{
		bi->SetSpritePosition(bi,x,y,rgbformat);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->SetSpritePosition,
			ET_RegisterA0,	bi,
			ET_RegisterD0,	x,
			ET_RegisterD1,	y,
			ET_RegisterD7,	rgbformat,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME void biSetSpriteImage(
	struct BoardInfo * bi,
	RGBFTYPE rgbformat)
{
	if (ISNATIVE(bi->SetSpriteImage))
	{
		bi->SetSpriteImage(bi,rgbformat);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->SetSpriteImage,
			ET_RegisterA0,	bi,
			ET_RegisterD7,	rgbformat,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME void biSetSpriteColor(
	struct BoardInfo * bi,
	UBYTE n,
	UBYTE r,
	UBYTE g,
	UBYTE b,
	RGBFTYPE rgbformat)
{
	if (ISNATIVE(bi->SetSpriteColor))
	{
		bi->SetSpriteColor(bi,n,r,g,b,rgbformat);
	}
	else
	{
		SAVEREGS;

		EmulateTags((APTR)bi->SetSpriteColor,
			ET_RegisterA0,	bi,
			ET_RegisterD0,	n,
			ET_RegisterD1,	r,
			ET_RegisterD2,	g,
			ET_RegisterD3,	b,
			ET_RegisterD7,	rgbformat,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;
	}
}

/*****************************************************************************/

INLINE_ME APTR biAllocCardMem(
	struct BoardInfo * bi,
	ULONG size,
	BOOL force,
	BOOL system)
{
	if (ISNATIVE(bi->AllocCardMem))
	{
		return bi->AllocCardMem(bi,size,force,system);
	}
	else
	{
		APTR retval;

		SAVEREGS;

		retval = (APTR)EmulateTags((APTR)bi->AllocCardMem,
			ET_RegisterA0,	bi,
			ET_RegisterD0,	size,
			ET_RegisterD1,	force,
			ET_RegisterD2,	system,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;

		return retval;
	}
}

/*****************************************************************************/

INLINE_ME BOOL biFreeCardMem(
	struct BoardInfo * bi,
	APTR membase)
{
	if (ISNATIVE(bi->FreeCardMem))
	{
		return bi->FreeCardMem(bi,membase);
	}
	else
	{
		BOOL retval;

		SAVEREGS;

		retval = (BOOL)EmulateTags(bi->FreeCardMem,
			ET_RegisterA0,	bi,
			ET_RegisterA1,	membase,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;

		return retval;
	}
}

/*****************************************************************************/

INLINE_ME struct BitMap * biAllocBitMap(
	struct BoardInfo * bi,
	ULONG w,
	ULONG h,
	struct TagItem * tags)
{
	if (ISNATIVE(bi->AllocBitMap))
	{
		return (bi->AllocBitMap)(bi,w,h,tags);
	}
	else
	{
		struct BitMap * retval;

		SAVEREGS;

		retval = (struct BitMap *)EmulateTags((APTR)bi->AllocBitMap,
			ET_RegisterA0,	bi,
			ET_RegisterD0,	w,
			ET_RegisterD1,	h,
			ET_RegisterA1,	tags,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;

		return retval;
	}
}

/*****************************************************************************/

INLINE_ME BOOL biFreeBitMap(
	struct BoardInfo * bi,
	struct BitMap * bm,
	struct TagItem * tags)
{
	if (ISNATIVE(bi->FreeBitMap))
	{
		return (bi->FreeBitMap)(bi,bm,tags);
	}
	else
	{
		BOOL retval;

		SAVEREGS;

		retval = (BOOL)EmulateTags((APTR)bi->FreeBitMap,
			ET_RegisterA0,	bi,
			ET_RegisterA1,	bm,
			ET_RegisterA2,	tags,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;

		return retval;
	}
}

/*****************************************************************************/

INLINE_ME ULONG biGetBitMapAttr(
	struct BoardInfo * bi,
	struct BitMap * bm,
	ULONG attr)
{
	if (ISNATIVE(bi->GetBitMapAttr))
	{
		return (bi->GetBitMapAttr)(bi,bm,attr);
	}
	else
	{
		ULONG retval;

		SAVEREGS;

		retval = (ULONG)EmulateTags((APTR)bi->GetBitMapAttr,
			ET_RegisterA0,	bi,
			ET_RegisterA1,	bm,
			ET_RegisterD0,	attr,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;

		return retval;
	}
}

/*****************************************************************************/

INLINE_ME APTR biCreateFeature(
	struct BoardInfo * bi,
	ULONG type,
	struct TagItem * tags)
{
	if (ISNATIVE(bi->CreateFeature))
	{
		return bi->CreateFeature(bi, type, tags);
	}
	else
	{
		APTR retval;

		SAVEREGS;

		retval = (APTR)EmulateTags((APTR)bi->CreateFeature,
			ET_RegisterA0,	bi,
			ET_RegisterD0,	type,
			ET_RegisterA1,	tags,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;

		return retval;
	}
}

/*****************************************************************************/

INLINE_ME ULONG biSetFeatureAttrs(
	struct BoardInfo * bi,
	APTR data,
	ULONG type,
	struct TagItem * tags)
{
	if (ISNATIVE(bi->SetFeatureAttrs))
	{
		return bi->SetFeatureAttrs(bi,data,type,tags);
	}
	else
	{
		ULONG retval;

		SAVEREGS;

		retval = (ULONG)EmulateTags((APTR)bi->SetFeatureAttrs,
			ET_RegisterA0,	bi,
			ET_RegisterA1,	data,
			ET_RegisterD0,	type,
			ET_RegisterA2,	tags,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;

		return retval;
	}
}

/*****************************************************************************/

INLINE_ME BOOL biDeleteFeature(
	struct BoardInfo * bi,
	APTR data,
	ULONG type)
{
	if (ISNATIVE(bi->DeleteFeature))
	{
		return bi->DeleteFeature(bi,data,type);
	}
	else
	{
		BOOL retval;

		SAVEREGS;

		retval = (BOOL)EmulateTags((APTR)bi->DeleteFeature,
			ET_RegisterA0,	bi,
			ET_RegisterA1,	data,
			ET_RegisterD0,	type,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;

		return retval;
	}
}

/*****************************************************************************/

INLINE_ME ULONG biGetFeatureAttrs(
	struct BoardInfo * bi,
	APTR data,
	ULONG type,
	struct TagItem * tags)
{
	if (ISNATIVE(bi->GetFeatureAttrs))
	{
		return bi->GetFeatureAttrs(bi,data,type,tags);
	}
	else
	{
		ULONG retval;

		SAVEREGS;

		retval = (ULONG)EmulateTags((APTR)bi->GetFeatureAttrs,
			ET_RegisterA0,	bi,
			ET_RegisterA1,	data,
			ET_RegisterD0,	type,
			ET_RegisterA2,	tags,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;

		return retval;
	}
}

/*****************************************************************************/

INLINE_ME int biWriteYUVRect(
	struct BoardInfo * bi,
	APTR yuvinfo,
	SHORT x,
	SHORT y,
	struct RenderInfo * ri,
	SHORT x2,
	SHORT y2,
	SHORT w,
	SHORT h,
	struct TagItem * tags)
{
	if (ISNATIVE(bi->WriteYUVRect))
	{
		return bi->WriteYUVRect( bi, yuvinfo, x,y, ri, x2,y2,w,h,tags );
	}
	else
	{
		int retval;

		SAVEREGS;

		retval = (int)EmulateTags((APTR)bi->WriteYUVRect,
			ET_RegisterA0,	bi,
			ET_RegisterA1,	yuvinfo,
			ET_RegisterD0,	x,
			ET_RegisterD1,	y,
			ET_RegisterA2,	ri,
			ET_RegisterD2,	x2,
			ET_RegisterD3,	y2,
			ET_RegisterD4,	w,
			ET_RegisterD5,	h,
			ET_RegisterA3,	tags,
			ET_SaveRegs, TRUE,
		TAG_DONE);

		RESTOREREGS;

		return retval;
	}
}

/*****************************************************************************/

INLINE_ME BOOL biGetGammaRamp(struct BoardInfo *bi, UWORD *r, UWORD *g, UWORD *b)
{
	if (ISNATIVE(bi->GetGammaRamp))
	{
		return bi->GetGammaRamp(bi, r,g,b);
	}
	else
	{
		return EmulateTags((APTR)bi->GetGammaRamp,
			ET_RegisterA0, bi,
			ET_RegisterA1, r,
			ET_RegisterA2, g,
			ET_RegisterA3, b,
			TAG_DONE);
	}
}

/*****************************************************************************/

INLINE_ME BOOL biSetGammaRamp(struct BoardInfo *bi, UWORD *r, UWORD *g, UWORD *b)
{
	if (ISNATIVE(bi->SetGammaRamp))
	{
		return bi->SetGammaRamp(bi, r,g,b);
	}
	else
	{
		return EmulateTags((APTR)bi->SetGammaRamp,
			ET_RegisterA0, bi,
			ET_RegisterA1, r,
			ET_RegisterA2, g,
			ET_RegisterA3, b,
			TAG_DONE);
	}
}

/*****************************************************************************/
