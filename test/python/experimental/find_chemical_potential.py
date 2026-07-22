import unittest

from triqs.experimental.lattice import BzIntOptions, make_tb_hk_from_w90_tb_file, find_chemical_potential, gloc
import numpy as np
from triqs.mesh import MeshImFreq, MeshDLRImFreq
from triqs.gfs import density, BlockGf, Gf

class findMuTests(unittest.TestCase):

  def setUp(self):
    pass

  def test_find_mu_multiorbital_svo(self):

    tb = make_tb_hk_from_w90_tb_file("svo")
    beta  = 10.
    n_iw    = 51
    iw_mesh = MeshImFreq(beta=beta, statistic='Fermion', n_iw=n_iw)

    # make a zero self energy with spin up and down channels
    Sigma = Gf(mesh=iw_mesh, target_shape=(tb.n_orbitals,tb.n_orbitals))
    Sigma_block = BlockGf(name_list=['up', 'down'], block_list=[Sigma,Sigma])

    # calculate mu, as well as density from the returned mu value to check it works
    # in both directions
    options = BzIntOptions(k_grid = [12,12,12], k_grid_max=[12,12,12], run_adaptive = False)
    # use the functions for the bare quantities
    mu = find_chemical_potential(1.0, tb, Sigma_block, options)
    gloc_result = gloc(tb, mu, Sigma_block, options)

    n = 0
    for bl, g_bl in gloc_result:
      n += np.trace(density(g_bl)).real

    # check against reference values that this works
    # Note DFT value: Efermi = 12.6263 -- can be replicated with 12x12x12 grid and small beta
    self.assertAlmostEqual(mu, 12.580778955845707, places=4)
    self.assertAlmostEqual(n, 1.0, places=4)

  def test_find_mu_multiorbital_svo_dlr(self):

    tb = make_tb_hk_from_w90_tb_file("svo")
    beta = 10.
    w_mesh = MeshDLRImFreq(beta=beta, statistic='Fermion', w_max=20.0, eps=1e-8)

    # make a zero self energy with spin up and down channels on a DLR Matsubara mesh
    Sigma = Gf(mesh=w_mesh, target_shape=(tb.n_orbitals,tb.n_orbitals))
    Sigma_block = BlockGf(name_list=['up', 'down'], block_list=[Sigma,Sigma])

    # use the same fixed grid as the imfreq test for a fair comparison
    options = BzIntOptions(k_grid = [12,12,12], k_grid_max=[12,12,12], run_adaptive = False)
    mu = find_chemical_potential(1.0, tb, Sigma_block, options)
    gloc_result = gloc(tb, mu, Sigma_block, options)

    n = 0
    for bl, g_bl in gloc_result:
      n += np.trace(density(g_bl)).real

    # the compact DLR Matsubara mesh reproduces the dense imfreq reference (mu ~ 12.5808)
    self.assertAlmostEqual(mu, 12.580778955845707, places=4)
    self.assertAlmostEqual(n, 1.0, places=4)

if __name__ == '__main__':
    unittest.main()
