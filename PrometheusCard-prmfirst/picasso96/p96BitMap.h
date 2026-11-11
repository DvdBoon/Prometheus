/** p96BitMap.h
 * Private bitmap data.
 */
#ifndef P96_BITMAP_H
#define P96_BITMAP_H

#ifndef P96_SEMAPHORE_H
#include <p96Semaphore.h>
#endif

#define MAGIC_BASE		0xf8c0			/* some kind of RTG-Screen */
#define MAGIC_MASK		0xfcc0
#define FORMAT_MASK		0x001f
#define RTG_CLUT		(MAGIC_BASE | RGBFB_CLUT)		/* RTG Chunky */

#define IsMagic(a_bitmap)		((a_bitmap->pad & MAGIC_MASK) == MAGIC_BASE)
#define MagicFormat(a_bitmap)	(a_bitmap->pad & FORMAT_MASK)

#define SaveMemory(a_bitmap)	((UBYTE *)a_bitmap->Planes[1])
#define BaseMemory(a_bitmap)	((UBYTE *)a_bitmap->Planes[2])
#define Data(a_bitmap)			((struct BitMapData *)a_bitmap->Planes[6])
#define ColorMapping(a_bitmap)	((struct ColorMapping *)(a_bitmap->Planes[7]))

#define MatchBitMaps(x,y)		(x && y && ((IsMagic(x) ? x->Planes[1] : x->Planes[0]) == (IsMagic(y) ? y->Planes[1] : y->Planes[0])))

// IMPORTANT: This pack pragma is required to keep the alignment of this structure correct (must match what it always was)
#pragma pack(2)

struct BitMapData
{
	uint32						Alignment;
	uint32						Flags;
	APTR						UserData;
	APTR						RenderFunc;
	APTR						SaveFunc;
	struct SignalSemaphore		NestCountLock;
	uint16						NestCount;
	struct p96SemaphoreHandle	Lock;
	struct BitMap *				BackLink;      // points back to bitmap this struct was allocated for
	struct BitMap *				MasterFriend;  // points to top level friend, probably the screen (as this one is allocated without friend)
	struct Hook *				NotifyHook;    // for bitmap change notification
	struct BitMapExtra *		Extra;
	BOOL						LTBNestCount;  // rtgLockBitMapToBoard() nest count
	uint32						ActualWidth;   // unaltered width passed to AllocBitMap()
};

#pragma pack()


#endif
