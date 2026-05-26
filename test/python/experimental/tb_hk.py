import unittest

from triqs.experimental.lattice import TbHk, BzIntOptions
import numpy as np
from h5 import *
import triqs.utility.mpi as mpi

class TbTests(unittest.TestCase):

  def setUp(self):

    # set up tb_hopping object
    displ_vec       = np.array([[1, 0, 0], [-1, 0, 0], [0, 1, 0], [0, -1, 0], [0, 0, 1], [0, 0, -1]])
    overlap_mat_vec = np.array([np.eye(2) for _ in range(displ_vec.shape[0])])
    self.tb_H = TbHk(displ_vec, overlap_mat_vec)

  def test_get_R_idx(self):

      Ridx = self.tb_H.get_R_idx([0, 1, 0])

      self.assertEqual(Ridx, 2)
      self.assertAlmostEqual(-0.097887 + 0j, self.tb_H([0.25, 0.5, 0.05])[0, 0], places=5)

  def test_evaluators(self):

    Hk_ab = self.tb_H([0.33, 0.5, 0.33]);
    kpoint = np.array([[+0.15, +0.338028169014, -0.4], [0.33, 0.5, 0.33]])

    self.assertAlmostEqual(Hk_ab[0,0], self.tb_H(kpoint)[1,0,0])

  def test_h5_io(self):

    # Write list of operators
    with HDFArchive('tb_Hk.h5', 'w') as arch:
        arch['tb_Hk'] = self.tb_H
    # Read list
    arch = HDFArchive('tb_Hk.h5', 'r')

    self.assertEqual(arch['tb_Hk'], self.tb_H)

  def test_mpi_broadcast(self):

    tb = mpi.bcast(self.tb_H)
    self.assertEqual(tb, self.tb_H)

if __name__ == '__main__':
    unittest.main()