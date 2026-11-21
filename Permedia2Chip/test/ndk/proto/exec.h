/* cloud test build only: minimal exec inlines (register calling via a6) */
#ifndef PROTO_EXEC_H
#define PROTO_EXEC_H
#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/interrupts.h>
#include <exec/lists.h>

#ifndef EXEC_BASE_NAME
#define EXEC_BASE_NAME SysBase
#define FindTask(n) ({ register struct ExecBase *_a6 __asm("a6") = (EXEC_BASE_NAME); \
	register const char *_a1 __asm("a1") = (n); register ULONG _d0 __asm("d0"); \
	__asm volatile ("jsr -0x126(%%a6)" : "=r"(_d0), "+r"(_a1) : "r"(_a6) : "d1","a0","cc","memory"); (struct Task *)_d0; })
#endif
#define EXEC_CLOBBER "d0","d1","a0","a1","cc","memory"

#define Disable() ({ register struct ExecBase *_a6 __asm("a6") = (EXEC_BASE_NAME); \
	__asm volatile ("jsr -0x78(%%a6)" : : "r"(_a6) : EXEC_CLOBBER); })
#define Enable() ({ register struct ExecBase *_a6 __asm("a6") = (EXEC_BASE_NAME); \
	__asm volatile ("jsr -0x7e(%%a6)" : : "r"(_a6) : EXEC_CLOBBER); })
#define CacheClearU() ({ register struct ExecBase *_a6 __asm("a6") = (EXEC_BASE_NAME); \
	__asm volatile ("jsr -0x27c(%%a6)" : : "r"(_a6) : EXEC_CLOBBER); })
#define Alert(code) ({ register struct ExecBase *_a6 __asm("a6") = (EXEC_BASE_NAME); \
	register ULONG _d7 __asm("d7") = (code); \
	__asm volatile ("jsr -0x6c(%%a6)" : : "r"(_a6), "r"(_d7) : EXEC_CLOBBER); })
#define Cause(i) ({ register struct ExecBase *_a6 __asm("a6") = (EXEC_BASE_NAME); \
	register APTR _a1 __asm("a1") = (i); \
	__asm volatile ("jsr -0xb4(%%a6)" : "+r"(_a1) : "r"(_a6) : "d0","d1","a0","cc","memory"); })
#define Remove(n) ({ register struct ExecBase *_a6 __asm("a6") = (EXEC_BASE_NAME); \
	register APTR _a1 __asm("a1") = (n); \
	__asm volatile ("jsr -0xfc(%%a6)" : "+r"(_a1) : "r"(_a6) : "d0","d1","a0","cc","memory"); })
#define FreeMem(p,s) ({ register struct ExecBase *_a6 __asm("a6") = (EXEC_BASE_NAME); \
	register APTR _a1 __asm("a1") = (p); register ULONG _d0 __asm("d0") = (s); \
	__asm volatile ("jsr -0xd2(%%a6)" : "+r"(_a1), "+r"(_d0) : "r"(_a6) : "d1","a0","cc","memory"); })
#define Forbid() ({ register struct ExecBase *_a6 __asm("a6") = (EXEC_BASE_NAME); \
	__asm volatile ("jsr -0x84(%%a6)" : : "r"(_a6) : EXEC_CLOBBER); })
#define Permit() ({ register struct ExecBase *_a6 __asm("a6") = (EXEC_BASE_NAME); \
	__asm volatile ("jsr -0x8a(%%a6)" : : "r"(_a6) : EXEC_CLOBBER); })
#define AllocMem(s,f) ({ register struct ExecBase *_a6 __asm("a6") = (EXEC_BASE_NAME); \
	register ULONG _d0 __asm("d0") = (s); register ULONG _d1 __asm("d1") = (f); \
	__asm volatile ("jsr -0xc6(%%a6)" : "+r"(_d0), "+r"(_d1) : "r"(_a6) : "a0","a1","cc","memory"); (APTR)_d0; })
#define FindSemaphore(n) ({ register struct ExecBase *_a6 __asm("a6") = (EXEC_BASE_NAME); \
	register const char *_a1 __asm("a1") = (n); register ULONG _d0 __asm("d0"); \
	__asm volatile ("jsr -0x252(%%a6)" : "=r"(_d0), "+r"(_a1) : "r"(_a6) : "d1","a0","cc","memory"); (struct SignalSemaphore *)_d0; })
#define AddSemaphore(x) ({ register struct ExecBase *_a6 __asm("a6") = (EXEC_BASE_NAME); \
	register APTR _a1 __asm("a1") = (x); \
	__asm volatile ("jsr -0x258(%%a6)" : "+r"(_a1) : "r"(_a6) : "d0","d1","a0","cc","memory"); })
#define AttemptSemaphore(x) ({ register struct ExecBase *_a6 __asm("a6") = (EXEC_BASE_NAME); \
	register APTR _a0 __asm("a0") = (x); register ULONG _d0 __asm("d0"); \
	__asm volatile ("jsr -0x240(%%a6)" : "=r"(_d0), "+r"(_a0) : "r"(_a6) : "d1","a1","cc","memory"); (ULONG)_d0; })
#define ReleaseSemaphore(x) ({ register struct ExecBase *_a6 __asm("a6") = (EXEC_BASE_NAME); \
	register APTR _a0 __asm("a0") = (x); \
	__asm volatile ("jsr -0x23a(%%a6)" : "+r"(_a0) : "r"(_a6) : "d0","d1","a1","cc","memory"); })
#define FindTask(n) ({ register struct ExecBase *_a6 __asm("a6") = (EXEC_BASE_NAME); \
	register const char *_a1 __asm("a1") = (n); register ULONG _d0 __asm("d0"); \
	__asm volatile ("jsr -0x126(%%a6)" : "=r"(_d0), "+r"(_a1) : "r"(_a6) : "d1","a0","cc","memory"); (struct Task *)_d0; })
#endif
