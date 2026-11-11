#ifndef P4FLIFI_H
#define P4FLIFI_H
/*
 * $Id: p4flifi.h,v 1.2 2004-06-12 11:15:09 stephanr Exp $
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

struct P4ColorMode
{
    UBYTE  OffsetDivide;
    UBYTE  OffsetMultiply;
    UBYTE  CommandRegister;
    UBYTE  ClockDivide;
};

struct P4Timing
{
    UWORD  PixelParam;
    UWORD  HDisplayEnd;
    UWORD  HSyncStart;
    UWORD  HSyncEnd;
    UWORD  HTotal;
    UWORD  VDisplayEnd;
    UWORD  VSyncStart;
    UWORD  VSyncEnd;
    UWORD  VTotal;
    UWORD  HCapture;
    UWORD  VCapture;
    UWORD  Flags;
};

/* defines for P4Timing.Flags */
#define	P4TF_HSYNC		0x01	// if set, active positive
#define	P4TF_VSYNC		0x02	// if set, active positive
#define	P4TF_SCANMODEMASK	0x0C
#define	P4TF_SCAN_NORMAL	0x00
#define	P4TF_SCAN_INTERLACE	0x04
#define	P4TF_SCAN_DOUBLESCAN	0x08

/*****************************************************************************/

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack()
   #endif
#elif defined(__VBCC__)
   #pragma default-align
#endif

#endif /* P4FLIFI_H */
