import unittest

from triqs.experimental.lattice import TbHk, read_wannier90_tb_data, make_tb_hk_from_w90_hr_file
import numpy as np

class WannierTests(unittest.TestCase):

  def setUp(self):
    pass

  def test_wannier_loader(self):

    R, H_R, _, latt_vec = read_wannier90_tb_data("../../c++/experimental/lattice/svo")
    tb_from_hr = make_tb_hk_from_w90_hr_file("../../c++/experimental/lattice/svo")

    # confirm the lattice vectors are read in correctly
    for i in [0, 1, 2]:
      for j in [0, 1, 2]:
        if (i == j):
          self.assertAlmostEqual(latt_vec[i, j], 3.8419797311652863, places=9)
        else:
          self.assertAlmostEqual(latt_vec[i, j], 0.0, places=9)

    # create a tb object from _tb.dat
    tb_from_tb = TbHk(R, H_R);

    # ----------------------------------------------------------------------------
    # check that we get energies as expected at the gamma point, and for a
    # non-high sym point, where they are not degenerate.
    gamma  = np.array([[0., 0., 0.]])
    kpoint = np.array([[+0.330985915493, +0.338028169014, 0.]])

    for tb in [tb_from_tb,tb_from_hr]:

      # check gamma
      Ek_band = tb.eigenvalues(gamma);
      for i in [0,1,2]:
        self.assertAlmostEqual(11.6282, Ek_band[0, i], places=3)

      # check the arbitrary point
      Ek_band = tb.eigenvalues(kpoint)
      self.assertAlmostEqual(13.1134, Ek_band[0, 0], places=4)
      self.assertAlmostEqual(13.20945, Ek_band[0, 1], places=4)
      self.assertAlmostEqual(13.5010, Ek_band[0, 2], places=4)

      #check that the eigenvectors of the Hamiltonian properly diagonalize it at the arbitrary point
      _, eigvecs = tb.eigenvectors(kpoint)
      Uk    = eigvecs[0]
      Hk_ab = tb(kpoint)[0]
      Hk_diag = np.matmul(np.matmul(Uk.conj().T, Hk_ab), Uk)

      self.assertAlmostEqual(Hk_diag[0, 0], Ek_band[0, 0], places=4)
      self.assertAlmostEqual(Hk_diag[1, 1], Ek_band[0, 1], places=4)
      self.assertAlmostEqual(Hk_diag[2, 2], Ek_band[0, 2], places=4)

if __name__ == '__main__':
    unittest.main()