/************************************************************************/
#ifndef CGX_BASE_H
#define CGX_BASE_H
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

#if 0
struct CgxBase {
	struct Library	LibNode;
	UBYTE  Flags;
	UBYTE  pad;

	/* We are now longword aligned */
	struct Library			*Dummy;
	struct RTGBase			*RTGBase;			// 0x28
	struct Library			*LayersBase;

	UWORD	Filler1[16];
	struct ExecBase		*SysBase;
	APTR	SegList;
	UWORD	Filler2;
	struct DOSBase			*DosBase;
	APTR	AslDummy;
	struct Library			*UtilityBase;
	struct IntuitionBase	*IntuitionBase;
	struct GfxBase			*GfxBase;
	APTR	DatatypeDummy;
	APTR	VideoBase;
	struct SignalSemaphore	Lock;
	struct Library			*EmuBase;
	UBYTE	Data[0x2000];
};
#endif

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
