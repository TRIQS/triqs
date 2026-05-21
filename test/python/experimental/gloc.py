import unittest

from triqs.experimental.lattice import TbHk, BzIntOptions, gloc
import numpy as np
from h5 import *
from triqs.mesh import MeshImFreq

class glocTests(unittest.TestCase):

  def setUp(self):

    self.displ_vec       = np.array([[1, 0, 0], [-1, 0, 0], [0, 1, 0], [0, -1, 0], [0, 0, 1], [0, 0, -1]])
    self.beta = 10.
    self.n_iw = 5
    self.iw_mesh = MeshImFreq(beta=self.beta, statistic='Fermion', n_iw=self.n_iw, opt="positive_frequencies_only")
    self.ref = [0+ -0.419074j, 0+ -0.364261j, 0+ -0.316034j, 0+ -0.275981j, 0 + -0.243231j]

  def test_gloc_single_orbital(self):

    overlap_mat_vec = np.array([np.eye(1) for _ in range(self.displ_vec.shape[0])])
    tb = TbHk(self.displ_vec, overlap_mat_vec)

    # call the integration function
    options = BzIntOptions(k_grid = [10,10,10], k_grid_max=[20,20,20], tolerance = 1.e-3)

    gloc_result = gloc(self.iw_mesh, tb, 0., options)

    for idx_w in range(self.n_iw):
      self.assertAlmostEqual(self.ref[idx_w], gloc_result[self.iw_mesh[idx_w]][0, 0], places=3)

  def test_gloc_multiple_orbitals(self):

    # two orbitals
    overlap_mat_vec = np.array([np.eye(2) for _ in range(self.displ_vec.shape[0])])
    tb = TbHk(self.displ_vec, overlap_mat_vec)

    # call the integration function
    options = BzIntOptions(k_grid = [10,10,10], k_grid_max=[40,40,40], tolerance = 1.e-3)

    gloc_result = gloc(self.iw_mesh, tb, 0., options)

    for idx_w in range(self.n_iw):
      self.assertAlmostEqual(self.ref[idx_w], gloc_result[self.iw_mesh[idx_w]][0, 0], places=3)

if __name__ == '__main__':
    unittest.main()