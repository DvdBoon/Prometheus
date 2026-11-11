#ifndef P96_SEMAPHORE_H
#define P96_SEMAPHORE_H

/************************************************************************/
/* our private Semaphore stuff
 */

#include <proto/exec.h>

struct p96SemaphoreHandle
{
	struct Node		Link;
	BOOL				Exclusive;
	struct Task		*Task;
};

struct p96Semaphore
{
	struct Node					Link;
	WORD							NestCount;
	struct MinList				WaitQueue;
	struct MinList				HoldQueue;
	struct Task					*Owner;
	WORD							QueueCount;
	BYTE							CurrentMinHold;
	BYTE							CurrentMaxWait;
};


/************************************************************************/

#endif
