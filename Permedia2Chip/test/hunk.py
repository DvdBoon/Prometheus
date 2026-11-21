import struct,sys
def rd(b,o): return struct.unpack('>I',b[o:o+4])[0]
def parse(path):
    b=open(path,'rb').read(); o=0
    assert rd(b,0)==0x3f3; o=4
    while rd(b,o): n=rd(b,o); o+=4+4*n
    o+=4; tab=rd(b,o); first=rd(b,o+4); last=rd(b,o+8); o+=12
    sizes=[rd(b,o+4*i)&0x3fffffff for i in range(last-first+1)]; o+=4*len(sizes)
    hunks=[]; cur=None
    while o<len(b):
        t=rd(b,o)&0x3fffffff; o+=4
        if t in (0x3e9,0x3ea):
            n=rd(b,o); o+=4; cur={'type':'CODE' if t==0x3e9 else 'DATA','data':b[o:o+4*n],'relocs':{},'syms':[],'other':[]}; hunks.append(cur); o+=4*n
        elif t==0x3eb:
            n=rd(b,o); o+=4; cur={'type':'BSS','data':b'','size':n*4,'relocs':{},'syms':[],'other':[]}; hunks.append(cur)
        elif t==0x3ec:
            while True:
                n=rd(b,o); o+=4
                if n==0: break
                h=rd(b,o); o+=4
                cur['relocs'].setdefault(h,[]).extend(rd(b,o+4*i) for i in range(n)); o+=4*n
        elif t==0x3f0:
            while True:
                n=rd(b,o); o+=4
                if n==0: break
                name=b[o:o+4*n].rstrip(b'\0').decode('latin1'); o+=4*n; v=rd(b,o); o+=4
                cur['syms'].append((v,name))
        elif t==0x3f1:
            n=rd(b,o); o+=4; cur['other'].append(('debug',b[o:o+4*n])); o+=4*n
        elif t==0x3f2: pass
        else: raise Exception('hunk %x at %x'%(t,o))
    return sizes,hunks
if __name__=='__main__':
    s,h=parse(sys.argv[1])
    print(s)
    for i,x in enumerate(h):
        print(i,x['type'],len(x['data']) or x.get('size'),{k:len(v) for k,v in x['relocs'].items()},len(x['syms']),[ (a,len(d)) for a,d in x['other']])
