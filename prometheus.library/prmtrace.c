/*
 * prmtrace.c -- trace build of prometheus.library (compiled in with
 * -DPRMTRACE; included by driver_2.c, nothing of it is in the normal build).
 *
 * Every D(kprintf(...)) of driver_2.c and every public Prm_* call (through
 * the T_ wrappers below, with arguments, result and the calling task's name)
 * becomes one line of SYS:prmtrace.log.
 *
 * Only the logger process ("prmtrace") does file I/O: it opens the file,
 * appends one line or a block of the ring, and closes it again, so whatever
 * reached the disk survives a hang or a reset.
 * - A caller that may wait (no interrupt, no Forbid, no Disable) hands its
 *   line to the logger in a message and waits for the reply: its line is on
 *   disk before the call goes on into the hardware.
 * - A caller in an interrupt, under Forbid or under Disable only copies its
 *   line into a static ring (under Disable) and signals the logger, which
 *   writes it later; when the ring is full, lines are counted as lost.
 * - The logger is started once, by the first process that may wait
 *   (CreateNewProc, the only DOS call made in a caller's context). Until
 *   then, lines go to the ring; the library's init lines are kept that way.
 * - Stack use per call: about 300 bytes (the message with its text).
 */
#pragma pack(push,2)
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <proto/dos.h>
#pragma pack(pop)

#define TR_LOGFILE "SYS:prmtrace.log"
#define TR_RING    16384
#define TR_TEXT    200

struct Library *DOSBase = NULL;

struct TrMsg {
    struct Message tm_Msg;
    char tm_Text[TR_TEXT];
};

static struct MsgPort * volatile tr_port = NULL;
static struct Task * volatile tr_task = NULL;
static struct Task *tr_starter = NULL;
static LONG tr_startsig = -1;
static volatile UBYTE tr_starting = 0;
static char tr_ring[TR_RING];
static volatile ULONG tr_rlen = 0, tr_lost = 0;
static ULONG tr_seq = 0;

struct tr_out { char *p, *end; };

void tr_putch(__reg("d0") UBYTE c, __reg("a3") struct tr_out *o)
{
    if (c && o->p < o->end) *o->p++ = c;
}

static ULONG tr_strlen(const char *s)
{
    ULONG n = 0;
    while (s[n]) n++;
    return n;
}

/* the logger only */
static void tr_write(const char *s, ULONG n)
{
    BPTR fh;
    if (!n) return;
    fh = Open(TR_LOGFILE, MODE_READWRITE);
    if (fh) {
        Seek(fh, 0, OFFSET_END);
        Write(fh, (APTR)s, n);
        Close(fh);
    }
}

/* the logger only: move up to 1 KB of the ring out under Disable, write it */
static void tr_flushring(char *tmp)
{
    struct ExecBase *SysBase = *(struct ExecBase **)4L;
    ULONG n, i, lost;
    do {
        Disable();
        n = tr_rlen;
        if (n > 1024) n = 1024;
        for (i = 0; i < n; i++) tmp[i] = tr_ring[i];
        for (i = n; i < tr_rlen; i++) tr_ring[i - n] = tr_ring[i];
        tr_rlen -= n;
        lost = tr_lost;
        tr_lost = 0;
        Enable();
        tr_write(tmp, n);
        if (lost) {
            char l[48];
            struct tr_out o;
            o.p = l; o.end = l + sizeof(l) - 1;
            RawDoFmt("[prmtrace] %ld lines lost (ring full)\n", (APTR)&lost, (APTR)tr_putch, &o);
            tr_write(l, o.p - l);
        }
    } while (n == 1024);
}

