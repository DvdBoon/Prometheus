#ifndef PROTO_VILINTUISUP_H
#define PROTO_VILINTUISUP_H

/*
**	$Id: vilintuisup.h,v 1.2 2005-09-03 09:23:08 stephanr Exp $
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
#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif
#ifndef INTUITION_SCREENS_H
#include <intuition/screens.h>
#endif
#ifndef VILINTUISUP_VILINTUISUP_H
#include <vilintuisup/vilintuisup.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
 #ifndef __USE_BASETYPE__
  extern struct Library * VilIntuiBase;
 #else
  extern struct Library * VilIntuiBase;
 #endif /* __USE_BASETYPE__ */
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/vilintuisup.h>
 #ifdef __USE_INLINE__
  #include <inline4/vilintuisup.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_VILINTUISUP_PROTOS_H
  #define CLIB_VILINTUISUP_PROTOS_H 1
 #endif /* CLIB_VILINTUISUP_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct VilIntuiIFace *IVilIntui;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_VILINTUISUP_PROTOS_H
  #include <clib/vilintuisup_protos.h>
 #endif /* CLIB_VILINTUISUP_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/vilintuisup.h>
  #else
   #include <ppcinline/vilintuisup.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/vilintuisup_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/vilintuisup_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_VILINTUISUP_H */
