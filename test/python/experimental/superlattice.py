import unittest

from triqs.experimental.lattice import TbHk, BzIntOptions, Superlattice, fold, gloc
import numpy as np
from triqs.gfs import MeshDLRImFreq, Gf

class SuperlatticeTests(unittest.TestCase):

  def setUp(self):
    pass

  def test_2x2SquareLattice3d(self):

      # set up tb_hk object
      t = 1.
      displ_vec       = np.array([[1, 0, 0], [-1, 0, 0], [0, 1, 0], [0, -1, 0], [0, 0, 1], [0, 0, -1]])
      overlap_mat_vec = np.array([np.eye(1) for _ in range(displ_vec.shape[0])])
      tb = TbHk(displ_vec, overlap_mat_vec)
      SL = Superlattice([[2, 0, 0], [0, 2, 0], [0, 0, 1]], [[0, 0, 0], [1, 0, 0], [0, 1, 0], [1, 1, 0]])

      # Call the fold function
      result = fold(SL, tb)
      Rs           = result.get_R_list
      coefs = result.hoppings

      t_SL_0   = np.array([[0, t, t, 0], [t, 0, 0, t], [t, 0, 0, t], [0, t, t, 0]], dtype=complex)
      t_SL_1h  = np.array([[0, 0, 0, 0], [t, 0, 0, 0], [0, 0, 0, 0], [0, 0, t, 0]], dtype=complex)
      t_SL_1v  = np.array([[0, 0, 0, 0], [0, 0, 0, 0], [t, 0, 0, 0], [0, t, 0, 0]], dtype=complex)
      t_SL_m1h = t_SL_1h.transpose()
      t_SL_m1v = t_SL_1v.transpose()

      expected_Rs = np.array([[0, 0, 1], [0, 0, -1], [0, 0, 0], [-1, 0, 0], [0, -1, 0], [1, 0, 0], [0, 1, 0]])
      expected_coeffs = np.array([np.eye(4), np.eye(4), t_SL_0, t_SL_m1h, t_SL_m1v, t_SL_1h, t_SL_1v])

      # check R and coeffs
      for R1, R2 in zip(Rs, expected_Rs):
        np.testing.assert_allclose(np.array(R1), R2)
      self.assertAlmostEqual(-0.097887 + 0j, tb([0.25, 0.5, 0.05])[0, 0], places=5)

      for c1,c2 in zip(coefs,expected_coeffs):
        np.testing.assert_allclose(c1, c2)

  def test_2x2SquareLattice3d_with_gloc(self):

      eps   = 1.e-12
      w_max = 10.0

      def make_random_self(M):
        beta = 5.0
        iw_mesh = MeshDLRImFreq(beta=beta, statistic='Fermion', w_max=w_max, eps=eps)
        g           = Gf(mesh=iw_mesh, target_shape=[M, M])
        A           = np.random.random()
        B           = np.random.random()
        for n, iw in enumerate(iw_mesh):
          g.data[n] = A + B / (iw + 10)
        return g

      # set up tb_hk object
      t = 1.
      displ_vec       = np.array([[1, 0, 0], [-1, 0, 0], [0, 1, 0], [0, -1, 0], [0, 0, 1], [0, 0, -1]])
      overlap_mat_vec = np.array([np.eye(1) for _ in range(displ_vec.shape[0])])
      tb = TbHk(displ_vec, overlap_mat_vec)
      SL = Superlattice([[2, 0, 0], [0, 2, 0], [0, 0, 1]], [[0, 0, 0], [1, 0, 0], [0, 1, 0], [1, 1, 0]])

      # Call the fold function
      tb_AF = fold(SL, tb)
      Rs           = tb_AF.get_R_list
      coefs = tb_AF.hoppings

      Sigma1 = make_random_self(tb.n_orbitals)
      M = tb_AF.n_orbitals
      Sigma2 = Gf(mesh=Sigma1.mesh, target_shape=[M, M])

      for i in range(M):
         Sigma2.data[:, i, i] = Sigma1.data[:, 0, 0]

      opt1 = BzIntOptions(k_grid = [10,10,5], k_grid_max=[10,10,5], run_adaptive=False)
      opt2 = BzIntOptions(k_grid = [5,5,5], k_grid_max=[10,10,5], run_adaptive=False)
      g1 = gloc(tb, 0.0, Sigma1, opt1)
      g2 = gloc(tb_AF, 0.0, Sigma2, opt2)

      for i in range(M):
        np.testing.assert_allclose(g1.data[:, 0, 0], g2.data[:, i, i], rtol=1.e-12)

if __name__ == '__main__':
    unittest.main()