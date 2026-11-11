#ifndef PROTO_PICASSO96_CARD_H
#define PROTO_PICASSO96_CARD_H

/*
**	$Id: Picasso96_card.h,v 1.5 2004-05-31 22:09:23 stephanr Exp $
**	Includes Release 50.1
**
**	Prototype/inline/pragma header file combo
**
**	(C) Copyright 2003-2004 Amiga, Inc.
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
  extern struct Library * CardBase;
 #else
  extern struct Library * CardBase;
 #endif /* __USE_BASETYPE__ */
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/Picasso96_card.h>
 #ifdef __USE_INLINE__
  #include <inline4/Picasso96_card.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_PICASSO96_CARD_PROTOS_H
  #define CLIB_PICASSO96_CARD_PROTOS_H 1
 #endif /* CLIB_PICASSO96_CARD_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct CardIFace *ICard;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_PICASSO96_CARD_PROTOS_H
  #include <clib/Picasso96_card_protos.h>
 #endif /* CLIB_PICASSO96_CARD_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/Picasso96_card.h>
  #else
   #include <ppcinline/Picasso96_card.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/Picasso96_card_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/Picasso96_card_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_PICASSO96_CARD_H */
