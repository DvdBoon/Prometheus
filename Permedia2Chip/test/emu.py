import struct, hunk, protos, bioffs
import machine68k as M
P=protos.parse(); OFF=bioffs.OFF
REGN=['d%d'%i for i in range(8)]+['a%d'%i for i in range(8)]
RID={n:getattr(M.Register,n.upper()) for n in REGN}
HALT=0x400; EXEC=0x8000; DEFSTUB=0xA000; BI=0x10000; STRUCTS=0x20000; STACK=0x7F000
CODE=0x100000; DATA=0x140000; VRAMSIZE=0x810000
LVO={0x240:'AttemptSemaphore',0x252:'FindSemaphore',0x258:'AddSemaphore',0xc6:'AllocMem',0x78:'Disable',0x7e:'Enable',0x27c:'CacheClearU',0xd2:'FreeMem',0xfc:'Remove',0xb4:'Cause',0x84:'Forbid',0x8a:'Permit',0x228:'OpenLibrary',0x19e:'CloseLibrary',0x114:'FindName',0x234:'ObtainSemaphore',0x23a:'ReleaseSemaphore',0x6c:'LVO_6c',0x126:'FindTask'}
RETS={'FindSemaphore':0,'AllocMem':0x20800,'AttemptSemaphore':1,'FindTask':0x7777}
DEFAULTS=[n for n in P if n.endswith('Default')]
class Chip:
    def __init__(self): self.shadow={}; self.idx=0; self.dac={}; self.line=0; self.log=[]
    def rd(self,r,size):
        if r==0x18: v=0x100
        elif r in (0x0,0x68): v=0
        elif r==0x3070: self.line+=1; v=self.line%1100
        elif r==0x4050: v=0x10 if self.idx in (0x29,0x33) else self.dac.get(self.idx,0)
        else: v=self.shadow.get(r,0)
        self.log.append(('R',r,v,size)); return v
    def wr(self,r,v,size):
        self.shadow[r]=v
        if r==0x4000: self.idx=v&0xff
        if r==0x4050: self.dac[self.idx]=v
        self.log.append(('W',r,v,size))
