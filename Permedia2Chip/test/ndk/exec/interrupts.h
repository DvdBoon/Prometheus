#ifndef EXEC_INTERRUPTS_H
#define EXEC_INTERRUPTS_H
#include <exec/nodes.h>
#include <exec/lists.h>
struct Interrupt { struct Node is_Node; APTR is_Data; void (*is_Code)(void); };
#endif
