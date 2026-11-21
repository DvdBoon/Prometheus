#ifndef EXEC_PORTS_H
#define EXEC_PORTS_H
#include <exec/lists.h>
#include <exec/tasks.h>
struct MsgPort { struct Node mp_Node; UBYTE mp_Flags; UBYTE mp_SigBit; void *mp_SigTask; struct List mp_MsgList; };
struct Message { struct Node mn_Node; struct MsgPort *mn_ReplyPort; UWORD mn_Length; };
#endif
