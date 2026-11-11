#define ForeachNode(l,n) \
    for (n=(void *)(((struct List *)(l))->lh_Head); \
        ((struct Node *)(n))->ln_Succ; \
        n=(void *)(((struct Node *)(n))->ln_Succ))


#define ForeachNodeSafe(l,n,n2) \
    for (n=(void *)(((struct List *)(l))->lh_Head); \
        (n2=(void *)((struct Node *)(n))->ln_Succ); \
        n=(void *)n2)

#define ListLength(list,count)        \
    do {                              \
        struct Node * n;              \
        count = 0;                    \
        ForeachNode (list,n) count++; \
    } while (0)

#define NewList(l)                                                                \
    (((struct List *)l)->lh_TailPred = (struct Node *)(l),                        \
    ((struct List *)l)->lh_Tail = 0,                                              \
    ((struct List *)l)->lh_Head = (struct Node *) &(((struct List *)l)->lh_Tail)) \
