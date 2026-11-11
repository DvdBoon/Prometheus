#ifndef P4PABLO_H
#define P4PABLO_H
/*
 * $Id: p4pablo.h,v 1.2 2004-06-12 11:15:09 stephanr Exp $
 *
 * Copyright © 1998 Village Tronic Marketing GmbH
 * All Rights Reserved
 */

#include <exec/libraries.h>
#include <exec/semaphores.h>

#include <utility/tagitem.h>

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack(2)
   #endif
#elif defined(__VBCC__)
   #pragma amiga-align
#endif

/*****************************************************************************/

enum /* Video signal type */
{
    PABLOSIG_SVHS,
    PABLOSIG_FBAS
};

enum /* Video output modes */
{
    PABLOVID_Blank,
    PABLOVID_Ramp,
    PABLOVID_Bars,
    PABLOVID_Normal
};

enum /* Video standard */
{
    PABLOSTD_NTSC_EIA,
    PABLOSTD_NTSC,
    PABLOSTD_PAL_M,
    PABLOSTD_PAL_BGI_640,
    PABLOSTD_PAL_BGI_800
};

enum /* Display offset push direction */
{
    PABLOPUSH_Horizontal,
    PABLOPUSH_Vertical
};

struct P4PabloConfig
{
    UBYTE p4pc_Initialized;   /* FALSE if the following data is entirely uninitialized */
    UBYTE p4pc_Enabled;	      /* FALSE if in powerdown mode */
    UBYTE p4pc_VideoSignal;   /* SVHS, FBAS */
    UBYTE p4pc_VideoMode;     /* blank, ramp, bars, normal */
    UBYTE p4pc_VideoStandard; /* NTSC-EIA, NTSC, PAL M, PAL BGI 640, PAL BGI 800 */
    UBYTE p4pc_DefaultStandard; /* NTSC-EIA, NTSC, PAL M, PAL BGI 640, PAL BGI 800 */
    BYTE  p4pc_OffsetX;       /* -8..8 */
    BYTE  p4pc_OffsetY;       /* -8..8 */
    UBYTE p4pc_FilterLines;   /* 0..2, i.e. 0 = no filter, 1 = 2 lines, etc. */
    BYTE  p4pc_DirectionX;    /* -1 or 1 */
    BYTE  p4pc_DirectionY;    /* -1 or 1 */
    UBYTE p4pc_RGBLoads;      /* FALSE if RGB loads are not enabled. */
};

/*****************************************************************************/

#define P4PA_BASE		(TAG_USER+7000)
#define P4PA_SET_InitialSetup	(P4PA_BASE+1)
#define P4PA_SET_VideoStandard	(P4PA_BASE+2)
#define P4PA_SET_VideoMode	(P4PA_BASE+3)
#define P4PA_SET_FilterLines	(P4PA_BASE+4)
#define P4PA_SET_VideoSignal	(P4PA_BASE+5)
#define P4PA_SET_OffsetX	(P4PA_BASE+6)
#define P4PA_SET_OffsetY	(P4PA_BASE+7)
#define P4PA_SET_Enable		(P4PA_BASE+8)
#define P4PA_SET_RGBLoads	(P4PA_BASE+9)
#define P4PA_GET_VideoStandard	(P4PA_BASE+10)
#define P4PA_GET_VideoMode	(P4PA_BASE+11)
#define P4PA_GET_FilterLines	(P4PA_BASE+12)
#define P4PA_GET_VideoSignal	(P4PA_BASE+13)
#define P4PA_GET_OffsetX	(P4PA_BASE+14)
#define P4PA_GET_OffsetY	(P4PA_BASE+15)
#define P4PA_GET_RGBLoads	(P4PA_BASE+16)
#define P4PA_GET_Enable		(P4PA_BASE+17)

/*****************************************************************************/

#define P4I2CDEV_PabloIV 0x6E	/* Pablo IV responds to that device ID. */
// #define P4I2CDEV_PabloII 0x6E /* obsolete, use Pablo IV */

/*****************************************************************************/

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack()
   #endif
#elif defined(__VBCC__)
   #pragma default-align
#endif

#endif /* P4PABLO_H */
