import emu, map717, build, struct, sys
from emu import *
S17={v:k for k,v in map717.M.items()}
ALIAS={'SetMemoryMode_etc':'NoOp'}
def load17(): return Emu(load_hunk('bin/p2_717.chip',S17))
def symname(e,v):
    for n,a in e.sym.items():
        if a==v and not n.endswith('_stub'): return ALIAS.get(n,n)
    return None
def snapshot(e,regions):
    out={}
    for base,size in regions:
        b=e.mem.r_block(base,size)
        for i in range(0,size,2): out[base+i]=b[i:i+2]
    return out
def norm_log(e,log,reads=False):
    r=[]
    for x in log:
        if x[0]=='R' and not reads: continue
        r.append(x)
    return r
def bidiff(e,before,after):
    # word-granular diff; pointer-valued longs normalised to names
    d={}
    for a in after:
        if before[a]!=after[a]: d[a]=after[a]
    out={}
    for a in sorted(d):
        out[a-BI if a<BI+0x700 else a]=d[a].hex()
    # normalise longs that hold code addresses
    res={}
    keys=sorted(out)
    for a in keys:
        res[a]=out[a]
    return res
def norm_ptrs(e,base,size):
    res={}
    for o in range(0,size,2):
        v=e.mem.r32(base+o) if o+4<=size else None
        n=symname(e,v) if v and (CODE<=v<CODE+0x40000) else None
        if n: res[o]=n
    return res
class Run:
    def __init__(self,img=None):
        self.e=load17() if img is None else Emu(img)
        self.e.setup_bi()
    def init(self):
        e=self.e; d0,log=e.call(e.fn('InitChip'),{'a0':BI}); return d0,log
    def call(self,name,regs,regions=((BI,0x700),(STRUCTS,0x1000))):
        e=self.e; b=snapshot(e,regions)
        d0,log=e.call(e.fn(name),regs)
        a=snapshot(e,regions)
        mem={k:v for k,v in a.items() if b[k]!=v}
        return d0,log,mem
