/*
 * 3DLabsPermedia2.chip - library base, RomTag and the standard library functions
 */
#pragma pack(2)
#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/libraries.h>
#include <exec/resident.h>
#include <exec/nodes.h>
#pragma pack()

#pragma pack(2)
#include "boardinfo.h"
#pragma pack()
#include "version.h"

struct ExecBase *SysBase;

#pragma pack(2)
#include <proto/exec.h>
#pragma pack()

extern ULONG InitChip(__REGA0(struct BoardInfo *bi));

struct ChipBase *LibInit(__REGD0(struct ChipBase *base), __REGA0(APTR seglist), __REGA6(struct ExecBase *sysbase));
struct ChipBase *LibOpen(__REGA6(struct ChipBase *base));
APTR LibClose(__REGA6(struct ChipBase *base));
APTR LibExpunge(__REGA6(struct ChipBase *base));
ULONG LibReserved(void);

static const char LibName[] = LIBNAME;
static const char LibId[] = VERSTRING;

static const APTR FuncTable[] = {
	(APTR)LibOpen,
	(APTR)LibClose,
	(APTR)LibExpunge,
	(APTR)LibReserved,
	(APTR)InitChip,
	(APTR)-1
};

static const ULONG InitTab[] = {
	sizeof(struct ChipBase),
	(ULONG)FuncTable,
	0,
	(ULONG)LibInit
};

const struct Resident RomTag = {
	RTC_MATCHWORD,
	(struct Resident *)&RomTag,
	(APTR)(&RomTag + 1),
	RTF_AUTOINIT,
	VERSION,
	NT_LIBRARY,
	0,
	(char *)LibName,
	(char *)LibId,
	(APTR)InitTab
};

static void FreeBase(struct ChipBase *base)
{
	struct Library *lib = &base->LibBase;

	FreeMem((UBYTE *)lib - lib->lib_NegSize, (ULONG)lib->lib_NegSize + lib->lib_PosSize);
}

struct ChipBase *LibInit(__REGD0(struct ChipBase *base), __REGA0(APTR seglist), __REGA6(struct ExecBase *sysbase))
{
	SysBase = sysbase;
	/* the code uses 68020 instructions */
	if (!(sysbase->AttnFlags & AFF_68020)) {
		FreeBase(base);
		return NULL;
	}
	base->LibBase.lib_Revision = REVISION;
	base->ExecBase = sysbase;
	base->SegList = seglist;
	return base;
}

struct ChipBase *LibOpen(__REGA6(struct ChipBase *base))
{
	base->LibBase.lib_OpenCnt++;
	base->LibBase.lib_Flags &= ~LIBF_DELEXP;
	return base;
}

APTR LibExpunge(__REGA6(struct ChipBase *base))
{
	APTR seglist;

	if (base->LibBase.lib_OpenCnt) {
		base->LibBase.lib_Flags |= LIBF_DELEXP;
		return NULL;
	}
	Remove(&base->LibBase.lib_Node);
	seglist = base->SegList;
	FreeBase(base);
	return seglist;
}

APTR LibClose(__REGA6(struct ChipBase *base))
{
	if (--base->LibBase.lib_OpenCnt == 0 && (base->LibBase.lib_Flags & LIBF_DELEXP))
		return LibExpunge(base);
	return NULL;
}

ULONG LibReserved(void)
{
	return 0;
}
