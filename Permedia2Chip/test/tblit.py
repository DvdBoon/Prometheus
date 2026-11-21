import sys, build, tchip, itertools, random
from emu import *
RI=STRUCTS+0x100; RI2=STRUCTS+0x120; PAT=STRUCTS+0x200; PATM=STRUCTS+0x240; TPL=STRUCTS+0x300; LINE=STRUCTS+0x340
BM=STRUCTS+0x380; DATA=STRUCTS+0x400
PW={0:1,1:1,2:3,3:3,4:2,5:2,6:4,7:4,8:4,9:4,10:2,11:2,12:2,13:2,14:2,15:1,16:1,17:2,18:2,19:2,20:2,21:2}
def vectors():
    V=[]; rnd=random.Random(1)
    V.append(('WaitBlitter',{'a0':BI},{}))
    fmts=[1,2,4,5,6,7,9,10,11,13,14,18]
    widths=[640,1024,1152,1920,100,0]
    for fmt in fmts:
        for wd in widths:
            bpr=wd*PW[fmt]
            for mask in (0xff,0x0f):
                V.append(('FillRect',{'a0':BI,'a1':RI,'d0':10,'d1':20,'d2':100,'d3':50,'d4':0x12345678,'d5':mask|0x1200,'d7':fmt},{'ri':(0x2000,bpr)}))
                V.append(('InvertRect',{'a0':BI,'a1':RI,'d0':0xfffe,'d1':5,'d2':7,'d3':3,'d4':mask,'d7':fmt},{'ri':(0x3000,bpr)}))
                for sx,sy,dx,dy in ((10,20,30,40),(30,40,10,20),(5,3,6,2),(0xffff,2,3,0xfffe)):
                    V.append(('BlitRect',{'a0':BI,'a1':RI,'d0':sx,'d1':sy,'d2':dx,'d3':dy,'d4':33,'d5':17,'d6':mask,'d7':fmt},{'ri':(0x4000,bpr),'flags':0}))
                V.append(('BlitRect',{'a0':BI,'a1':RI,'d0':1,'d1':2,'d2':5,'d3':1,'d4':33,'d5':17,'d6':mask,'d7':fmt},{'ri':(0x4000,bpr),'biflags':1<<22}))
    for fmt in (1,4,6,2):
        for wd in (640,1024,100,1920):
            bpr=wd*PW[fmt]
            for mt in (0,3,5,0xc,0xf,0x6):
                for same in (0,1,2):
                    for (sx,sy,dx,dy,h) in ((10,20,30,40,3),(30,40,10,20,2),(5,3,6,2,1),(1,1,2,2,0)):
                        ri2=(0x4000,bpr) if same==1 else ((0x8000,bpr) if same==0 else (0x4000,bpr+4*PW[fmt]))
                        V.append(('BlitRectNoMaskComplete',{'a0':BI,'a1':RI,'a2':RI2,'d0':sx,'d1':sy,'d2':dx,'d3':dy,'d4':25,'d5':h,'d6':mt,'d7':fmt},{'ri':(0x4000,bpr),'ri2':ri2}))
    for fmt in (0,1,4,6,2,10):
        bpr=640*PW.get(fmt,1)
        for dm in range(8):
            for pat in (0xffff,0xf0f0,0x0000):
                for hor,dxv,dyv in ((1,20,5),(1,-20,5),(1,0,0),(0,3,30),(0,-3,-30),(0,0,0),(1,-7,-2)):
                    for mask in (0xff,0x3):
                        V.append(('DrawLine',{'a0':BI,'a1':RI,'a2':LINE,'d0':mask,'d7':fmt},{'ri':(0x1000,bpr),'line':(100,50,abs(dxv) if hor else abs(dyv),dxv,dyv,pat,0x11223344,0x55667788,hor,dm,90,40)}))
    for fmt in (1,4,6,2,0):
        bpr=640*PW.get(fmt,1)
        for size in (0,1,2,3,4):
            for good in (1,0):
                for dm in range(8):
                    for mask in (0xff,0x7):
                        V.append(('BlitPattern',{'a0':BI,'a1':RI,'a2':PAT,'d0':13,'d1':7,'d2':50,'d3':20,'d4':mask,'d7':fmt},{'ri':(0x1000,bpr),'pat':(size,good,3,5,0x12345678,0x9abcdef0,dm)}))
    for fmt in (1,4,6,2):
        bpr=640*PW[fmt]
        for xo in (0,5,15):
            for w,h in ((10,3),(40,2),(1,1),(70,0)):
                for dm in range(8):
                    for mask in (0xff,0x1):
                        V.append(('BlitTemplate',{'a0':BI,'a1':RI,'a2':TPL,'d0':11,'d1':9,'d2':w,'d3':h,'d4':mask,'d7':fmt},{'ri':(0x1000,bpr),'tpl':(xo,dm,0x0f0f0f0f,0xf0f0f0f0)}))
    for wd in (640,100,1920):
        for depth in (1,4,8):
            for mask in (0xff,0x5,0x80):
                for mt in (0xc,0x3,0x1c):
                    for (sx,sy,dx,dy,w,h) in ((3,1,10,20,40,2),(32,0,5,5,33,1),(0,0,0,0,1,1),(31,2,7,3,2,0)):
                        V.append(('BlitPlanar2Chunky',{'a0':BI,'a1':BM,'a2':RI,'d0':sx,'d1':sy,'d2':dx,'d3':dy,'d4':w,'d5':h,'d6':mt,'d7':mask},{'ri':(0x1000,wd),'bm':(depth,)}))
    return V
