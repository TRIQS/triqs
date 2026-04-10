# Copyright (c) 2021-2024 Simons Foundation
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
# Authors: Alexander Hampel, Harrison LaBollita, Nils Wentzell

import numpy as np

from triqs.gfs import Gf, iOmega_n, inverse, BlockGf, make_gf_dlr, make_gf_imfreq, make_gf_dlr_imfreq
from triqs.mesh import MeshImFreq, MeshDLRImTime
from triqs.gfs.dlr_crm_dyson_solver import minimize_dyson
from triqs.utility.comparison_tests import assert_gfs_are_close

import unittest


class test_crm_dyson_solver(unittest.TestCase):
    def setUp(self):
        self.U = 2.0
        self.beta = 20
        self.w_max = 4
        self.lamb = self.beta * self.w_max
        self.n_iw = 1000
        self.eps = 1e-10
        # noise on input G_tau
        self.tol = 1e-8
        self.eps1 = 0.5 * self.U
        self.eps2 = -0.5 * self.U
        np.random.seed(85281)
        # create reference solution on full mesh
        self.iw_mesh = MeshImFreq(beta=self.beta, statistic='Fermion', n_iw=self.n_iw)
        self.Sigma_iw_ref = Gf(mesh=self.iw_mesh, target_shape=[1, 1])
        self.Sigma_iw_ref << self.U / 2 + 0.25 * self.U * self.U * inverse(iOmega_n)
        # Specify the moments
        self.moments = np.array([self.U / 2, 0.25 * self.U * self.U], dtype=complex).reshape(2, 1, 1)

    # Create input Green functions: Non-interacting and interacting Green function of the Hubbard atom
    def make_G0_G_HubbardAtom(self, target_shape):
        mesh_dlr_tau = MeshDLRImTime(beta=self.beta, statistic='Fermion', w_max=self.w_max, eps=self.eps)
        G0_tau = Gf(mesh=mesh_dlr_tau, target_shape=target_shape)
        G_tau = Gf(mesh=mesh_dlr_tau, target_shape=target_shape)

        one_fermion = lambda eps, t: -np.exp((self.beta * (eps < 0) - t) * eps) / (1.0 + np.exp(-self.beta * abs(eps)))
        for t in mesh_dlr_tau:
            if target_shape == []:
                G0_tau[t] = one_fermion(self.eps2, t)
                G_tau[t] = 0.5 * one_fermion(self.eps1, t) + 0.5 * one_fermion(self.eps2, t)
            else:
                np.fill_diagonal(G0_tau[t], one_fermion(self.eps2, t))
                np.fill_diagonal(G_tau[t], 0.5 * one_fermion(self.eps1, t) + 0.5 * one_fermion(self.eps2, t))

        return G0_tau, G_tau

    def test_DLRImTime_constraint(self):
        # create input data from Hubbard atom with tol noise
        G0_tau, G_tau = self.make_G0_G_HubbardAtom(target_shape=[1, 1])
        G_tau.data[:] += np.random.normal(scale=self.tol, size=G_tau.data.shape)

        # run CRM Dyson solver with constraints
        Sigma_dlr, Sigma_HF, residual = minimize_dyson(G0_dlr=G0_tau, G_dlr=G_tau, Sigma_moments=self.moments)
        assert residual < 1e-5

        # comparison with reference solution
        Sigma_iw_crm = make_gf_imfreq(Sigma_dlr, n_iw=self.n_iw)
        Sigma_iw_crm += Sigma_HF
        assert_gfs_are_close(Sigma_iw_crm, self.Sigma_iw_ref, 1e-5)

        # analytic moments of self-energy, use only first moment as constraint for testing
        bS_moments = {'up': self.moments[0:1], 'dn': self.moments[0:1]}

        # create block Gf
        bG0_tau = BlockGf(name_list=['up', 'dn'], block_list=[G0_tau, G0_tau], make_copies=True)
        bG_tau = BlockGf(name_list=['up', 'dn'], block_list=[G_tau, G_tau], make_copies=True)

        # run CRM Dyson solver with constraints
        Sigma_dlr, Sigma_HF, residual = minimize_dyson(G0_dlr=bG0_tau, G_dlr=bG_tau, Sigma_moments=bS_moments)

        for b in ['up', 'dn']:
            assert residual[b] < 1e-5
            # comparison with reference solution
            Sigma_iw_crm = make_gf_imfreq(Sigma_dlr[b], n_iw=self.n_iw)
            Sigma_iw_crm += Sigma_HF[b]
            assert_gfs_are_close(Sigma_iw_crm, self.Sigma_iw_ref, 1e-5)

    def test_DLRImFreq_HF(self):
        # create input data from Hubbard atom with tol noise
        G0_tau, G_tau = self.make_G0_G_HubbardAtom(target_shape=[1, 1])
        G_tau.data[:] += np.random.normal(scale=self.tol, size=G_tau.data.shape)

        G_iw = make_gf_dlr_imfreq(G_tau)
        G0_iw = make_gf_dlr_imfreq(G0_tau)

        # run CRM Dyson solver with constraints
        Sigma_dlr, Sigma_HF, residual = minimize_dyson(G0_dlr=G0_iw, G_dlr=G_iw, Sigma_moments=self.moments)
        assert residual < 1e-5

        # comparison with reference solution
        Sigma_iw_crm = make_gf_imfreq(Sigma_dlr, n_iw=self.n_iw)
        Sigma_iw_crm += Sigma_HF
        assert_gfs_are_close(Sigma_iw_crm, self.Sigma_iw_ref, 1e-5)

    def test_DLR_scalar(self):
        # create input data from Hubbard atom with tol noise
        G0_tau, G_tau = self.make_G0_G_HubbardAtom(target_shape=[])
        G_tau.data[:] += np.random.normal(scale=self.tol, size=G_tau.data.shape)

        G_dlr = make_gf_dlr(G_tau)
        G0_dlr = make_gf_dlr(G0_tau)

        # run CRM Dyson solver with constraints
        mom_scalar = self.moments[:, 0, 0]
        Sigma_dlr, Sigma_HF, residual = minimize_dyson(G0_dlr=G0_dlr, G_dlr=G_dlr, Sigma_moments=mom_scalar)
        assert residual < 1e-5

        # comparison with reference solution
        Sigma_iw_crm = make_gf_imfreq(Sigma_dlr, n_iw=self.n_iw)
        Sigma_iw_crm += Sigma_HF
        assert_gfs_are_close(Sigma_iw_crm, self.Sigma_iw_ref[0, 0], 1e-5)

    def test_mat_DLRImTime_tailfit(self):
        # create input data from Hubbard atom with tol noise
        G0_tau, G_tau = self.make_G0_G_HubbardAtom(target_shape=[2, 2])
        G_tau.data[:] += np.random.normal(scale=self.tol, size=G_tau.data.shape)

        # get estimate of Hartree shift from tail fit on noisy sigma
        G_iw = make_gf_imfreq(G_tau, n_iw=self.n_iw)
        G0_iw = make_gf_imfreq(G0_tau, n_iw=self.n_iw)
        S_iw = inverse(G0_iw) - inverse(G_iw)
        tail, err = S_iw.fit_hermitian_tail()

        # run CRM Dyson solver with constraints
        Sigma_dlr, Sigma_HF, residual = minimize_dyson(G0_dlr=G0_tau, G_dlr=G_tau, Sigma_moments=tail[0:1])
        assert residual < 1e-5

        # comparison with reference solution
        Sigma_iw_crm = make_gf_imfreq(Sigma_dlr, n_iw=self.n_iw)
        Sigma_iw_crm += Sigma_HF
        assert_gfs_are_close(Sigma_iw_crm[0:1,0:1], self.Sigma_iw_ref, 1e-5)


if __name__ == '__main__':
    unittest.main()
