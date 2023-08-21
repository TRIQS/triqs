from triqs.gf import *
from h5 import HDFArchive

# Create Matsubara mesh
iw_mesh = MeshImFreq(beta=10, S='Fermion', n_iw=100)

# Create BlockGf
giw = Gf(mesh=iw_mesh, target_shape=())
giw << 0.0
bgf = BlockGf(block_list = [giw, giw])

# Store in hdf5 file
with HDFArchive("bgf_pyh5_ref.h5",'w') as f:
    f["bgf"] = bgf
