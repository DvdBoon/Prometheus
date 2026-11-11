#ifndef P4PALOMA_H
#define P4PALOMA_H
/*
 * $Id: p4paloma.h,v 1.2 2004-06-12 11:15:09 stephanr Exp $
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

struct P4PalomaConfig
{
    ULONG p4pal_VPX_Type;
    UBYTE p4pal_VPX_Addr;
    UBYTE p4pal_Tuner_Type;
    UBYTE p4pal_Tuner_Addr;
    UBYTE p4pal_Audio_Type;
    UBYTE p4pal_Audio_Addr;
    UBYTE p4pal_Control_Type;
    UBYTE p4pal_Control_Addr;
};

/*****************************************************************************/

#define P4VPX3220ID	    0xec4680	/* old Video-Decoder */
#define P4VPX3225ID	    0xec7230	/* new Video-Decoder with TeleText*/

#define P4I2CDEV_Paloma1	0x43	/* Paloma responds to that device ID. */
#define P4I2CDEV_Paloma2	0x47	/* alternative device ID */

#define P4TunerTypeNTSC		0x10	/* Tuner is NTSC type */
#define P4TunerTypeNTSC_US	0x10	/* Tuner is NTSC-USA type */
#define P4TunerTypeNTSC_JP	0x11	/* Tuner is NTSC-Japan type */
#define P4TunerTypeNTSC_XX	0x12	/* Tuner is unknown NTSC type */
#define P4TunerTypeNTSC_YY	0x13	/* Tuner is unknown NTSC type */

#define P4TunerTypePAL		0x20	/* Tuner is PAL type */
#define P4TunerTypePAL_BG	0x20	/* Tuner is PAL-B/G type (Europe/Germany) */
#define P4TunerTypePAL_I	0x21	/* Tuner is PAL-I type (UK/Ireland) */
#define P4TunerTypePAL_DK	0x22	/* Tuner is PAL-D/K type (China) */
#define P4TunerTypePAL_XX	0x23	/* Tuner is unknown PAL type */

#define P4TunerReadback		0x08	/* Tuner can be read back */

#define P4I2CDEV_TunerPAL	0x60	/* Tuner responds to that device ID */
#define P4I2CDEV_TunerNTSC	0x61	/* Tuner responds to that device ID */

#define P4PalomaAudio0Master	0x22	/* Master Volume */
#define P4PalomaAudio0TV		0x26	/* Tuner Volume */
#define P4PalomaAudio0MPEG		0x04	/* MPEG volume */

/*****************************************************************************/

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack()
   #endif
#elif defined(__VBCC__)
   #pragma default-align
#endif

#endif /* P4PALOMA_H */
