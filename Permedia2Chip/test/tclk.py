import sys, build, tchip
from emu import *
MI=STRUCTS
def vectors():
    V=[]
    for idx in (0,1,100,668):
        V.append(('GetPixelClock',{'a0':BI,'a1':MI,'d0':idx,'d7':1},{}))
    for fmt in range(0,23):
        for clk in (0,1,13000000,13750000,13760000,25175000,31500000,65000000,108000000,135000000,157500000,175000000,200000000,250000000,0xffffffff):
            V.append(('ResolvePixelClock',{'a0':BI,'a1':MI,'d0':clk,'d7':fmt},{}))
    for n,d in ((0x68,0x1b),(0,0),(0xff,0xff),(0x12,0x37)):
        V.append(('SetClock',{'a0':BI},{'pll':(n,d)}))
    return V
def prep(r,opt):
    e=r.e; e.mem.w_block(MI,bytes(0x40)); e.w32(BI+0x22e,MI)
    if 'pll' in opt: e.w8(MI+0x2a,opt['pll'][0]); e.w8(MI+0x2b,opt['pll'][1])
if __name__=='__main__':
    img=build.build(sys.argv[1:] or ['src/chip.c','src/blitter.c','src/clocks.c','src/tables.c','src/tmpstubs.c','src/cloud_entry.s'])
    tchip.run_all(img,vectors,prep)