void tr_logger(void)
{
    struct ExecBase *SysBase = *(struct ExecBase **)4L;
    struct MsgPort *port = CreateMsgPort();
    struct TrMsg *m;
    char tmp[1024];

    tr_task = FindTask(NULL);
    tr_port = port;
    Signal(tr_starter, 1L << tr_startsig);
    if (!port) return;
    tr_write("[prmtrace] logger started\n", 26);
    for (;;) {
        Wait((1L << port->mp_SigBit) | SIGBREAKF_CTRL_F);
        tr_flushring(tmp);
        while ((m = (struct TrMsg *)GetMsg(port)) != NULL) {
            tr_flushring(tmp);
            tr_write(m->tm_Text, tr_strlen(m->tm_Text));
            ReplyMsg(&m->tm_Msg);
        }
    }
}

/* the first process that may wait starts the logger */
static void tr_start(struct ExecBase *SysBase, struct Task *me)
{
    static struct TagItem tags[] = {
        {NP_Entry, 0},
        {NP_Name, 0},
        {NP_StackSize, 8192},
        {NP_Priority, 5},
        {TAG_END, 0}
    };
    Forbid();
    if (tr_starting) { Permit(); return; }
    tr_starting = 1;
    Permit();
    tags[0].ti_Data = (ULONG)tr_logger;
    tags[1].ti_Data = (ULONG)"prmtrace";
    if (!DOSBase) DOSBase = OpenLibrary("dos.library", 37);
    if (!DOSBase) return;
    tr_starter = me;
    tr_startsig = AllocSignal(-1);
    if (tr_startsig < 0) { tr_starting = 0; return; }
    if (CreateNewProc(tags)) Wait(1L << tr_startsig);
    FreeSignal(tr_startsig);
    tr_startsig = -1;
}

static void tr_ring_add(struct ExecBase *SysBase, const char *s, ULONG n)
{
    ULONG i;
    Disable();
    if (tr_rlen + n <= TR_RING) {
        for (i = 0; i < n; i++) tr_ring[tr_rlen + i] = s[i];
        tr_rlen += n;
    }
    else tr_lost++;
    Enable();
    if (tr_task) Signal(tr_task, SIGBREAKF_CTRL_F);
}

void kprintf(STRPTR format, ...)
{
    struct ExecBase *SysBase = *(struct ExecBase **)4L;
    struct Task *me = FindTask(NULL);
    ULONG sv = SetSR(0, 0) & 0x2000;
    BOOL canwait = !sv && SysBase->TDNestCnt < 0 && SysBase->IDNestCnt < 0;
    struct TrMsg m;
    struct tr_out o;
    struct { ULONG seq; char *name; char *where; } pre;
    va_list args;

    if (!format) return;
    pre.seq = ++tr_seq;
    pre.name = (me && me->tc_Node.ln_Name) ? me->tc_Node.ln_Name : "?";
    pre.where = sv ? " [int]" : (SysBase->IDNestCnt >= 0 ? " [disable]" : (SysBase->TDNestCnt >= 0 ? " [forbid]" : ""));
    o.p = m.tm_Text; o.end = m.tm_Text + TR_TEXT - 2;
    RawDoFmt("%ld %s%s: ", (APTR)&pre, (APTR)tr_putch, &o);
    va_start(args, format);
    RawDoFmt(format, (APTR)args, (APTR)tr_putch, &o);
    va_end(args);
    if (o.p > m.tm_Text && o.p[-1] != '\n') *o.p++ = '\n';
    *o.p = 0;

    if (canwait && !tr_port && me && me->tc_Node.ln_Type == NT_PROCESS && me != tr_task) tr_start(SysBase, me);
    if (canwait && tr_port && me != tr_task) {
        struct MsgPort *reply = CreateMsgPort();
        if (reply) {
            m.tm_Msg.mn_Node.ln_Type = NT_MESSAGE;
            m.tm_Msg.mn_ReplyPort = reply;
            m.tm_Msg.mn_Length = sizeof(m);
            PutMsg(tr_port, &m.tm_Msg);
            WaitPort(reply);
            GetMsg(reply);
            DeleteMsgPort(reply);
            return;
        }
    }
    tr_ring_add(SysBase, m.tm_Text, o.p - m.tm_Text);
}

