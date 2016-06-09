from pytriqs.gf.local.multivar import *
from pytriqs.gf.local import *
from pytriqs.lattice import *
from pytriqs.lattice.tight_binding import *
from pytriqs.plot.mpl_interface import *
from numpy import cos

BL = BravaisLattice(units = [(1,0,0) , (0,1,0) ]) #square lattice
nk=20
mk = MeshBrillouinZone(BrillouinZone(BL), nk)
miw=MeshImFreq(beta=1., S="Fermion", n_max=100) #not used  (just demo)
mprod_iw = MeshBrillouinZoneImFreq(mk, miw) # not used (just demo)

mw=MeshReFreq(-5,5, 100)
mprod = MeshBrillouinZoneReFreq(mk, mw)

for m in mk:   print m
    
nkmid=nk/2
path=[]
for i in range(0,nkmid):    path.append((i,i))
for i in range(0,nkmid):    path.append((nkmid-i,nkmid))
for i in range(0,nkmid):    path.append((0,nkmid-i))
    
#let us fill two G_k_w
G_w = GfReFreq(mesh=mw, shape=[1,1])
t=-0.25
U=5.0
eta=0.2
G_k_w = GfBrillouinZone_x_ReFreq(mprod, [1,1])
G_k_w_Mott = GfBrillouinZone_x_ReFreq(mprod, [1,1])
ik=0
for k in G_k_w.mesh.components[0]:
 G_w << inverse(iOmega_n-2*t*(cos(k[0])+cos(k[1]))+eta*1j)
 G_k_w.data[ik,:,0,0]=G_w.data[:,0,0]
 G_w << inverse(iOmega_n-2*t*(cos(k[0])+cos(k[1]))+eta*1j - 0.25*U**2*inverse(iOmega_n+eta*1j))
 G_k_w_Mott.data[ik,:,0,0]=G_w.data[:,0,0]
 ik+=1

#offset = GfBrillouinZone_x_ReFreq(mprod, [1,1])
#offset.data[:,:,0,0]=0.2*1j
#
#
#ind=0
#for i,j in path:
# oplot(((-1./pi)*(G_k_w+ind*offset).slice_at_const_w1([i,j,0])).imag, '-k', label=None)
# oplot(((-1./pi)*(G_k_w_Mott+ind*offset).slice_at_const_w1([i,j,0])).imag, '-r', label=None)
# ind+=1

