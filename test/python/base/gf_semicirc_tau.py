"""Test g_semicirc_tau panel quadrature against adaptive quadrature reference,
and SemiCircular descriptor on DLR imaginary-time meshes."""

import numpy as np
import unittest

from triqs.gf import Gf, make_gf_dlr
from triqs.gf.meshes import MeshDLRImTime, MeshDLRImFreq, MeshImFreq
from triqs.gf.descriptors import SemiCircular
from triqs.gf.semicirc import g_semicirc_tau, g_semicirc_tau_adapquad, g_semicirc_iw


class test_g_semicirc_tau(unittest.TestCase):

    def _check(self, D):
        betas = np.logspace(0, 4, 5)
        for beta in betas:
            left = np.logspace(np.log10(1.0 / D), np.log10(beta / 2), 10)
            taus = np.union1d(left, beta - left)
            G_panel = g_semicirc_tau(taus, beta, D)
            G_ref = g_semicirc_tau_adapquad(taus, beta, D)
            err = np.max(np.abs(G_panel - G_ref))
            self.assertLess(err, 1e-12,
                            f"D={D}, beta={beta:.1f}: max error {err:.2e} exceeds tolerance")

    def test_D1(self):
        self._check(D=1.0)

    def test_D05(self):
        self._check(D=0.5)

    def test_dlr_tau_to_iw(self):
        beta, eps, w_max, D = 100., 1e-10, 1., 1.

        g_tau = Gf(mesh=MeshDLRImTime(beta, 'Fermion', w_max, eps), target_shape=[])
        g_tau << SemiCircular(D)
        g_c = make_gf_dlr(g_tau)

        iw_mesh = MeshImFreq(beta, 'Fermion', 100)
        for iw in iw_mesh:
            ref = g_semicirc_iw(iw.value, D)
            self.assertAlmostEqual(g_c(iw), ref, delta=10*eps,
                                   msg=f"tau->dlr->iw failed at {iw.value}")

    def test_dlr_iw_to_tau(self):
        beta, eps, w_max, D = 100., 1e-10, 1., 1.

        g_iw = Gf(mesh=MeshDLRImFreq(beta, 'Fermion', w_max, eps), target_shape=[])
        g_iw << SemiCircular(D)
        g_c = make_gf_dlr(g_iw)

        test_taus = np.linspace(0, beta, 50)
        ref_tau = g_semicirc_tau(test_taus, beta, D)
        for j, tau in enumerate(test_taus):
            self.assertAlmostEqual(g_c(tau), ref_tau[j], delta=10*eps,
                                   msg=f"iw->dlr->tau failed at tau={tau}")

if __name__ == '__main__':
    unittest.main()