def prep(r,opt):
    e=r.e
    if 'ri' in opt: e.w32(RI,e.VRAM+opt['ri'][0]); e.w16(RI+4,opt['ri'][1])
    if 'ri2' in opt: e.w32(RI2,e.VRAM+opt['ri2'][0]); e.w16(RI2+4,opt['ri2'][1])
    if 'biflags' in opt: e.w32(BI+0xba,e.r32(BI+0xba)|opt['biflags'])
    if 'line' in opt:
        X,Y,L,dx,dy,pat,fg,bg,hor,dm,xo,yo=opt['line']
        e.w16(LINE,X); e.w16(LINE+2,Y); e.w16(LINE+4,L); e.w16(LINE+6,dx); e.w16(LINE+8,dy); e.w16(LINE+0x10,pat)
        e.w32(LINE+0x14,fg); e.w32(LINE+0x18,bg); e.w16(LINE+0x1c,hor); e.w8(LINE+0x1e,dm); e.w16(LINE+0x20,xo); e.w16(LINE+0x22,yo)
    if 'pat' in opt:
        size,good,xo,yo,fg,bg,dm=opt['pat']
        e.w32(PAT,PATM); e.w16(PAT+4,xo); e.w16(PAT+6,yo); e.w32(PAT+8,fg); e.w32(PAT+0xc,bg); e.w8(PAT+0x10,size); e.w8(PAT+0x11,dm)
        for i in range(16):
            b=(i*37+11)&0xff; e.w8(PATM+2*i,b); e.w8(PATM+2*i+1,b if (good or i!=(1<<min(size,3))-1) else b^1)
    if 'tpl' in opt:
        xo,dm,fg,bg=opt['tpl']
        e.w32(TPL,DATA); e.w16(TPL+4,16); e.w8(TPL+6,xo); e.w8(TPL+7,dm); e.w32(TPL+8,fg); e.w32(TPL+0xc,bg)
        for i in range(0x100): e.w8(DATA+i,(i*29+5)&0xff)
    if 'bm' in opt:
        depth,=opt['bm']
        e.w16(BM,16); e.w16(BM+2,8); e.w8(BM+5,depth)
        for p in range(8):
            v=[DATA+0x80*p,0,0xffffffff,DATA+0x80*p][p%4]
            e.w32(BM+8+4*p,v)
        for i in range(0x400): e.w8(DATA+i,(i*13+7)&0xff)
if __name__=='__main__':
    img=build.build(sys.argv[1:] or ['src/chip.c','src/blitter.c','src/tables.c','src/tmpstubs.c','src/cloud_entry.s'])
    tchip.run_all(img,vectors,prep)
