import subprocess, protos, sys, os
P=protos.parse()
CC='m68k-linux-gnu-gcc'
CF='-m68020 -O2 -fomit-frame-pointer -ffreestanding -fno-builtin -fno-pic -D__STORMGCC__ -include ndk/cloudcompat.h -I src -I ndk -Wall -Wno-pointer-sign -Wno-unused'.split()
# internal C functions called with register args by the harness
EXTRA={'HardInterruptCodeC':[('a1','')],'InitChip':[('a0','')],'LibInit':[('d0',''),('a0',''),('a6','')],'LibOpen':[('a6','')],'LibClose':[('a6','')],'LibExpunge':[('a6','')],'LibReserved':[]}
def build(srcs, out='build/p2c', extra_cflags=[]):
    os.makedirs('build',exist_ok=True)
    objs=[]
    for s in srcs:
        o='build/'+os.path.basename(s).rsplit('.',1)[0]+'.o'
        subprocess.check_call([CC]+CF+extra_cflags+['-c',s,'-o',o]); objs.append(o)
    # which functions exist?
    nm=subprocess.check_output(['m68k-linux-gnu-nm']+objs,text=True)
    defined={l.split()[2] for l in nm.splitlines() if len(l.split())==3 and l.split()[1]=='T'}
    st=['\t.text']
    for n,(ret,args) in list(P.items())+[(k,('',v)) for k,v in EXTRA.items()]:
        if n not in defined: continue
        st+=['\t.globl %s_stub'%n,'%s_stub:'%n]
        for r,t in reversed(args): st.append('\tmovel %%%s,%%sp@-'%r)
        st+=['\tjsr %s'%n,'\tlea %%sp@(%d),%%sp'%(4*len(args)),'\trts']
    open('build/stubs.s','w').write('\n'.join(st)+'\n')
    subprocess.check_call([CC,'-Wa,--noexecstack','-c','build/stubs.s','-o','build/stubs.o'])
    subprocess.check_call([CC,'-nostdlib','-static','-Wl,--build-id=none,-z,noexecstack','-T','link.ld','-o',out+'.elf']+objs+['build/stubs.o','-lgcc'])
    subprocess.check_call(['m68k-linux-gnu-objcopy','-O','binary',out+'.elf',out+'.bin'])
    syms={}
    for l in subprocess.check_output(['m68k-linux-gnu-nm',out+'.elf'],text=True).splitlines():
        a,t,n=l.split(); syms[n]=int(a,16)
    return {'flat':open(out+'.bin','rb').read(),'syms':syms}
if __name__=='__main__':
    img=build(sys.argv[1:]); print(len(img['flat']),'bytes')
