/************************************************************************/
#ifndef EMU_BASE_H
#define EMU_BASE_H
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

struct EmuBase {
	struct Library			LibNode;
	UBYTE						Flags;
	UBYTE						pad;

	APTR						SegList;
	struct ExecBase		*SysBase;
	ULONG						*FuncTable;
	struct IntuitionBase	*IntuitionBase;
	struct GfxBase			*GfxBase;
	struct Library			*UtilityBase;
	struct Library			*GadToolsBase;
	struct Library			*LayersBase;
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
