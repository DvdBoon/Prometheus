#ifndef EXEC_EXECBASE_H
#define EXEC_EXECBASE_H
#include <exec/libraries.h>
#include <exec/lists.h>
/* only the fields used here; AttnFlags is at offset 0x128 in the real ExecBase */
struct ExecBase { struct Library LibNode; UBYTE pad[0x128-34]; UWORD AttnFlags; UBYTE pad2[0x15e - 0x12a]; struct List DeviceList; };
#define AFB_68020 1
#define AFF_68020 (1<<1)
#endif
