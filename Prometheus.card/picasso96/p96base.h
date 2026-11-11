/************************************************************************/
#ifndef P96_BASE_H
#define P96_BASE_H
/************************************************************************/
/* includes
 */
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

/*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack(2)
   #endif
#elif defined(__VBCC__)
   #pragma amiga-align
#endif

/************************************************************************/
/* library data structures
 *
 *  Note that the library base begins with a library node
 */
struct P96Base {
	struct Library	LibNode;
	UBYTE  Flags;
	UBYTE  pad;

	/* We are now longword aligned */
	struct ExecBase		*SysBase;
	struct GfxBase			*GfxBase;
	struct IntuitionBase	*IntuitionBase;
	struct Library			*GadToolsBase;
	struct Library			*UtilityBase;
	struct RTGBase			*RTGBase;

	APTR						SegList;
	APTR						*FuncTable;
};

/************************************************************************/

#define P96NAME   "Picasso96API.library"

/************************************************************************/

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack()
   #endif
#elif defined(__VBCC__)
   #pragma default-align
#endif

#ifdef __cplusplus
}
#endif

/************************************************************************/

#endif

/************************************************************************/
