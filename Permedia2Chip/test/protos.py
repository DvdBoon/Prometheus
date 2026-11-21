import re
BI_OFF={}  # filled from offsets
def parse(path='src/boardinfo.h'):
    t=open(path,encoding='latin1').read()
    s=t[t.index('struct BoardInfo{'):]
    s=s[:s.index('};')]
    out={}
    for m in re.finditer(r'^\s*([\w\s\*]+?)\s+ASM\s+\(\*(\w+)\)\((.*)\);',s,re.M):
        ret,name,args=m.groups()
        regs=re.findall(r'__REG([AD][0-7])\(([^)]*)\)',args)
        out[name]=(ret.strip(),[(r.lower(),ty.strip()) for r,ty in regs])
    return out
if __name__=='__main__':
    for k,v in parse().items(): print(k,v)
