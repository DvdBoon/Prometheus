#ifndef PICASSOIVRESOURCE_H
#define PICASSOIVRESOURCE_H
/*
 * $Id: PicassoIVresource.h,v 1.2 2004-06-12 11:15:09 stephanr Exp $
 *
 * Copyright © 1998 Village Tronic Marketing GmbH
 * All Rights Reserved
 */

#include <exec/libraries.h>
#include <exec/semaphores.h>

#include <utility/tagitem.h>
#include "p4flifi.h"
#include "p4pablo.h"
#include "p4paloma.h"

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack(2)
   #endif
#elif defined(__VBCC__)
   #pragma amiga-align
#endif

/*****************************************************************************/

#define P4_NUMBOARDS 10

/*****************************************************************************/

/*
 *
 * Notes:
 * * A Picasso IV can be either Zorro II or Zorro III, depending on machine
 *   and board jumpers.
 * * There are various config options on Zorro II:
 *   * four megabytes: this is true if p4bi_MemoryBase2 is not NULL
 *   * two megabytes: this is true if p4bi_MemoryBase2 is NULL
 *   * only I/O: this is true if p4bi_MemoryBase1 and p4bi_MemoryBase2 are NULL
 *     (the board still can be used as a flicker fixer)
 */
struct P4BoardInfo
{
    ULONG p4bi_Flags;           /* see below                                         */
    ULONG p4bi_MemoryBase1;     /* first two megabytes of memory (may be NULL!)      */
    ULONG p4bi_MemoryBase2;     /* second two megabytes of memory (may be NULL!)     */
    ULONG p4bi_GPPortBase;      /* General Purpose Port address                      */
    ULONG p4bi_RegisterBase;    /* I/O register base                                 */
    ULONG p4bi_ConfigBase;      /* PCI configspace base                              */
    ULONG p4bi_MMIOBase;        /* Memory mapped I/O base                            */
    ULONG p4bi_Modules;         /* Presence flags for Multimedia Extensions          */
    UBYTE p4bi_BridgeRevision;  /* Revision of Zorro<->PCI brdige           PRIVATE! */
    UBYTE p4bi_FFRevision;      /* Revision of FlickFixer Controller        PRIVATE! */
    UBYTE p4bi_Revisions3;      /* Revision of programmable logic devices   PRIVATE! */
    UBYTE p4bi_Revisions4;      /* Revision of programmable logic devices   PRIVATE! */
    UBYTE p4bi_BankingSetting;  /* Setting of banking register              PRIVATE! */
    UBYTE p4bi_AudioSettings;   /* Settings of AudioSelect register         PRIVATE! */
    UBYTE p4bi_SecretSettings3; /* Settings of writeonly registers          PRIVATE! */
    UBYTE p4bi_SecretSettings4; /* Settings of writeonly registers          PRIVATE! */

    struct SignalSemaphore p4bi_BoardSema;                               /* PRIVATE! */
    /* last struct is word aligned */
    UBYTE p4bi_pad0;	/* Note: one byte hijacked for interrupt status */

    UBYTE          p4bi_IntStatus;    /* Current interrupt status, if called from handler */
    struct MinList p4bi_HandlerChain; /* Interrupt handler chain; Private! */

    /* New for V4.1 */
    struct P4Timing *p4bi_PALTiming;    /* pointer to PAL timing; PRIVATE! */
    struct P4Timing *p4bi_NTSCTiming;   /* pointer to NTSC timing; PRIVATE! */
    struct P4Timing *p4bi_CustomTiming; /* pointer to custom timing; can be NULL; PRIVATE! */

    UBYTE p4bi_AVMSelect; /* either AV module or MPEG module supplies audio signal */
    UBYTE p4bi_pad1;

    /* New for V5.3 */
    struct P4PabloConfig *p4bi_PabloConfig; /* if non-NULL, points to Pablo II control data. */

    /* New for V7.3 */
    struct P4PalomaConfig *p4bi_P4PalomaConfig; /* if non-NULL, points to Paloma control data. */
    ULONG  p4bi_ActiveContext;
};

/*
 * bit definitions for p4bi_Flags
 */
