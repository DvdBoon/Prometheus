#ifndef EXEC_RESIDENT_H
#define EXEC_RESIDENT_H
#include <exec/types.h>
struct Resident { UWORD rt_MatchWord; struct Resident *rt_MatchTag; APTR rt_EndSkip; UBYTE rt_Flags, rt_Version, rt_Type; BYTE rt_Pri; char *rt_Name; char *rt_IdString; APTR rt_Init; };
#define RTC_MATCHWORD 0x4AFC
#define RTF_AUTOINIT (1<<7)
#endif
