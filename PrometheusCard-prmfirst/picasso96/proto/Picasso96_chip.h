#ifndef PROTO_PICASSO96_CHIP_H
#define PROTO_PICASSO96_CHIP_H

/*
**	$Id: Picasso96_chip.h,v 1.2 2005-05-17 15:51:24 rossv Exp $
**	Includes Release 50.1
**
**	Prototype/inline/pragma header file combo
**
**	(C) Copyright 2003-2005 Amiga, Inc.
**	    All Rights Reserved
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef BOARDINFO_H
#include <boardinfo.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
 #ifndef __USE_BASETYPE__
  extern struct Library * ChipBase;
 #else
  extern struct ChipBase * ChipBase;
 #endif /* __USE_BASETYPE__ */
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/Picasso96_chip.h>
 #ifdef __USE_INLINE__
  #include <inline4/Picasso96_chip.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_PICASSO96_CHIP_PROTOS_H
  #define CLIB_PICASSO96_CHIP_PROTOS_H 1
 #endif /* CLIB_PICASSO96_CHIP_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct ChipIFace *IChip;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_PICASSO96_CHIP_PROTOS_H
  #include <clib/Picasso96_chip_protos.h>
 #endif /* CLIB_PICASSO96_CHIP_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/Picasso96_chip.h>
  #else
   #include <ppcinline/Picasso96_chip.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/Picasso96_chip_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/Picasso96_chip_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_PICASSO96_CHIP_H */
