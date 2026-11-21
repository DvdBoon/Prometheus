import sys, build, cmp, protos
from emu import *
P=protos.parse()
MI=STRUCTS
def mode(e,w,h,depth,flags,ht,hss,hssz,vt,vss,vssz):
    e.mem.w_block(MI,bytes(0x40))
    e.w16(MI+0x12,w); e.w16(MI+0x14,h); e.w8(MI+0x16,depth); e.w8(MI+0x17,flags)
    e.w16(MI+0x18,ht); e.w16(MI+0x1c,hss); e.w16(MI+0x1e,hssz); e.w16(MI+0x22,vt); e.w16(MI+0x26,vss); e.w16(MI+0x28,vssz)
RETMASK={'void':0,'BOOL':0xffff,'UWORD':0xffff,'WORD':0xffff}
def vectors():
    V=[]
    for lvl in range(6):
        for vc in (None,0xffffffff,0x12345678):
            V.append(('SetDPMSLevel',{'a0':BI,'d0':lvl},{'chip':{0x3058:vc} if vc is not None else {}}))
    for fl in (0,0x10):
        for st in (0,1,0x10000,0xffff):
            V.append(('SetInterrupt',{'a0':BI,'d0':st},{'flags':fl,'chip':{0x8:0xff}}))
    for f in (0,0x10,0x11):
        V.append(('HardInterruptCode',{'a1':BI},{'chip':{0x10:f,0x8:0xffff}}))
    for fmt in (1,2):
        for st,cn in ((0,256),(10,5),(250,6),(0xfff0,0x20),(5,0)):
            V.append(('SetColorArray',{'a0':BI,'d0':st|0xabcd0000,'d1':cn},{'rgbformat':fmt,'clut':True}))
    modes=[(640,480,8,0,800,656,96,525,490,2),(800,600,16,0,1056,840,128,628,601,4),(1024,768,15,4,1344,1048,136,806,771,6),
           (1280,1024,24,2,1688,1328,112,1066,1025,3),(1600,1200,32,6,2160,1664,192,1250,1201,3),(320,240,8,4,400,328,48,262,245,2)]
    for m in modes:
        for vc in (0x229,0x201,0x0,0xffffffff):
            for fl in (0,0x8,0x10,0x18):
                V.append(('SetGC',{'a0':BI,'a1':MI,'d0':1},{'mode':m,'chip':{0x3058:vc},'flags':fl}))
    for w in (0,1,31,32,33,320,640,800,1024,1152,1280,1600,1920,2047,2048,2049,4000):
        for fmt in list(range(0,22))+[0x80000000]:
            V.append(('CalculateBytesPerRow',{'a0':BI,'d0':w|0x55550000,'d7':fmt},{}))
    for addr in (VRAM_-4,0,0x10,0x7ffffc,0x800000,0x900000):
        for fmt in (0,1,9,10,11,12,31,32,42):
            V.append(('CalculateMemory',{'a0':BI,'a1':(VRAM_+addr)&0xffffffff if addr!=VRAM_-4 else VRAM_-4,'d7':fmt},{}))
    for fmt in (0,1,2,5,10,11,20,21,22):
        V.append(('SetDAC',{'a0':BI,'d7':fmt&0xffffffff},{'clut':True}))
    for fmt in range(-1,24):
        V.append(('SetMemoryMode',{'a0':BI,'d7':fmt&0xffffffff,'d0':0x11111111,'d1':0x22222222,'a1':0x33333333},{}))
    V.append(('GetCompatibleFormats',{'a0':BI,'d7':1},{}))
    V.append(('SetDisplay',{'a0':BI,'d0':0},{}))
    for name in ('GetVSyncState','WaitVerticalSync'):
        for m in (None,modes[0],modes[5]):
            for ex in (0,1):
                V.append((name,{'a0':BI,'d0':ex},{'mode':m,'nomode':m is None}))
    for (w,x,y,fmt,fl) in ((640,0,0,1,0),(800,16,8,10,0),(1024,3,5,9,2),(1280,-1,-2,6,0),(640,100,100,25,0),(2049,0,0,1,0),(640,7,3,22,2)):
        V.append(('SetPanning',{'a0':BI,'a1':VRAM_+0x1000,'d0':w,'d1':x&0xffff,'d2':y&0xffff,'d7':fmt},{'mode':(640,480,8,fl,800,656,96,525,490,2)}))
    return V
VRAM_=None
PREP=[]
def prep(r,opt):
    e=r.e
    if 'flags' in opt: e.w32(BI+0xba,(e.r32(BI+0xba)&~0x18)|opt['flags'])
    if 'rgbformat' in opt: e.w32(BI+0x232,opt['rgbformat'])
    if opt.get('clut'):
        for i in range(256*3): e.w8(BI+0x242+i,(i*7+3)&0xff)
    if opt.get('mode'):
        mode(e,*opt['mode']); e.w32(BI+0x22e,MI)
    if opt.get('nomode'): e.w32(BI+0x22e,0)
    for k,v in opt.get('chip',{}).items(): e.chip.shadow[k]=v
import bioffs
FPOFFS=sorted(set(bioffs.OFF.values())|{0x56})
def fptrs(e):
    res={}
    for o in FPOFFS:
        v=e.r32(BI+o)
        if CODE<=v<CODE+0x40000: res[hex(o)]=cmp.symname(e,v) or hex(v-CODE)
    return res
