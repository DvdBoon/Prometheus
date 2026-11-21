import hunk,struct,sys,re,capstone

def symbols(path):
    s,h=hunk.parse(path); syms={}
    for v,n in h[0]['syms']: syms[v]=n
    for nm,d in h[1]['other'] if len(h)>1 else []:
        ns,ss=struct.unpack('>II',d[4:12]); st=d[12+ns:12+ns+ss]
        for i in range(ns//12):
            sx,t,o,de,v=struct.unpack('>IBBhI',d[12+i*12:24+i*12])
            n=st[sx:st.index(b'\0',sx)].decode()
            if t in (4,5) and not n.startswith(('gcc2','___gnu','_SWIZ')): syms.setdefault(v,n)
    return s,h,syms
def dis(path,extra={}):
    s,h,syms=symbols(path); syms.update(extra)
    code=h[0]['data']; rel={}
    for th,offs in h[0]['relocs'].items():
        for o in offs: rel[o]=th
    md=capstone.Cs(capstone.CS_ARCH_M68K,capstone.CS_MODE_M68K_020); md.skipdata=True
    out=[]
    for i in md.disasm(code,0):
        if i.address in syms: out.append('\n%s:'%syms[i.address])
        r=[ (o-i.address,rel[o]) for o in range(i.address,i.address+i.size) if o in rel]
        ann=''
        for off,th in r:
            v=struct.unpack('>I',code[i.address+off:i.address+off+4])[0]
            ann+=' ; R%s:%s'%(['code','data'][th] if th<2 else th, syms.get(v,hex(v)) if th==0 else hex(v))
        ops=i.op_str
        out.append('%05x %-24s %s %s%s'%(i.address,i.bytes.hex(),i.mnemonic,ops,ann))
    return '\n'.join(out)
if __name__=='__main__':
    print(dis(sys.argv[1]))
