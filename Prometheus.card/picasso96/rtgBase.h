#ifndef RTG_BASE_H
#define RTG_BASE_H

#include <exec/types.h>
#include <exec/libraries.h>
#include <graphics/gfxbase.h>
#include <intuition/intuitionbase.h>
#include <exec/execbase.h>

#include <boardinfo.h>
#include <p96BitMap.h>
#include <p96Semaphore.h>
#include <settings.h>

//#include <internal/graphics/gfxbase_private.h>

/************************************************************************/

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

#define NumHashEntries 128

/************************************************************************/
/* hash array
 */
#define	HASHCOUNT	8

struct PIPHashNode {
	APTR	Match;
	struct PIPHashNode *HashChain;
	APTR	Extra;
};

struct PIPHashTable {
	APTR	Hash[HASHCOUNT];
};

struct ExtraData {
	struct SpecialFeature	*Special;
   WORD							 Left,Top,Width,Height;
	ULONG							 Flags;
};

enum {PIPB_RELBOTTOM, PIPB_RELRIGHT, PIPB_RELWIDTH, PIPB_RELHEIGHT, PIPB_OCCLUSION};
#define	PIPF_RELBOTTOM		(1<<PIPB_RELBOTTOM)
#define	PIPF_RELRIGHT		(1<<PIPB_RELRIGHT)
#define	PIPF_RELWIDTH		(1<<PIPB_RELWIDTH)
#define	PIPF_RELHEIGHT		(1<<PIPB_RELHEIGHT)
#define	PIPF_OCCLUSION		(1<<PIPB_OCCLUSION)

typedef void (*RTGCOPYFUNC)(CONST_APTR src, APTR dest, uint32 numBytes);
typedef void (*RTGCLEARFUNC)(APTR dest, uint32 numBytes);
typedef void (*RTGSETFUNC)(APTR dest, uint32 value, uint32 numBytes);


/************************************************************************/

/* library data structures-- Note that the library base begins with a library node */

// WARNING  WARNING  WARNING  WARNING
//
// Only add new fields to the bottom of this structure and do not
// remove obsolete fields. There are components which peek directly
// into this structure.
//
// WARNING  WARNING  WARNING  WARNING

struct RTGBase
{
	struct Library				LibNode;
	UBYTE						Flags;
	UBYTE						obsoleteBoardCount;  // Still populated; used by Picasso96Mode and P96/CGX libs

	/* We are now longword aligned */

	struct ExecBase 			*SysBase;
	struct DosLibrary			*obsoleteDosBase;
	struct GfxBase				*GfxBase;
	struct IntuitionBase		*IntuitionBase;
	struct Library				*obsoleteIFFParseBase;
	struct Library				*UtilityBase;

	ULONG  						SegList;

	struct BoardInfo			*obsoleteBoards[GFX_MAX_NUM_BOARDS];  // Still populated; used by Picasso96Mode and P96/CGX libs

	struct p96Semaphore			ShuffleLock;

	APTR						obsoleteVecTable[GFX_MAX_NUM_BOARDS];
	APTR						obsoleteVecTable_TrueColor[GFX_MAX_NUM_BOARDS];

	APTR						obsoletePoolMem;
	struct BitMapExtra			*BitMapExtraHashArray[NumHashEntries];
	struct Library				*LayersBase;

	struct LibResolution		*DisplayIDHashArray[16];

	struct Process				*obsoleteProcess;

	ULONG						obsoleteGlobalFlags;

	struct Library				*obsoleteIconBase;

	UWORD						*TextBuffer;
	ULONG						TextBufferSize;
	APTR						TextBufferMutex;

	struct Library				*TimerBase;

	struct SignalSemaphore		EditScreenSemaphore;  // Used by Picasso96Mode

	APTR						obsoleteDMAMutex;

	ULONG						obsoleteDMAMinSize;
	ULONG						obsoleteDMABLToVRAM;
	ULONG						obsoleteDMABLFromVRAM;

	struct PIPHashTable			Extras;

	ULONG						FormatsShown;

	struct p96Semaphore			obsoletePoolMemLock;

	struct Resource				*obsoleteDracoMouseBase;

	APTR						obsoleteBitMapPool; /* Item pool! */

	APTR						TempBufferMutex;
	ULONG						TempBufferSize;
	APTR						TempBuffer;

	ULONG						obsolete;

