#ifndef UTILITY_TAGITEM_H
#define UTILITY_TAGITEM_H
#include <exec/types.h>
struct TagItem { Tag ti_Tag; ULONG ti_Data; };
#define TAG_DONE 0
#define TAG_END 0
#define TAG_USER (1UL<<31)
#endif