class Emu:
    def __init__(self,img,cpu=M.CPUType.M68030,nops=True):
        mc=M.Machine(cpu,16*1024); self.cloud='flat' in img; self.mc=mc; self.mem=mem=mc.mem; self.cpu=mc.cpu; tr=mc.traps
        self.chip=Chip()
        self.MMIO=mem.reserve_special_range(2); self.REGBASE=self.MMIO+0x8000
        c=self.chip; B=self.MMIO
        mem.set_special_range_read_funcs(B,2,lambda a:c.rd(a-B,1)&0xff,lambda a:c.rd(a-B,2)&0xffff,lambda a:c.rd(a-B,4))
        mem.set_special_range_write_funcs(B,2,lambda a,v:c.wr(a-B,v,1),lambda a,v:c.wr(a-B,v,2),lambda a,v:c.wr(a-B,v,4))
        self.VRAM=mem.reserve_special_range(VRAMSIZE>>16); V=self.VRAM; self.vram=vr=bytearray(VRAMSIZE)
        def vrd(a,n): o=a-V; return int.from_bytes(vr[o:o+n],'big')
        def vwr(a,v,n): o=a-V; vr[o:o+n]=v.to_bytes(n,'big'); c.log.append(('VRAM',o,v,n))
        mem.set_special_range_read_funcs(V,VRAMSIZE>>16,lambda a:vrd(a,1),lambda a:vrd(a,2),lambda a:vrd(a,4))
        mem.set_special_range_write_funcs(V,VRAMSIZE>>16,lambda a,v:vwr(a,v,1),lambda a,v:vwr(a,v,2),lambda a,v:vwr(a,v,4))
        self.halt_tid=tr.alloc(lambda op,pc: mc.create_execute_end('halt'))
        mem.w16(HALT,0xA000|self.halt_tid)
        for o in range(6,0x300,6):
            tid=tr.alloc(self._mk_lvo(o)); mem.w16(EXEC-o,0xA000|tid); mem.w16(EXEC-o+2,0x4e75)
        mem.w16(EXEC+0x128,0x02|0x04)
        for i,n in enumerate(DEFAULTS):
            tid=tr.alloc(self._mk_def(n)); mem.w16(DEFSTUB+4*i,0xA000|tid); mem.w16(DEFSTUB+4*i+2,0x4e75)
        if 'hunks' in img:
            h=img['hunks']; bases=[CODE,DATA]
            for i,x in enumerate(h): mem.w_block(bases[i],x['data'])
            for i,x in enumerate(h):
                for th,offs in x['relocs'].items():
                    for o in offs: a=bases[i]+o; mem.w32(a,(mem.r32(a)+bases[th])&0xffffffff)
            self.sym={k:CODE+v for k,v in img['syms'].items()}
        else:
            mem.w_block(CODE,img['flat']); self.sym=img['syms']
        if nops:
            m=self.mem; lg=self.chip
            def ih(*a):
                if m.r16(self.cpu.r_pc())==0x4e71: lg.log.append(('NOP',))
            self.cpu.set_instr_hook_callback(ih)
    def fn(self,name):
        if self.cloud and name+'_stub' in self.sym: return self.sym[name+'_stub']
        return self.sym[name]
    def reg(self,n): return self.cpu.r_reg(RID[n])
    def _mk_lvo(self,o):
        def f(op,pc):
            n=LVO.get(o,hex(o)); a6=self.reg('a6')
            AR={'AttemptSemaphore':('a0',),'FindSemaphore':(),'AddSemaphore':('a1',),'AllocMem':('d0','d1'),'FreeMem':('a1','d0'),'Remove':('a1',),'Cause':('a1',),'ObtainSemaphore':('a0',),'ReleaseSemaphore':('a0',),'LVO_6c':('d7',),'FindTask':('a1',)}
            extra=tuple(self.reg(r) for r in AR[n]) if n in AR else None
            self.chip.log.append(('CALL','exec' if a6==EXEC else hex(a6),n,extra))
            if n in RETS: self.cpu.w_reg(RID['d0'],RETS[n]&0xffffffff)
        return f
    def _mk_def(self,n):
        def f(op,pc):
            ret,args=P[n]; vals=[]; sp=self.reg('a7')
            for i,(r,t) in enumerate(args):
                v=self.mem.r32(sp+4+4*i) if self.cloud else self.reg(r)
                if 'WORD' in t or 'SHORT' in t or t=='BOOL': v&=0xffff
                elif 'UBYTE' in t: v&=0xff
                vals.append(v)
            self.chip.log.append(('CALL',n,tuple(vals)))
        return f
    def w32(self,a,v): self.mem.w32(a,v&0xffffffff)
    def w16(self,a,v): self.mem.w16(a,v&0xffff)
    def w8(self,a,v): self.mem.w8(a,v&0xff)
    def r32(self,a): return self.mem.r32(a)
    def setup_bi(self,memsize=0x800000):
        self.mem.w_block(BI,bytes(0x700))
        self.w32(BI+0,self.REGBASE); self.w32(BI+4,self.VRAM); self.w32(BI+8,self.REGBASE); self.w32(BI+0xc,memsize)
        self.w32(BI+0x3c,EXEC)
        for i,n in enumerate(DEFAULTS): self.w32(BI+OFF[n],DEFSTUB+4*i)
    def call(self,addr,regs={},stackargs=(),limit=50_000_000):
        cpu=self.cpu; init={}
        for i,r in enumerate(REGN):
            if r!='a7': v=regs.get(r,0x5a5a0000|i); init[r]=v&0xffffffff; cpu.w_reg(RID[r],init[r])
        sp=STACK
        for v in reversed(list(stackargs)): sp-=4; self.w32(sp,v)
        sp-=4; self.w32(sp,HALT); cpu.w_reg(RID['a7'],sp); cpu.w_pc(addr)
        self.chip.log=[]
        res=self.mc.execute(limit)
        if not (res.result and getattr(res.result,'desc','')=='halt'): raise Exception('no return: %r pc=%x'%(res,cpu.r_pc()))
        log=self.chip.log
        if self.reg('a7')!=sp+4: log.append(('SPBAD',))
        bad=tuple(r for r in ('d2','d3','d4','d5','d6','d7','a2','a3','a4','a5','a6') if self.reg(r)!=init[r])
        if bad: log.append(('CLOBBER',bad))
        return self.reg('d0'),log
def load_hunk(path,syms):
    s,h=hunk.parse(path); return {'sizes':s,'hunks':h,'syms':syms}