	LONG						obsoleteClassicMachine;

	ULONG						c2pTable[512];
	ULONG						p2cTable[512];

	struct Resident				*obsoleteBootImageModule;
	APTR						obsoleteBootImageData;

	struct SignalSemaphore		ExtrasLock;
	struct SignalSemaphore		LibResLock;

	RTGCLEARFUNC				obsoleteClearVRAM;
	RTGSETFUNC					obsoleteSetVRAM;
	RTGCOPYFUNC					obsoleteCopyToVRAM;
	RTGCOPYFUNC					obsoleteCopyFromVRAM;
	RTGCOPYFUNC					obsoleteCopyVRAM;
	RTGCOPYFUNC					obsoleteCopyMem;
	RTGCOPYFUNC					obsoleteMoveVRAM;
	RTGCOPYFUNC					obsoleteMoveMem;
	RTGCLEARFUNC				obsoleteDMAClearVRAM;
	RTGSETFUNC					obsoleteDMASetVRAM;
	RTGCOPYFUNC					obsoleteDMACopyToVRAM;
	RTGCOPYFUNC					obsoleteDMACopyFromVRAM;

	struct SignalSemaphore		obsoleteBootScreenSemaphore;

	struct ExecIFace *			IExec;
	struct GraphicsIFace *		IGraphics;
};

#define RTGNAME "rtg.library"

/************************************************************************/

struct ColorMapping
{
	uint32 ReferenceCount;
	struct ColorIndexMapping Mapping;
};

#if 0

struct BitMapData
{
	uint32                    Alignment;
	uint32                    Flags;
	APTR                      UserData;
	APTR                      RenderFunc;
	APTR                      SaveFunc;
	struct SignalSemaphore    NestCountLock;
	uint16                    NestCount;
	struct p96SemaphoreHandle Lock;
	struct BitMap *           BackLink;      // points back to bitmap this struct was allocated for
	struct BitMap *           MasterFriend;  // points to top level friend, probably the screen (as this one is allocated without friend)
	struct Hook *             NotifyHook;    // for bitmap change notification
	struct BitMapExtra *      Extra;
	BOOL                      LTBNestCount;  // rtgLockBitMapToBoard() nest count
	uint32                    ActualWidth;   // unaltered width passed to AllocBitMap()
};

#define	BMDB_NOSPRITE     0
#define	BMDB_ALIGNED      1
#define	BMDB_DIRECT       2
#define	BMDB_DISPLAYABLE  3
#define	BMDB_VISIBLE      4
#define	BMDB_ISVILLAGE    5
#define	BMDB_TRANSIENT    6
#define	BMDB_CONSTBSWAP   7
#define	BMDB_USERMEM      8
#define	BMDB_NOTIFYHOOK   9
#define BMDB_LOCKED       10
#define BMDB_ALPHAMAP     11
#define BMDB_KEEPALLOCED  12
#define BMDB_SPECIAL      13

#define	BMDF_NOSPRITE     (1L<<BMDB_NOSPRITE)
#define	BMDF_ALIGNED      (1L<<BMDB_ALIGNED)
#define	BMDF_DIRECT       (1L<<BMDB_DIRECT)
#define	BMDF_DISPLAYABLE  (1L<<BMDB_DISPLAYABLE)
#define	BMDF_VISIBLE      (1L<<BMDB_VISIBLE)
#define	BMDF_ISVILLAGE    (1L<<BMDB_ISVILLAGE)
#define	BMDF_TRANSIENT    (1L<<BMDB_TRANSIENT)
#define	BMDF_CONSTBSWAP   (1L<<BMDB_CONSTBSWAP)
#define	BMDF_USERMEM      (1L<<BMDB_USERMEM)
#define	BMDF_NOTIFYHOOK   (1L<<BMDB_NOTIFYHOOK)
#define BMDF_LOCKED       (1L<<BMDB_LOCKED)
#define BMDF_ALPHAMAP     (1L<<BMDB_ALPHAMAP)
#define BMDF_KEEPALLOCED  (1L<<BMDB_KEEPALLOCED)
#define BMDF_SPECIAL      (1L<<BMDB_SPECIAL)

/* synonyms */
#define	BMDB_USERPRIVATE BMDB_TRANSIENT
#define	BMDF_USERPRIVATE BMDF_TRANSIENT

#endif

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

#endif  /* RTG_BASE_H */
