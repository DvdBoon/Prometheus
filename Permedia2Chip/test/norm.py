import sys,re
for l in open(sys.argv[1]):
    l=l.rstrip('\n')
    if not l or l.endswith(':'): 
        if l: print(l)
        continue
    p=l.split(None,2)
    if len(p)<3: continue
    ins=p[2]
    if ins.strip()=='nop': continue
    ins=re.sub(r'\$[0-9a-f]+(?=$|\s|;|,)', lambda m: m.group(0) if not re.match(r'(b|j|db)',ins) else '$X', ins)
    ins=re.sub(r'; R(code|data):\S+','; R',ins)
    print(ins)
