#ifndef PROTO_CGXVIDEO_H
#define PROTO_CGXVIDEO_H

/*
**	$Id: cgxvideo.h,v 1.3 2005-09-03 09:23:08 stephanr Exp $
**	Includes Release 50.1
**
**	Prototype/inline/pragma header file combo
**
**	(C) Copyright 2003-2004 Amiga, Inc.
**	    All Rights Reserved
*/

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
 #ifndef __USE_BASETYPE__
  extern struct Library * CGXVideoBase;
 #else
  extern struct Library * CGXVideoBase;
 #endif /* __USE_BASETYPE__ */
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/cgxvideo.h>
 #ifdef __USE_INLINE__
  #include <inline4/cgxvideo.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_CGXVIDEO_PROTOS_H
  #define CLIB_CGXVIDEO_PROTOS_H 1
 #endif /* CLIB_CGXVIDEO_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct CGXVideoIFace *ICGXVideo;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_CGXVIDEO_PROTOS_H
  #include <clib/cgxvideo_protos.h>
 #endif /* CLIB_CGXVIDEO_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/cgxvideo.h>
  #else
   #include <ppcinline/cgxvideo.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/cgxvideo_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/cgxvideo_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_CGXVIDEO_H */
