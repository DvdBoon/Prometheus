#ifndef EXEC_LIBRARIES_H
#define EXEC_LIBRARIES_H
#include <exec/nodes.h>
struct Library { struct Node lib_Node; UBYTE lib_Flags; UBYTE lib_pad; UWORD lib_NegSize, lib_PosSize, lib_Version, lib_Revision; APTR lib_IdString; ULONG lib_Sum; UWORD lib_OpenCnt; };
#define LIBF_SUMMING (1<<0)
#define LIBF_CHANGED (1<<1)
#define LIBF_SUMUSED (1<<2)
#define LIBF_DELEXP (1<<3)
#endif