#define P4BIB_Z3            1   /* board is a Zorro III board           */
#define P4BIB_FF            2   /* board is FlickerFixer only           */
#define P4BIB_BAD           3   /* board is defective                   */
#define P4BIB_REV_B         4   /* Gfx-Chip is CL-GD5446 Rev.B          */
#define P4BIB_I2C_BAD       8   /* I2C bus is defective                 */
#define P4BIB_DDC_BAD       9   /* DDC bus is defective                 */
#define P4BIB_MONO_MON      10  /* Monochrome Monitor attached          */
#define P4BIB_COLOR_MON     11  /* Color Monitor attached               */
#define P4BIB_INUSEBYGS     31  /* if set, board is already used by a   */
                                /* graphics system like Picasso96       */

#define P4BIF_Z3            (1 << P4BIB_Z3)
#define P4BIF_FF            (1 << P4BIB_FF)
#define P4BIF_BAD           (1 << P4BIB_BAD)
#define P4BIF_REV_B         (1 << P4BIB_REV_B)
#define P4BIF_I2C_BAD       (1 << P4BIB_I2C_BAD)
#define P4BIF_DDC_BAD       (1 << P4BIB_DDC_BAD)
#define P4BIF_MONO_MON      (1 << P4BIB_MONO_MON)
#define P4BIF_COLOR_MON     (1 << P4BIB_COLOR_MON)
#define P4BIF_INUSEBYGS     (1 << P4BIB_INUSEBYGS)


/*
 * bit definitions for p4bi_Modules
 */
#define P4BIEXB_FF_ANY      0   /* board connected to Amiga VideoSlot/Chip */
#define P4BIEXB_FF_AA       1   /* board connected to AA VideoSlot/Chip    */
#define P4BIEXB_PABLO       2   /* board has Pablo2 installed              */
#define P4BIEXB_VIDEO       3   /* board has VideoCapture module installed */
#define P4BIEXB_TV_PAL      4   /* board has PAL TV-tuner                  */
#define P4BIEXB_TV_NTSC     5   /* board has NTSC TV-tuner                 */
#define P4BIEXB_AUDIO       6   /* board has audio module installed        */
#define P4BIEXB_MPEG        7   /* board has MPEG module installed         */
#define P4BIEXB_TEXT        8   /* VideoCapture has Teletext capability    */
#define P4BIEXB_STEREO      9   /* VideoCapture has Stereo decode          */

#define P4BIEXF_FF_ANY      (1 << P4BIEXB_FF_ANY)
#define P4BIEXF_FF_AA       (1 << P4BIEXB_FF_AA)
#define P4BIEXF_PABLO       (1 << P4BIEXB_PABLO)
#define P4BIEXF_VIDEO       (1 << P4BIEXB_VIDEO)
#define P4BIEXF_TV_PAL      (1 << P4BIEXB_TV_PAL)
#define P4BIEXF_TV_NTSC     (1 << P4BIEXB_TV_NTSC)
#define P4BIEXF_AUDIO       (1 << P4BIEXB_AUDIO)
#define P4BIEXF_MPEG        (1 << P4BIEXB_MPEG)
#define P4BIEXF_TEXT        (1 << P4BIEXB_TEXT)
#define P4BIEXF_STEREO      (1 << P4BIEXB_STEREO)


/*
 * bit definitions for p4bi_BankingSetting (SpecialFeatures)
 */
#define P4BIBANKB_RESET     0   /* local Reset pin (active low)          */
#define P4BIBANKB_ROMSEL    1   /* ROM mapped in (active low)            */
#define P4BIBANKB_MEMSEL    2   /* MMIO and GP mapped in (active high)   */
#define P4BIBANKB_COMPRESS  3   /* ARGB -> RGB compression (active high) */
#define P4BIBANKB_FLASH_A16 7   /* A16 line for FlashROM                 */

#define P4BIBANKF_RESET     (1 << P4BIBANKB_RESET)
#define P4BIBANKF_ROMSEL    (1 << P4BIBANKB_ROMSEL)
#define P4BIBANKF_MEMSEL    (1 << P4BIBANKB_MEMSEL)
#define P4BIBANKF_COMPRESS  (1 << P4BIBANKB_COMPRESS)
#define P4BIBANKF_FLASH_A16 (1 << P4BIBANKB_FLASH_A16)

/*
 * Offsets for GP-Bus
 */
