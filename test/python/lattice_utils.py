# Copyright (c) 2021-2022 Simons Foundation
# Copyright (c) 2023 Hugo U. R. Strand
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You may obtain a copy of the License at
#     https:#www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Alexander Hampel, Hugo U. R. Strand, Nils Wentzell

import unittest
from triqs.gf import GfImFreq, MeshImFreq, BlockGf
from triqs.lattice import BravaisLattice, BrillouinZone
from triqs.sumk import SumkDiscreteFromLattice
from triqs.lattice.utils import k_space_path, TB_from_wannier90, TB_from_pythTB

import numpy as np

class test_utils(unittest.TestCase):

    def test_k_space_path(self):

        units = np.array([[1.0, 1.0, 0.0], [0.5, 0.0, 0.5], [0., 1.5, 1.5]])
        bl    = BravaisLattice(units=units)
        bz    = BrillouinZone(bl)

        Gamma = np.array([0.0, 0.0, 0.0])
        M     = np.array([0.5, 0.5, 0.0])
        R     = np.array([0.5, 0.5, 0.5])
        X     = np.array([0.5, 0.0, 0.0])
        Z     = np.array([0.0, 0.0, 0.5])

        # ----

        num = 101
        paths = [(Gamma, M), (M, R), (X, Z)]
        kvecs, dist, ticks = k_space_path(paths, num=num, bz=bz)

        ticks_exact = [0.]
        for n, (ki, kf) in enumerate(paths):
            dk = kf - ki

            kvec = kvecs[n*num:(n+1)*num]
            kvec_exact = ki[None,:] + np.linspace(0,1,num)[:,None] * dk[None,:]
            self.assertTrue(np.array_equal(kvec, kvec_exact))

            dd = np.linalg.norm(np.dot(dk, bz.units))
            ticks_exact.append(dd + ticks_exact[-1])

        self.assertTrue(np.array_equal(ticks, ticks_exact))

    def test_TB_from_w90(self):

        # try first with spin and add local
        n_orb = 3
        mu = -12.3461 * np.eye(n_orb*2)
        TB_from_wannier90(seed='wannier_TB_test', path='./', extend_to_spin=True, add_local=mu)
        # and normal
        mu = -12.3461 * np.eye(n_orb)
        tbl_w90 = TB_from_wannier90(seed='wannier_TB_test', path='./', extend_to_spin=False, add_local=mu)

        # check if orbitals are degenerate as in hr.dat
        hr_0 = tbl_w90.hoppings[(0, 0, 0)]
        self.assertTrue(hr_0[0, 0] == hr_0[1, 1])

        # check 100 hopping
        hr_100 = tbl_w90.hoppings[(1, 0, 0)]
        self.assertTrue(hr_100[0, 0] == -0.256015)
        self.assertTrue(hr_100[2, 2] == -0.025979)

        # Evaluate dispersion on k-space path
        Gamma = np.array([0.0, 0.0, 0.0])
        M = np.array([0.5, 0.5, 0.0])
        paths = [(Gamma, M)]
        kvecs, dist, ticks = k_space_path(paths, num=101, bz=tbl_w90.bz)
        epsilon_k = tbl_w90.dispersion(kvecs)
        self.assertTrue(epsilon_k.shape == (101, 3))

        # Obtain H_k on same path and compare eigenvalues against dispersion
        H_k = tbl_w90.fourier(kvecs)
        evals = np.sort(np.linalg.eigvalsh(H_k))
        self.assertTrue(np.allclose(evals, epsilon_k))

        # Obtain H_k as Gf
        H_k = tbl_w90.fourier(tbl_w90.get_kmesh(11))
        self.assertTrue(H_k.data.shape == (1331, 3, 3))

    def test_TB_from_pythTB(self):

        try:
            from pythtb import tb_model
        except(ModuleNotFoundError):
            print('skipping pythTB test because module pythTB could not be imported')
            return

        # set up simple 1band model with NN and NNN hopping
        n_orb = 3
        lattice = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
        orbitals = [[0, 0, 0]] * n_orb
        d0 = 1.0
        t0 = -0.5
        ptb = tb_model(3, 3, lattice, orbitals)
        # set cfs and hoppings
        ptb.set_onsite([d0] * n_orb)
        ptb.set_hop(1*t0, 0, 0, [1, 0, 0])
        ptb.set_hop(2*t0, 1, 1, [1, 0, 0])
        ptb.set_hop(1*t0, 0, 0, [0, 1, 0])
        ptb.set_hop(4*t0, 2, 2, [0, 1, 0])
        ptb.set_hop(1*t0, 1, 1, [0, 0, 1])
        ptb.set_hop(8*t0, 2, 2, [0, 0, 1])

        tbl_ptb = TB_from_pythTB(ptb)

        # Check that hopping dict matches pythtb model above
        hr_0 = tbl_ptb.hoppings[(0, 0, 0)]
        self.assertTrue(hr_0[0, 0] == hr_0[1, 1] == hr_0[2, 2] == d0)

        hr_100 = tbl_ptb.hoppings[(1, 0, 0)]
        self.assertTrue(2*hr_100[0, 0] == hr_100[1, 1] == 2*t0)

        hr_010 = tbl_ptb.hoppings[(0, 1, 0)]
        self.assertTrue(4*hr_010[0, 0] == hr_010[2, 2] == 4*t0)

        hr_001 = tbl_ptb.hoppings[(0, 0, 1)]
        self.assertTrue(8*hr_001[1, 1] == hr_001[2, 2] == 8*t0)

        # Evaluate dispersion on k-space path
        Gamma = np.array([0.0, 0.0, 0.0])
        M = np.array([0.5, 0.5, 0.0])
        paths = [(Gamma, M)]
        kvecs, dist, ticks = k_space_path(paths, num=101, bz=tbl_ptb.bz)
        epsilon_k = tbl_ptb.dispersion(kvecs)
        self.assertTrue(epsilon_k.shape == (101, 3))

        # compare eigenvalues against pythTB dispersion
        epsilon_k_ptb = ptb.solve_all(kvecs).T
        self.assertTrue(np.allclose(epsilon_k_ptb, epsilon_k), 'eigenvalues between PythTB and TRIQS TBLattice do not match')

        # Obtain H_k on same path and compare eigenvalues against dispersion
        H_k = tbl_ptb.fourier(kvecs)
        evals = np.linalg.eigvalsh(H_k)
        self.assertTrue(np.allclose(evals, epsilon_k))

        # Obtain H_k as Gf
        H_k = tbl_ptb.fourier(tbl_ptb.get_kmesh(11))
        self.assertTrue(H_k.data.shape == (1331, 3, 3))

    def test_sumk_discrete(self):
        tbl_w90 = TB_from_wannier90(seed='wannier_TB_test', path='./', extend_to_spin=False)

        SK = SumkDiscreteFromLattice(lattice=tbl_w90, n_points=10)
        Sigma_proto = GfImFreq(mesh=MeshImFreq(beta=40, S='Fermion', n_iw=1025),
                               target_shape=[tbl_w90.n_orbitals, tbl_w90.n_orbitals])
        Sigma_iw = BlockGf(name_list=['up', 'down'], block_list=[Sigma_proto, Sigma_proto], make_copies=True)

        # Sumk only accepts Sigma of MeshImFreq
        Gloc = SK(Sigma=Sigma_iw, mu=12.3461)
        # check if density is close to 1, not to strict since nkpts is relatively small
        assert abs(Gloc.total_density().real - 1.0) < 1e-2

    def test_tb_lattice_1d_v_k(self, verbose=False):

        from triqs.lattice.tight_binding import TBLattice

        t = 1.0 # nearest-neigbhour hopping amplitude

        H_r = TBLattice( 
            units=[
                (1,0,0), # basis vector in the x-direction 
            ],
            hoppings={
                (+1,) : [[-t]], # hopping in the +x direction
                (-1,) : [[-t]], # hopping in the -x direction
            })

        def e_k_analytic(k, dim=1):
            return -2 * t * np.sum(np.cos(k[0:dim] * 2*np.pi))

        def v_k_analytic(k, idx, dim=1):
            return -2 * t * np.sum(np.cos(k[0:dim] * 2*np.pi)) * 2*np.pi * -np.tan(k[idx] * 2*np.pi)

        dim = 1

        ks = np.zeros((10, 3))
        ks[:,0] = np.linspace(0, 1, num=ks.shape[0])

        e_k = np.array([ np.squeeze(H_r.fourier(k)) for k in ks ])
        e_k_ref = np.array([ e_k_analytic(k, dim=dim) for k in ks ])
        
        idx = 0
        v_k = np.array([ np.squeeze(H_r.tb.fourier_dk(k, idx)) for k in ks ])
        v_k_ref = np.array([ v_k_analytic(k, idx, dim=dim) for k in ks ])

        if verbose:
            import matplotlib.pyplot as plt
            plt.plot(ks[:, 0], e_k, label='e_k')
            plt.plot(ks[:, 0], e_k_ref, '.', label='e_k_ref')
            plt.plot(ks[:, 0], v_k, '--', label='v_k')
            plt.plot(ks[:, 0], v_k_ref, 'x', label='v_k_ref')
            plt.legend()
            plt.show()

        np.testing.assert_almost_equal(e_k, e_k_ref)
        np.testing.assert_almost_equal(v_k, v_k_ref)

    def test_tb_lattice_2d_v_k(self, verbose=False):

        from triqs.lattice.tight_binding import TBLattice

        t = 1.0 # nearest-neigbhour hopping amplitude

        H_r = TBLattice( 
            units=[
                (1,0,0), # basis vector in the x-direction 
                (0,1,0), # basis vector in the y-direction
            ],
            hoppings={
                (+1,0) : [[-t]], # hopping in the +x direction
                (-1,0) : [[-t]], # hopping in the -x direction
                (0,+1) : [[-t]], # hopping in the +y direction
                (0,-1) : [[-t]], # hopping in the -y direction
            })

        def e_k_analytic(k, dim=1):
            return -2 * t * np.sum(np.cos(k[0:dim] * 2*np.pi))

        def v_k_analytic(k, idx, dim=1):
            return -2 * t * 2*np.pi * -np.sin(k[idx] * 2*np.pi)

        dim = 2

        n_k = 10
        ks = np.zeros((n_k**2, 3))
        k = np.linspace(0, 1, num=n_k)
        kx, ky = np.meshgrid(k, k)
        ks[:,0] = kx.flatten()
        ks[:,1] = ky.flatten()

        e_k = np.array([ np.squeeze(H_r.fourier(k)) for k in ks ])
        e_k_ref = np.array([ e_k_analytic(k, dim=dim) for k in ks ])
        
        idx = 0
        v_k = np.array([ np.squeeze(H_r.tb.fourier_dk(k, idx)) for k in ks ])
        v_k_ref = np.array([ v_k_analytic(k, idx, dim=dim) for k in ks ])

        if verbose:
            import matplotlib.pyplot as plt
            plt.plot(ks[:, 0], e_k, label='e_k')
            plt.plot(ks[:, 0], e_k_ref, '.', label='e_k_ref')
            plt.plot(ks[:, 0], v_k, '--', label='v_k')
            plt.plot(ks[:, 0], v_k_ref, 'x', label='v_k_ref')
            plt.legend()
            plt.show()

        np.testing.assert_almost_equal(e_k, e_k_ref)
        np.testing.assert_almost_equal(v_k, v_k_ref)
        

if __name__ == '__main__':
    unittest.main()