/* ---- the public Prm_* calls, with their results --------------------------- */

static ULONG tr_tag(struct PrometheusBase *pb, PCIBoard *b, ULONG tag)
{
    struct Library *UtilityBase = pb->pb_UtilityBase;
    struct TagItem *t;
    if (!b) return 0;
    t = FindTagItem(tag, b->pn_TagList);
    return t ? t->ti_Data : 0;
}

PCIBoard *T_FindBoardTagList(__reg("a6") struct PrometheusBase *pb, __reg("a0") PCIBoard *node, __reg("a1") struct TagItem *taglist)
{
    PCIBoard *r = FindBoardTagList(pb, node, taglist);
    kprintf("Prm_FindBoardTagList(prev %08lx, tags %08lx) = %08lx (vendor %04lx device %04lx)\n",
        (ULONG)node, (ULONG)taglist, (ULONG)r, tr_tag(pb, r, PRM_Vendor), tr_tag(pb, r, PRM_Device));
    return r;
}
ULONG T_GetBoardAttrsTagList(__reg("a6") struct PrometheusBase *pb, __reg("a0") PCIBoard *board, __reg("a1") struct TagItem *taglist)
{
    ULONG r = GetBoardAttrsTagList(pb, board, taglist);
    kprintf("Prm_GetBoardAttrsTagList(board %08lx) = %ld\n", (ULONG)board, r);
    return r;
}
ULONG T_ReadConfigLong(__reg("a6") struct PrometheusBase *pb, __reg("a0") PCIBoard *board, __reg("d0") UBYTE offset)
{
    ULONG r = ReadConfigLong(pb, board, offset);
    kprintf("Prm_ReadConfigLong(board %08lx, %02lx) = %08lx\n", (ULONG)board, (ULONG)offset, r);
    return r;
}
UWORD T_ReadConfigWord(__reg("a6") struct PrometheusBase *pb, __reg("a0") PCIBoard *board, __reg("d0") UBYTE offset)
{
    UWORD r = ReadConfigWord(pb, board, offset);
    kprintf("Prm_ReadConfigWord(board %08lx, %02lx) = %04lx\n", (ULONG)board, (ULONG)offset, (ULONG)r);
    return r;
}
UBYTE T_ReadConfigByte(__reg("a6") struct PrometheusBase *pb, __reg("a0") PCIBoard *board, __reg("d0") UBYTE offset)
{
    UBYTE r = ReadConfigByte(pb, board, offset);
    kprintf("Prm_ReadConfigByte(board %08lx, %02lx) = %02lx\n", (ULONG)board, (ULONG)offset, (ULONG)r);
    return r;
}
void T_WriteConfigLong(__reg("a6") struct PrometheusBase *pb, __reg("a0") PCIBoard *board, __reg("d0") ULONG data, __reg("d1") UBYTE offset)
{
    kprintf("Prm_WriteConfigLong(board %08lx, %02lx, %08lx)\n", (ULONG)board, (ULONG)offset, data);
    WriteConfigLong(pb, board, data, offset);
}
void T_WriteConfigWord(__reg("a6") struct PrometheusBase *pb, __reg("a0") PCIBoard *board, __reg("d0") UWORD data, __reg("d1") UBYTE offset)
{
    kprintf("Prm_WriteConfigWord(board %08lx, %02lx, %04lx)\n", (ULONG)board, (ULONG)offset, (ULONG)data);
    WriteConfigWord(pb, board, data, offset);
}
void T_WriteConfigByte(__reg("a6") struct PrometheusBase *pb, __reg("a0") PCIBoard *board, __reg("d0") UBYTE data, __reg("d1") UBYTE offset)
{
    kprintf("Prm_WriteConfigByte(board %08lx, %02lx, %02lx)\n", (ULONG)board, (ULONG)offset, (ULONG)data);
    WriteConfigByte(pb, board, data, offset);
}
LONG T_SetBoardAttrsTagList(__reg("a6") struct PrometheusBase *pb, __reg("a0") PCIBoard *board, __reg("a1") struct TagItem *taglist)
{
    LONG r;
    kprintf("Prm_SetBoardAttrsTagList(board %08lx, tags %08lx) ...\n", (ULONG)board, (ULONG)taglist);
    r = SetBoardAttrsTagList(pb, board, taglist);
    kprintf("Prm_SetBoardAttrsTagList = %ld\n", r);
    return r;
}
BOOL T_AddIntServer_(__reg("a6") struct PrometheusBase *pb, __reg("a0") PCIBoard *board, __reg("a1") struct Interrupt *intr)
{
    BOOL r;
    kprintf("Prm_AddIntServer(board %08lx, int %08lx code %08lx) ...\n", (ULONG)board, (ULONG)intr, intr ? (ULONG)intr->is_Code : 0);
    r = AddIntServer_(pb, board, intr);
    kprintf("Prm_AddIntServer = %ld\n", (LONG)r);
    return r;
}
void T_RemIntServer_(__reg("a6") struct PrometheusBase *pb, __reg("a0") PCIBoard *board, __reg("a1") struct Interrupt *intr)
{
    kprintf("Prm_RemIntServer(board %08lx, int %08lx)\n", (ULONG)board, (ULONG)intr);
    RemIntServer_(pb, board, intr);
}
APTR T_AllocDMABuffer(__reg("a6") struct PrometheusBase *pb, __reg("d0") ULONG size)
{
    APTR r;
    kprintf("Prm_AllocDMABuffer(%08lx) ...\n", size);
    r = AllocDMABuffer(pb, size);
    kprintf("Prm_AllocDMABuffer = %08lx (card library %08lx)\n", (ULONG)r, (ULONG)pb->pb_DMASuppBase);
    return r;
}
void T_FreeDMABuffer(__reg("a6") struct PrometheusBase *pb, __reg("a0") APTR buffer, __reg("d0") ULONG size)
{
    kprintf("Prm_FreeDMABuffer(%08lx, %08lx) ...\n", (ULONG)buffer, size);
    FreeDMABuffer(pb, buffer, size);
    kprintf("Prm_FreeDMABuffer done\n");
}
APTR T_GetPhysicalAddress(__reg("a6") struct PrometheusBase *pb, __reg("d0") APTR addr)
{
    APTR r = GetPhysicalAddress(pb, addr);
    kprintf("Prm_GetPhysicalAddress(%08lx) = %08lx\n", (ULONG)addr, (ULONG)r);
    return r;
}
APTR T_GetVirtualAddress(__reg("a6") struct PrometheusBase *pb, __reg("d0") APTR addr)
{
    APTR r = GetVirtualAddress(pb, addr);
    kprintf("Prm_GetVirtualAddress(%08lx) = %08lx\n", (ULONG)addr, (ULONG)r);
    return r;
}
APTR T_AllocPCIAddressSpace(__reg("a6") struct PrometheusBase *pb, __reg("a0") PCIBoard *board, __reg("d0") ULONG size, __reg("d1") ULONG bar)
{
    APTR r = AllocPCIAddressSpace(pb, board, size, bar);
    kprintf("Prm_AllocPCIAddressSpace(board %08lx, size %08lx, bar %ld) = %08lx\n", (ULONG)board, size, bar, (ULONG)r);
    return r;
}
void T_FreePCIAddressSpace(__reg("a6") struct PrometheusBase *pb, __reg("a0") PCIBoard *board, __reg("d0") ULONG bar)
{
    kprintf("Prm_FreePCIAddressSpace(board %08lx, bar %ld)\n", (ULONG)board, bar);
    FreePCIAddressSpace(pb, board, bar);
}
