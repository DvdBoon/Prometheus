import emu,p2dis,map717
from emu import *
s,h,syms=p2dis.symbols('bin/p2_716.chip')
S16={n.lstrip('_'):a for a,n in syms.items()}
S17={v:k for k,v in map717.M.items()}
L=[]
for path,S in (('bin/p2_716.chip',S16),('bin/p2_717.chip',S17)):
    e=Emu(load_hunk(path,S)); e.setup_bi()
    d0,log=e.call(e.sym['InitChip'],{'a0':BI})
    w=[x for x in log if x[0]!='R']; L.append(w)
    print(path,len(log),len(w))
import difflib
for l in difflib.unified_diff([str(x) for x in L[0]],[str(x) for x in L[1]],lineterm='',n=1): print(l)
