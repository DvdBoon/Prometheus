#ifndef settings_H
#define settings_H
/*
 * settings.h
 *
 * :ts=4
 */

#include <exec/lists.h>

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

enum
{
	PLANAR,
	CHUNKY,
	HICOLOR,
	TRUECOLOR,
	TRUEALPHA,
	MAXMODES
};

/************************************************************************/

#define	SETTINGSNAMEMAXCHARS	30
#define	BOARDNAMEMAXCHARS		30

/************************************************************************/

struct P96MonitorSpecs
{
	/* old P96MonitorInfo */
	TEXT	Name[32];		/* descriptive name, can be set by user */
	uint32	HSyncMin;		/* min horizontal frequency (hz) */
	uint32	HSyncMax;		/* max horizontal frequency (hz) */
	uint16	VSyncMin;		/* min vertical frequency (hz) */
	uint16	VSyncMax;		/* max vertical frequency (hz) */
	uint32	Flags;			/* various flags, see below */

	/* new fields */
	uint32	DotClockMin;	/* min dotclock (hz) */
	uint32	DotClockMax;	/* max dotclock (hz) */
	uint8	DisplayWidth;	/* max horizontal size (cm) */
	uint8	DisplayHeight;	/* max vertical size (cm) */
	uint16	DisplayGamma;	/* gamma (fraction of 100) */
	uint32	InputType;		/* enum enMonitorInputType */
	TEXT	VendorID[4];	/* three-byte Vendor ID assigned by Microsoft */
	TEXT	SerialNo[14];	/* serial number of this monitor (ascii) */
	TEXT	Product[14];
	uint32	Serial;			/* old serial number (integer) */
	uint16	ProductID;		/* product ID */
	uint16	Year;			/* year of manufacture */
	uint16	Week;			/* week of manufacture, 0xff == model-year */
	uint8	EDIDVersion;	/* guess what... */
	uint8	EDIDRevision;
	uint16	StructSize;		/* actual size of this structure */
	uint16	NumModes;
	APTR *	ModeDB;			/* list of supported modes */
	BOOL	BasicAudio;		/* basic audio capability */
};

// Monitor supports DPMS-Level "stand-by".
// Not every DPMS-enabled monitor must support it.
// DPMS-fähige Monitor muß es unterstützen.
// Activation: hsync out, vsync on
#define MIB_DPMS_StandBy	(0)

// Monitor supports DPMS-Level "suspend".
// This feature is mandatory, each
// DPMS-enabled Monitor must support it.
// Activation: hsync on, vsync off
#define MIB_DPMS_Suspend	(1)

// Monitor supports DPMS-Level "active off".
// This feature is mandatory, each
// DPMS-enabled Monitor must support it.
// Activation: hsync out, vsync off
#define MIB_DPMS_ActiveOff	(2)

#define MIF_DPMS_StandBy	(1UL << MIB_DPMS_StandBy)
#define MIF_DPMS_Suspend	(1UL << MIB_DPMS_Suspend)
#define MIF_DPMS_ActiveOff	(1UL << MIB_DPMS_ActiveOff)

/************************************************************************/

struct Settings
{
	struct Node			Node;
	struct MinList		Resolutions;
	ULONG					BoardType;
// a value describing assignment to nth board local to boardtype
// to be used for reassignment when boards are added or removed.
	UWORD					LocalOrdering;
	WORD					LastSelected;
	char					NameField[SETTINGSNAMEMAXCHARS];
	char					*BoardName;
	struct P96MonitorInfo *MonitorInfo;
};

#define MAXRESOLUTIONNAMELENGTH 22

/********************************
 * only used within rtg.library *
 ********************************/
struct LibResolution
{
	struct Node				Node;
	char					P96ID[6];
	char					Name[MAXRESOLUTIONNAMELENGTH];
	ULONG					DisplayID;
	UWORD					Width;
	UWORD					Height;
	UWORD					Flags;
	struct ModeInfo			*Modes[MAXMODES];
	struct BoardInfo		*BoardInfo;
	struct LibResolution	*HashChain;
};

/*****************************
 * only used within MoniTool *
 *****************************/
