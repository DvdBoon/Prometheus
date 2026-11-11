/************************************************************************/
#ifndef CGV_BASE_H
#define CGV_BASE_H
/************************************************************************/
/* includes */

#ifndef  EXEC_TYPES_H
#include  <exec/types.h>
#endif

#ifndef  EXEC_LISTS_H
#include  <exec/lists.h>
#endif

#ifndef  EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

#ifndef  EXEC_EXECBASE_H
#include <exec/execbase.h>
#endif

#ifndef  RTG_BASE_H
#include <rtgBase.h>
#endif

/************************************************************************
 * library data structures
 *
 *  Note that the library base begins with a library node
 *
 ************************************************************************/

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack(2)
   #endif
#elif defined(__VBCC__)
   #pragma amiga-align
#endif

struct CgvBase {
	struct Library	LibNode;
	UBYTE  Flags;
	UBYTE  pad;

	/* We are now longword aligned */
	struct RTGBase			*RTGBase;
	struct Library			*LayersBase;
	struct ExecBase		*SysBase;
	APTR	SegList;
	struct DOSBase			*DosBase;
	struct Library			*UtilityBase;
	struct IntuitionBase	*IntuitionBase;
	struct GfxBase			*GfxBase;
	struct SignalSemaphore	Lock;
	struct GraphicsIFace *	IGraphics;
};

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack()
   #endif
#elif defined(__VBCC__)
   #pragma default-align
#endif

/************************************************************************/
#endif
/************************************************************************/
