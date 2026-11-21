import emu,map717,sys
from emu import *
S17={v:k for k,v in map717.M.items()}
e=Emu(load_hunk('bin/p2_717.chip',S17)); e.setup_bi()
last=[]
def h(uc,a,s,u):
    last.append(a)
    if len(last)>20: last.pop(0)
    sys.stderr.write('%x\n'%a)
e.uc.hook_add(UC_HOOK_CODE,h,begin=CODE,end=CODE+0x10000)
e.call(e.sym['InitChip'],{'a0':BI})
