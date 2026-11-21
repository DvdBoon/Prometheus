import sys, build, tchip
from emu import *
LB=STRUCTS+0x200
def vectors():
    V=[]
    for af in (0x0,0x2,0x6):
        V.append(('LibInit',{'d0':LB,'a0':0x1234,'a6':EXEC},{'attn':af}))
    for cnt in (0,1,2):
        for fl in (0,8,0xff):
            V.append(('LibOpen',{'a6':LB},{'lib':(cnt,fl)}))
            V.append(('LibClose',{'a6':LB},{'lib':(cnt,fl)}))
            V.append(('LibExpunge',{'a6':LB},{'lib':(cnt,fl)}))
    V.append(('LibReserved',{},{}))
    return V
def prep(r,opt):
    e=r.e
    if 'attn' in opt: e.w16(EXEC+0x128,opt['attn'])
    e.mem.w_block(LB-0x40,bytes(0x80)); e.w16(LB+0x10,0x24); e.w16(LB+0x12,0x2c); e.w32(LB+0x28,0x5678)
    if 'lib' in opt: e.w16(LB+0x20,opt['lib'][0]); e.w8(LB+0xe,opt['lib'][1])
tchip.RETMASK.update({})
if __name__=='__main__':
    img=build.build(sys.argv[1:] or ['src/chip.c','src/blitter.c','src/clocks.c','src/sprite.c','src/tables.c','src/libinit.c','src/cloud_entry.s'])
    tchip.run_all(img,vectors,prep)
