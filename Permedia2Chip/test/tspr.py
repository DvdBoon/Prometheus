import sys, build, tchip
from emu import *
MI=STRUCTS; IMG=STRUCTS+0x100
def vectors():
    V=[]
    for st in (0,1,0x10000,0xffff):
        V.append(('SetSprite',{'a0':BI,'d0':st,'d7':1},{}))
    for act in (0,1):
        for m in (None,0,2,4,6):
            for x,y in ((0,0),(-63,-63),(-64,-64),(100,50),(2000,1500),(-1000,5)):
                V.append(('SetSpritePosition',{'a0':BI,'d0':x&0xffff,'d1':y&0xffff,'d7':1},{'act':act,'smode':m,'off':(3,7)}))
    for fl in (0,1<<16,1<<17,3<<16):
        for hgt in (0,1,16,31,32,33,63,64,65,128,255):
            V.append(('SetSpriteImage',{'a0':BI,'d7':1},{'sflags':fl,'img':hgt}))
    for i in (0,1,2,255):
        V.append(('SetSpriteColor',{'a0':BI,'d0':i,'d1':0x12,'d2':0x34,'d3':0x56,'d7':1},{}))
    for ms in (0x800000,0x400000,0x100000,0):
        for clk in (0,1,40000000,50000000,66000000,83000000,100000000,120000000):
            for fl in (0,0x8000):
                V.append(('InitChip',{'a0':BI},{'memsize':ms,'memclk':clk,'biflags':fl}))
    return V
def prep(r,opt):
    e=r.e
    if 'act' in opt: e.w32(BI+0x58e,opt['act'])
    if 'off' in opt: e.w16(BI+0x236,opt['off'][0]); e.w16(BI+0x238,opt['off'][1])
    if 'smode' in opt:
        if opt['smode'] is None: e.w32(BI+0x22e,0)
        else: e.mem.w_block(MI,bytes(0x40)); e.w8(MI+0x17,opt['smode']); e.w32(BI+0x22e,MI)
    if 'sflags' in opt: e.w32(BI+0xba,(e.r32(BI+0xba)&~(3<<16))|opt['sflags'])
    if 'img' in opt:
        e.w8(BI+0x56b,opt['img']); e.w32(BI+0x56e,IMG)
        for i in range(0xe00): e.w8(IMG+i,(i*71+13)&0xff)
    if 'memsize' in opt:
        e.mem.w_block(BI,bytes(0x700)); e.setup_bi(opt['memsize']); e.w32(BI+0xfa,opt['memclk']); e.w32(BI+0xba,opt['biflags'])
if __name__=='__main__':
    img=build.build(sys.argv[1:] or ['src/chip.c','src/blitter.c','src/clocks.c','src/sprite.c','src/tables.c','src/cloud_entry.s'])
    tchip.run_all(img,vectors,prep)
