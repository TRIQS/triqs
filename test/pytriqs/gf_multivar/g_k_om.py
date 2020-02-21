from __future__ import print_function
from __future__ import unicode_literals
from pytriqs.gf import *
from pytriqs.lattice import *
from pytriqs.archive import *
from pytriqs.lattice.tight_binding import * 
from pytriqs.utility.h5diff import h5diff
from pytriqs.utility.comparison_tests import assert_gfs_are_close
from numpy import cos

# === Initialize the containers and meshes

BL = BravaisLattice(units = [(1,0,0) , (0,1,0) ]) #square lattice
nk=20
mesh_k = MeshBrillouinZone(BrillouinZone(BL), nk)
mesh_iw = MeshImFreq(beta=1., S="Fermion", n_max=100) #not used  (just demo)
mesh_k_iw = MeshProduct(mesh_k, mesh_iw) # not used (just demo)

mesh_w = MeshReFreq(-5,5, 100)
mesh_k_w = MeshProduct(mesh_k, mesh_w)

for m in mesh_k: print(m)
    
# === Fill two G_k_w

G_w = GfReFreq(mesh=mesh_w, target_shape = [1,1]) 

t=-0.25
U=5.0
eta=0.2

G_k_w = Gf(mesh = mesh_k_w, target_shape = [1,1])
G_k_w_Mott = Gf(mesh = mesh_k_w, target_shape= [1,1])

for k in G_k_w.mesh.components[0]:
    G_w << inverse(iOmega_n-2*t*(cos(k[0])+cos(k[1]))+eta*1j)
    G_k_w[k,:].data[:] = G_w.data

    G_w << inverse(iOmega_n-2*t*(cos(k[0])+cos(k[1]))+eta*1j - 0.25*U**2*inverse(iOmega_n+eta*1j))
    G_k_w_Mott[k,:].data[:] = G_w.data

# === Test the hdf5 write/read

with HDFArchive('g_k_om.h5', 'w') as arch:
    arch['mesh_k'] = mesh_k
    arch['G'] = G_k_w

with HDFArchive('g_k_om.h5', 'r') as arch:
    mesh_k_read = arch['mesh_k']
    G_k_w_read = arch['G']

    assert(mesh_k == mesh_k_read)
    assert_gfs_are_close(G_k_w, G_k_w_read)