struct Resolution
{
	struct Node		Node;
	struct MinList	ModeInfos;
	ULONG			DisplayID;
	UWORD			Width;
	UWORD			Height;
	BOOL			Active;
	WORD			LastSelected;
	UWORD			Flags;
	char			Name[MAXRESOLUTIONNAMELENGTH];
};

#define	P96B_FAMILY		0	// obsolete (Resolution is an entire family)
#define	P96B_PUBLIC		1	// Resolution should be added to the public
#define	P96B_MONITOOL	2
#define P96B_PREFERRED	3	// Resolution is prefered mode of monitor (from DDC)
#define	P96B_CHECKME	15	// Resolution has been attached to another board
							// by AttachSettings without being checked against
							// hardware limits

#define	P96F_FAMILY		(1<<P96B_FAMILY)	// obsolete
#define	P96F_PUBLIC		(1<<P96B_PUBLIC)
#define	P96F_MONITOOL	(1<<P96B_MONITOOL)
#define P96F_PREFERRED	(1<<P96B_PREFERRED)
#define	P96F_CHECKME	(1<<P96B_CHECKME)

/*
enum {
	DBLLORES_FLAGS,	// 0000		 320x200
	DBLHIRES_FLAGS,	// 8000		 640x200
	DBLSHIRES_FLAGS,	// 8020		1280x200
	LORES_FLAGS,		// 0004		 320x400
	HIRES_FLAGS,		// 8004		 640x400
	SHIRES_FLAGS,		// 8024		1280x400
	LORESLACE_FLAGS,	// 0005		 320x800
	HIRESLACE_FLAGS,	//	8005		 640x800
	SHIRESLACE_FLAGS,	// 8025		1280x800
	MAXFAMILYFLAGS
};
*/

/*****************************
 * this one describes a mode *
 *****************************/
struct ModeInfo
{
	struct Node	Node;						// used for linking ModeInfos e.g. within MoniTool
	WORD			OpenCount;
	BOOL			Active;
	UWORD			Width;
	UWORD			Height;
	UBYTE			Depth;
	UBYTE			Flags;

	UWORD			HorTotal;				// wichtig für aufziehen (beeinflußt Timings)
	UWORD			HorBlankSize;			// Rahmengröße
	UWORD			HorSyncStart;			// bestimmt Bildlage
	UWORD			HorSyncSize;			// muß Spezifikation für Sync-Lücke erfüllen

	UBYTE			HorSyncSkew;			// im Moment obsolet
	UBYTE			HorEnableSkew;			//

	UWORD			VerTotal;				// analog zu horizontalen Werten
	UWORD			VerBlankSize;
	UWORD			VerSyncStart;
	UWORD			VerSyncSize;

	union{
		UBYTE		Clock;					// Tseng: Nummer der Hardwareclock
		UBYTE		Numerator;				// Cirrus: Numerator für PLL
	} pll1;
	union{
		UBYTE		ClockDivide;			// Tseng: Clockteiler
		UBYTE		Denominator;			// Cirrus: Denominator für PLL
	} pll2;
	ULONG			PixelClock;				// PixelClock in Hz
};

/***********************************
* Flags: */

#define GMB_DOUBLECLOCK		0
#define GMB_INTERLACE		1
#define GMB_DOUBLESCAN		2
#define GMB_HPOLARITY		3
#define GMB_VPOLARITY		4
#define GMB_COMPATVIDEO		5
#define GMB_DOUBLEVERTICAL	6
#define GMB_ALWAYSBORDER	7	// only used by MoniTool-EditScreen

#define GMF_DOUBLECLOCK		(1L<<GMB_DOUBLECLOCK)
#define GMF_INTERLACE		(1L<<GMB_INTERLACE)
#define GMF_DOUBLESCAN		(1L<<GMB_DOUBLESCAN)
#define GMF_HPOLARITY		(1L<<GMB_HPOLARITY)
#define GMF_VPOLARITY		(1L<<GMB_VPOLARITY)
#define GMF_COMPATVIDEO		(1L<<GMB_COMPATVIDEO)
#define GMF_DOUBLEVERTICAL	(1L<<GMB_DOUBLEVERTICAL)
#define GMF_ALWAYSBORDER	(1L<<GMB_ALWAYSBORDER)

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