def side(r,V):
    a=r.init()
    st=(dict(r.e.chip.shadow),r.e.mem.r_block(BI,0x700))
    out=[('InitChip',a[0],a[1],fptrs(r.e),r.e.mem.r_block(BI,0x700))]
    for name,regs,opt in V:
        r.e.chip.shadow=dict(st[0]); r.e.mem.w_block(BI,st[1]); r.e.mem.w_block(STRUCTS,bytes(0x1000))
        prep(r,opt)
        for f in PREP: f(r,opt)
        import os as _os
        if _os.environ.get('DIRTY'): r.e.w32(0x20800+46,1)
        d0,log,mem=r.call(name,regs)
        if _os.environ.get('DIRTY'):
            DIRTYSTATS[r.e.r32(0x20800+46)]=DIRTYSTATS.get(r.e.r32(0x20800+46),0)+1
        ret=P[name][0] if name in P else 'ULONG'
        m=RETMASK.get(ret,0xffffffff)
        extra=()
        if name=='SetMemoryMode': extra=tuple(r.e.reg(x) for x in ('d0','d1','a0','a1'))
        fp=fptrs(r.e)
        skip=set()
        for o in FPOFFS: skip|={BI+o,BI+o+2}
        mem={k:v for k,v in mem.items() if k not in skip}
        import os
        drop=('AttemptSemaphore','ReleaseSemaphore','FindTask') if os.environ.get('NOSEM') else ()
        if os.environ.get('RELOAD'):
            log=strip_reload(log)
        out.append((d0&m,[x for x in log if x[0]!='R' and not (x[0]=='CALL' and x[2] in drop)],mem,extra,fp))
    r.e.mc.cleanup()
    return out
RELOADREGS=[(0x80a0,0),(0x8180,0),(0x81a0,0),(0x81c8,0),(0x8380,0),(0x8590,0),(0x8670,0),(0x8678,0),(0x8680,0),(0x8690,0),(0x87e0,0),(0x8810,0),(0x8818,0),(0x8820,0xffffffff),(0x8828,0),(0x8880,0),(0x8888,0),
 (0x8890,0),(0x88b8,0),(0x88c0,0),(0x88c8,0),(0x8980,0x40000),(0x8988,0),(0x89a0,0),(0x8a80,0),(0x8a88,0),(0x8a90,0),(0x8ab0,0),(0x8ab8,1),(0x8ac0,0xffffffff),(0x8c00,0),(0x8c08,0),(0x8f00,0),(0x9300,0)]
RELOADSTATS={'ok':0,'nolock':0}
DIRTYSTATS={}
def strip_reload(log):
    out=[];i=0
    while i<len(log):
        x=log[i]
        if x[0]=='CALL' and x[2]=='FindTask': i+=1; continue
        if x[0]=='CALL' and x[2]=='AttemptSemaphore':
            i+=1; got=[]
            while len(got)<len(RELOADREGS):
                y=log[i]; i+=1
                if y[0]=='W': got.append((y[1],y[2]))
                elif y[0] in ('NOP','R'): pass
                else: raise Exception('reload interrupted %r'%(y,))
            assert got==RELOADREGS,(got)
            RELOADSTATS['ok']+=1
            continue
        if x[0]=='CALL' and x[2]=='ReleaseSemaphore': i+=1; continue
        out.append(x); i+=1
    return out
def report(V,A,B,maxshow=8):
    import difflib
    bad=0
    for (name,regs,opt),x,y in zip(V,A,B):
        if x==y: continue
        bad+=1
        if bad>maxshow: continue
        print('MISMATCH',name,{k:hex(v) for k,v in regs.items()},opt)
        for i,lab in enumerate(('ret','log','mem','regs','fptrs')):
            if x[i]!=y[i]:
                if lab=='log':
                    for l in list(difflib.unified_diff([str(t) for t in x[i]],[str(t) for t in y[i]],lineterm='',n=1))[2:30]: print('   ',l)
                elif lab=='mem':
                    ks=sorted(set(x[i])|set(y[i]))
                    print('   mem', [(hex(k),x[i].get(k,b'').hex(),y[i].get(k,b'').hex()) for k in ks if x[i].get(k)!=y[i].get(k)][:12])
                else: print('  ',lab,x[i],y[i])
    from collections import Counter
    c=Counter(n for (n,_,_),x,y in zip(V,A,B) if x!=y)
    print('vectors',len(V),'mismatch',bad, dict(c)); print('reloads',RELOADSTATS,'flag after',DIRTYSTATS)
def run_all(img,vectors=vectors,prep=None):
    global VRAM_, PREP
    if prep: PREP.append(prep)
    r=cmp.Run(); VRAM_=r.e.VRAM; V=vectors(); A=side(r,V)
    r=cmp.Run(img); assert VRAM_==r.e.VRAM; B=side(r,V)
    print('InitChip d0',A[0][1],B[0][1])
    la=[x for x in A[0][2] if x[0]!='R']; lb=[x for x in B[0][2] if x[0]!='R']
    if la!=lb:
        import difflib
        print('InitChip trace differs:')
        for l in list(difflib.unified_diff([str(t) for t in la],[str(t) for t in lb],lineterm='',n=1))[2:40]: print('   ',l)
    else: print('InitChip trace identical (%d events)'%len(la))
    print('InitChip fptrs', 'same' if A[0][3]==B[0][3] else (A[0][3],B[0][3]))
    skip=set()
    for o in FPOFFS: skip|=set(range(o,o+4))
    d=[hex(i) for i in range(0x700) if i not in skip and A[0][4][i]!=B[0][4][i]]
    print('InitChip bi bytes differing (excluding function pointers):', d[:20])
    report(V,A[1:],B[1:])
    return V,A,B
if __name__=="__main__":
    img=build.build(sys.argv[1:] or ["src/chip.c","src/blitter.c","src/tables.c","src/tmpstubs.c","src/cloud_entry.s"])
    run_all(img)
