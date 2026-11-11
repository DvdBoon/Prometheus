/************************************************************************/
#ifndef VIL_BASE_H
#define VIL_BASE_H
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

struct VilBase {
	struct Library	LibNode;
	UBYTE  Flags;
	UBYTE  pad;

	/* We are now longword aligned */
	struct ExecBase		*SysBase;
	struct IntuitionBase	*IntuitionBase;
	struct GfxBase			*GfxBase;
	struct Library			*UtilityBase;
	struct RTGBase			*RTGBase;
	APTR	SetRGB4;
	APTR	SegList;
	struct Library			*EmuBase;
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