#define FLIFIBASE       0x400   /* Base Offset of Flickerfixer           */
#define FLIFI_AUDIO       0x0   /* Offset for AudioSelect                */
#define FLIFI_AUX         0x2   /* Offset for AuxSelect                  */
#define FLIFI_FF          0x4   /* Offset for FliFi-Control              */
#define FLIFI_I2C         0x6   /* Offset for I2C Control                */
#define FLIFI_INTERRUPT   0x8   /* Offset for interrupt-handling         */

#define PALOMABASE      0x800   /* Base Offset of Video-Module           */
#define PALOMA_ENABLE     0x0   /* Offset for Output Enable              */
#define PALOMA_LOAD       0x2   /* Offset for Feedback Load              */
#define PALOMA_AUD_IDX    0x4   /* Offset for Audio-Mixer Index          */
#define PALOMA_AUD_DAT    0x6   /* Offset for Audio-Mixer Data           */

#define CONCIERTOBASE   0xc00   /* Base Offset of Audio-Module           */

/*
 * Bit definitions for GP-Bus interrupt sources.
 */
#define GPINTB_Sample     0     /* ESS-1688 IRQA (sampler) */
#define GPINTB_MIDI       1     /* ESS-1688 IRQE (MPU-401) */
#define GPINTB_GPUnused3  2     /* GP-Bus interrupt #3 (not used yet) */
#define GPINTB_GPUnused4  3     /* GP-Bus interrupt #4 (not used yet) */
#define GPINTB_PCIUnused1 4     /* PCI interrupt #1 (not used yet) */
#define GPINTB_PCIUnused2 5     /* PCI interrupt #2 (not used yet) */
#define GPINTB_Capture    6     /* Cirrus capture interrupt */
#define GPINTB_VBlank     7     /* Cirrus vertical blanking interrupt */

#define GPINTF_Sample     (1 << GPINTB_Sample)
#define GPINTF_MIDI       (1 << GPINTB_MIDI)
#define GPINTF_GPUnused3  (1 << GPINTB_GPUnused3)
#define GPINTF_GPUnused4  (1 << GPINTB_GPUnused4)
#define GPINTF_PCIUnused1 (1 << GPINTB_PCIUnused1)
#define GPINTF_PCIUnused2 (1 << GPINTB_PCIUnused2)
#define GPINTF_Capture    (1 << GPINTB_Capture)
#define GPINTF_VBlank     (1 << GPINTB_VBlank)

/*
 * Audio signal switcher sources.
 */
#define SWITCHERSOURCE_Amiga  0
#define SWITCHERSOURCE_AV     1
#define SWITCHERSOURCE_LineIn 2
#define SWITCHERSOURCE_CD     3

/*
 * This bit is set if the output of the
 * switcher is muted.
 */
#define SWITCHERSOURCEB_Mute  7
#define SWITCHERSOURCEF_Mute  (1 << SWITCHERSOURCEB_Mute)

/*
 * AV/MPEG switcher states
 */
#define AVMSOURCE_MPEG 0	/* MPEG module supplies sound (default) */
#define AVMSOURCE_AV   1	/* AV module supplies sound */

/*****************************************************************************/

struct P4Resource
{
    struct Library p4res_lib;
    /* last struct is word aligned */

    UWORD			p4res_boardcount;	/* number of boards installed */
    APTR			p4res_sysbase;		/* pointer to SysBase */

    struct P4Timing *		p4res_PALTiming;	/* pointer to PAL timing; obsolete as of V4.1 */
    struct P4Timing *		p4res_NTSCTiming;	/* pointer to NTSC timing; obsolete as of V4.1 */
    struct P4Timing *		p4res_CustomTiming;	/* pointer to custom timing; obsolete as of V4.1 */

    struct P4BoardInfo *	p4res_boardinfos[P4_NUMBOARDS];

    struct Interrupt *		p4res_IntServer;	/* New for V2.1: handler for ports interrupt. */

    struct Library *		p4res_UtilityBase;	/* New for V5.1 */
};

#define PICASSOIVRESNAME "PicassoIV.resource"

/*****************************************************************************/

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack()
   #endif
#elif defined(__VBCC__)
   #pragma default-align
#endif

#endif /* PICASSOIVRESOURCE_H */
