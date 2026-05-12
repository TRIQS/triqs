"""Test g_semicirc_tau panel quadrature against adaptive quadrature reference,
and SemiCircular descriptor on DLR imaginary-time meshes."""

import numpy as np
import unittest
from scipy.integrate import quad

from triqs.gfs import Gf, make_gf_dlr
from triqs.gfs.descriptors import SemiCircular
from triqs.gfs.semicirc import g_semicirc_tau, g_semicirc_tau_adapquad, g_semicirc_iw
from triqs.mesh import MeshDLRImTime, MeshDLRImFreq, MeshImFreq, MeshImTime, MeshReFreq


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

    def test_matrix_target_is_diagonal(self):
        """SemiCircular on matrix-valued Gfs must produce a diagonal with
        off-diagonals identically zero on every supported mesh."""
        beta, D = 10., 1.
        meshes = {
            'ImFreq':  MeshImFreq(beta, 'Fermion', 50),
            'ImTime':  MeshImTime(beta, 'Fermion', 101),
            'ReFreq':  MeshReFreq(w_min=-2., w_max=2., n_w=51),
        }
        for name, mesh in meshes.items():
            g_mat = Gf(mesh=mesh, target_shape=[2, 2])
            g_mat << SemiCircular(D)
            g_sca = Gf(mesh=mesh, target_shape=[])
            g_sca << SemiCircular(D)

            diag = np.einsum('nii->ni', g_mat.data)
            off = g_mat.data - np.einsum('ni,ij->nij', diag, np.eye(2))
            self.assertLess(np.max(np.abs(off)), 1e-14,
                            f"{name}: off-diagonals not zero")
            for i in range(2):
                np.testing.assert_allclose(g_mat.data[:, i, i], g_sca.data,
                                           err_msg=f"{name}: diagonal[{i}] differs from scalar")

class test_g_semicirc_iw_chem_potential(unittest.TestCase):
    """
    Tests for SemiCircular/g_semicirc_iw with non-zero chem_potential.
    """

    def _occupation_reference(self, D, mu, beta):
        """Numerically integrate n = integral rho(w) * f(w - mu) dw.

        G(z) = H(z + mu) = int rho(w) / (z + mu - w) dw, so by the
        Matsubara-sum identity the occupation is
            n = int rho(w) * f(w - mu) dw,  f(x) = 1/(1+exp(beta*x)).
        At T=0 this reduces to int_{-D}^{min(mu,D)} rho(w) dw.
        """
        rho = lambda w: (2.0 / (np.pi * D**2)) * np.sqrt(np.clip(D**2 - w**2, 0, None))
        fermi = lambda w: 1.0 / (1.0 + np.exp(beta * (w - mu)))
        n, _ = quad(lambda w: rho(w) * fermi(w), -D, D, limit=500, epsrel=1e-12)
        return n

    def _check_density(self, D, mu, beta, tol=1e-4):
        """Check that SemiCircular(D, mu) gives the correct occupation on MeshImFreq."""
        mesh = MeshImFreq(beta, 'Fermion', 1025)
        g = Gf(mesh=mesh, target_shape=[])
        g << SemiCircular(D, mu)
        n_gf = g.density().real
        n_ref = self._occupation_reference(D, mu, beta)
        self.assertAlmostEqual(n_gf, n_ref, delta=tol,
                               msg=f"D={D}, mu={mu}, beta={beta}: "
                                   f"n_gf={n_gf:.6f} vs n_ref={n_ref:.6f}")

    def _check_gf_values(self, D, mu, beta, n_iw_check=10, tol=1e-6):
        """Check G(iw_n + mu) values against the brute-force Hilbert transform."""
        mesh = MeshImFreq(beta, 'Fermion', 200)
        g = Gf(mesh=mesh, target_shape=[])
        g << SemiCircular(D, mu)

        # Reference: direct Hilbert transform via quad
        rho = lambda w: (2.0 / (np.pi * D**2)) * np.sqrt(np.clip(D**2 - w**2, 0, None))
        checked = 0
        for iw in mesh:
            if checked >= n_iw_check:
                break
            z = complex(iw) + mu
            g_ref_re, _ = quad(lambda w: rho(w) * (z.real - w) / ((z.real - w)**2 + z.imag**2),
                               -D, D, limit=500, epsrel=1e-10)
            g_ref_im, _ = quad(lambda w: rho(w) * (-z.imag) / ((z.real - w)**2 + z.imag**2),
                               -D, D, limit=500, epsrel=1e-10)
            g_ref = complex(g_ref_re, g_ref_im)
            g_val = complex(g[iw])
            self.assertAlmostEqual(g_val.real, g_ref.real, delta=tol,
                                   msg=f"Re G at iw={complex(iw):.4f}: got {g_val.real:.6f}, ref {g_ref.real:.6f}")
            self.assertAlmostEqual(g_val.imag, g_ref.imag, delta=tol,
                                   msg=f"Im G at iw={complex(iw):.4f}: got {g_val.imag:.6f}, ref {g_ref.imag:.6f}")
            checked += 1

    def test_density_mu_positive(self):
        """SemiCircular(D=2, mu=1): DOS center at -1, occupation ~0.8044."""
        self._check_density(D=2.0, mu=1.0, beta=40.0)

    def test_density_mu_large(self):
        """SemiCircular(D=1, mu=2): DOS fully below Fermi, occupation ~1."""
        self._check_density(D=1.0, mu=2.0, beta=40.0)

    def test_density_mu_zero(self):
        """SemiCircular(D=2, mu=0): symmetric DOS, occupation = 0.5."""
        self._check_density(D=2.0, mu=0.0, beta=40.0, tol=1e-6)

    def test_density_mu_negative(self):
        """SemiCircular(D=2, mu=-1): DOS center at +1, occupation ~0.1956."""
        self._check_density(D=2.0, mu=-1.0, beta=40.0)

    def test_density_symmetry(self):
        """Occupation(mu) + Occupation(-mu) = 1 by particle-hole symmetry."""
        D, beta = 2.0, 40.0
        for mu in [0.5, 1.0, 1.5]:
            mesh = MeshImFreq(beta, 'Fermion', 1025)
            gp = Gf(mesh=mesh, target_shape=[])
            gm = Gf(mesh=mesh, target_shape=[])
            gp << SemiCircular(D, mu)
            gm << SemiCircular(D, -mu)
            np = gp.density().real
            nm = gm.density().real
            self.assertAlmostEqual(np + nm, 1.0, delta=1e-4,
                                   msg=f"PH symmetry failed for mu={mu}: np+nm={np+nm:.6f}")

    def test_gf_values_mu_positive(self):
        """GF values for SemiCircular(D=2, mu=1) match Hilbert-transform reference."""
        self._check_gf_values(D=2.0, mu=1.0, beta=40.0)

    def test_gf_values_mu_negative(self):
        """GF values for SemiCircular(D=2, mu=-1) match Hilbert-transform reference."""
        self._check_gf_values(D=2.0, mu=-1.0, beta=40.0)


if __name__ == '__main__':
    unittest.main()
