#ifndef PROTO_RTG_H
#define PROTO_RTG_H

/*
**	$Id$
**	Includes Release 50.1
**
**	Prototype/inline/pragma header file combo
**
**	Copyright (c) 2010 Hyperion Entertainment CVBA.
**	All Rights Reserved.
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef BOARDINFO_H
#include <boardinfo.h>
#endif
#ifndef RTGBASE_H
#include <rtgBase.h>
#endif
#ifndef INTUITION_SCREENS_H
#include <intuition/screens.h>
#endif
#ifndef LIBRARIES_PICASSO96_H
#include <libraries/Picasso96.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
 #ifndef __USE_BASETYPE__
  extern struct Library * RTGBase;
 #else
  extern struct RTGBase * RTGBase;
 #endif /* __USE_BASETYPE__ */
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/rtg.h>
 #ifdef __USE_INLINE__
  #include <inline4/rtg.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_RTG_PROTOS_H
  #define CLIB_RTG_PROTOS_H 1
 #endif /* CLIB_RTG_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct RTGIFace *IRTG;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_RTG_PROTOS_H
  #include <clib/rtg_protos.h>
 #endif /* CLIB_RTG_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/rtg.h>
  #else /* __PPC__ */
   #include <ppcinline/rtg.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/rtg_protos.h>
  #endif /* __PPC__ */
 #else /* __GNUC__ */
  #include <pragmas/rtg_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_RTG_H */
