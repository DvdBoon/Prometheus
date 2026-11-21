#ifndef EXEC_TYPES_H
#define EXEC_TYPES_H
/* minimal NDK replacement for cloud test builds only */
typedef void *APTR; typedef long LONG; typedef unsigned long ULONG; typedef short WORD; typedef unsigned short UWORD;
typedef signed char BYTE; typedef unsigned char UBYTE; typedef short SHORT; typedef unsigned short USHORT; typedef short BOOL;
typedef unsigned char *STRPTR; typedef long BPTR; typedef unsigned char TEXT; typedef ULONG Tag; typedef float FLOAT;
#define VOID void
#define TRUE 1
#define FALSE 0
#ifndef NULL
#define NULL ((void*)0)
#endif
#define CONST const
#endif
