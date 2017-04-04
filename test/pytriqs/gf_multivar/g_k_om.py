from pytriqs.gf import *
from pytriqs.lattice import *
from pytriqs.lattice.tight_binding import *
from numpy import cos

BL = BravaisLattice(units = [(1,0,0) , (0,1,0) ]) #square lattice
nk=20
mk = MeshBrillouinZone(BrillouinZone(BL), nk)
miw=MeshImFreq(beta=1., S="Fermion", n_max=100) #not used  (just demo)
mprod_iw = MeshProduct(mk, miw) # not used (just demo)

mw=MeshReFreq(-5,5, 100)
mprod = MeshProduct(mk, mw)

for m in mk: print m
    
nkmid=nk/2
path=[]
for i in range(0,nkmid):    path.append((i,i))
for i in range(0,nkmid):    path.append((nkmid-i,nkmid))
for i in range(0,nkmid):    path.append((0,nkmid-i))
   
#let us fill two G_k_w
G_w = GfReFreq(mesh=mw, target_shape = [1,1]) 

t=-0.25
U=5.0
eta=0.2
G_k_w = Gf(mesh = mprod, target_shape = [1,1])
G_k_w_Mott = Gf(mesh = mprod, target_shape= [1,1])
ik=0
for k in G_k_w.mesh.components[0]:
 G_w << inverse(iOmega_n-2*t*(cos(k[0])+cos(k[1]))+eta*1j)
 G_k_w.data[ik,:,0,0]=G_w.data[:,0,0]
 G_w << inverse(iOmega_n-2*t*(cos(k[0])+cos(k[1]))+eta*1j - 0.25*U**2*inverse(iOmega_n+eta*1j))
 G_k_w_Mott.data[ik,:,0,0]=G_w.data[:,0,0]
 ik+=1

