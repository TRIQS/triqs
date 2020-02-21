#!/usr/bin/env python

from __future__ import division
from __future__ import unicode_literals
from builtins import range
from past.utils import old_div
import unittest
import os
import sys

from pytriqs.gf import *
from pytriqs.gf.tools import *
from pytriqs.gf.gf_fnt import *
import numpy as np
from numpy.random import randn

class test_tail_issues(unittest.TestCase):

    def test_fit_iOmega_n(self):
        # Init Gf with iOmega_n
        g =  GfImFreq(indices = [0,1], beta = 10, n_points = 1000)
        g << iOmega_n

        # Check that tail fit error exceeds fourier error boundrary of 1e-2
        tail, err = g.fit_tail()
        self.assertTrue(err > 1e-2)

    def test_multi_fft(self):
        for eps in [0.001, 0.01, 0.1, 1, 10, 100]: #, 1000]:
            self.multi_fft(eps)

    def multi_fft(self, eps):
        # We have to adjust the number of frequencies
        # depending on beta * eps where eps is the largest energy-scale
        Nw = max(25, eps * 60)

        # We choose Ntau six times larger than Nw
        # to avoid boundrary effects
        Ntau = 6 * Nw + 1

        # Init Gf with Single Pole
        g = GfImFreq(indices = [0,1], beta = 1., n_points = Nw)
        g << inverse(iOmega_n + eps)
        g_ref = g.copy()

        # Iterate FFT and check violation of G_ij(iw) = G*_ji(-iw)
        it = 100
        gt = make_gf_from_fourier(g, Ntau)
        err = np.zeros(it)
        err_herm = np.zeros(it)
        for i in range(it):
            gt << Fourier(g)
            g << Fourier(gt)
            err[i] = np.max(np.abs(g.data - g_ref.data))
            err_herm[i] = np.linalg.norm(g.data[0,:,:] - np.transpose(np.conj(g.data[-1,:,:])))

        # print "err, err_herm : ", np.max(err), np.max(err_herm)
        self.assertTrue(np.max(err) < 1e-9)
        self.assertTrue(np.max(err_herm) < 1e-9)

    def test_exact_moments(self):
        for eps in [0.001, 0.01, 0.1, 1, 10, 100]: #, 1000]:
            self.exact_moments(eps)

    def exact_moments(self, eps):
        Nw = max(25, eps * 60)
        # Init G with Hermitian Hamiltonian
        g =  GfImFreq(indices = [0,1], beta = 1., n_points = Nw)
        H = eps * np.array([[1.0, 0.1j],[-0.1j, 2.0]])
        g << inverse(iOmega_n - H)

        max_norm = lambda x: np.max(np.abs(x))
        # Check error of tail coefficients
        tail, tail_err = g.fit_hermitian_tail()
        for n, tail_mom in enumerate(tail[1:4]):
            exact_mom = np.linalg.matrix_power(H, n)
            rel_err = old_div(max_norm(exact_mom-tail_mom), max_norm(exact_mom))
            # print "rel err ", rel_err
            self.assertTrue(rel_err < 1e-4)

        # Check error of tail coefficients imposing known moments
        km = make_zero_tail(g, 2)
        km[1] = np.eye(g.target_shape[0])
        tail, tail_err = g.fit_hermitian_tail(km)
        for n, tail_mom in enumerate(tail[1:4]):
            exact_mom = np.linalg.matrix_power(H, n)
            rel_err = old_div(max_norm(exact_mom-tail_mom), max_norm(exact_mom))
            # print "rel err ", rel_err
            self.assertTrue(rel_err < 1e-4)

    def test_imag_gt(self):
        # Init Gf with SemiCircular DOS
        g = GfImFreq(indices=[0], beta = 50, n_points = 2000)
        g << SemiCircular(1)

        # Check size of imaginary part of gt
        gt = make_gf_from_fourier(g)
        max_imag_Gtau = np.max(np.abs(gt.data.imag))
        self.assertTrue(max_imag_Gtau < 1e-11)

    def test_delta_infty(self):
        # Init G given H
        g =  GfImFreq(indices = [0,1], beta = 1, n_points = 1000)
        H = np.array([[0.0, 0.5],[0.5, 1.0]])
        g << iOmega_n - H + inverse(iOmega_n + 2)
        g.invert()

        # Extract H through tail-fit
        C = g.copy()
        C << inverse(g) - iOmega_n
        tail, err = fit_hermitian_tail(C)
        error_H = np.max(np.abs(tail[0] + H))
        # print error_H
        self.assertTrue(error_H < 1e-10)

        # Extract Delta(tau) and check imaginary part
        d = C.copy()
        d << C - tail[0]
        tail[0] = 0.
        tau_mesh = MeshImTime(beta = 1, S = "Fermion", n_max = 2001)
        dt = make_gf_from_fourier(d, tau_mesh, tail)
        max_im = np.max(np.abs(dt.data.imag))
        # print "Imag Delta", max_im
        self.assertTrue(max_im < 1e-12)

        # Check Delta(iw) extracted through pytriqs.gf.tools.delta
        D_exact = g.copy()
        D_exact << -1.0 * inverse(iOmega_n + 2)
        D = delta(g)
        max_diff = np.max(np.abs(D.data - D_exact.data))
        # print "Diff Delta Extraction", max_diff
        self.assertTrue(max_diff < 1e-10)


    def test_noisy_gf(self):
        for noise in [1e-4, 1e-3, 1e-2]:
            self.noisy_gf(noise)

    def noisy_gf(self, noise):
        # Init Gf using Flat Descriptor
        g = GfImFreq(indices = [0,1], beta = 1., n_points = 1000)
        H = np.array([[1.0, 0.1j],[-0.1j, 2.0]])
        g << inverse(iOmega_n - H)

        # Generate gt and add noise
        gt = make_gf_from_fourier(g)
        np.random.seed(666)
        gt.data[:] = gt.data + noise * ( randn(*np.shape(gt.data)) + 1j * randn(*np.shape(gt.data)) )

        # Resymmetrize gt
        gt.data[:,0,1] = gt.data[:,1,0].conjugate()

        # Fourier transform to Matsubara and back given the high-frequency information
        tail, err = g.fit_tail()
        gw = make_gf_from_fourier(gt, g.mesh, tail)
        gt2 = make_gf_from_fourier(gw, gt.mesh, tail)

        # Check that magnitude of error is of order noise
        err = np.max(np.abs(gt.data - gt2.data))
        # print "noise %.3e  err %.3e"%(noise, err)
        self.assertTrue(err < 10 * noise)

        # We can also go through a legendre basis to filter the noise
        gl = fit_legendre(gt)
        gw_from_leg = g.copy()
        gw_from_leg << LegendreToMatsubara(gl)
        gt3 = make_gf_from_fourier(gw_from_leg, len(gt.mesh))
        err = np.max(np.abs(gt.data - gt3.data))
        # print "noise %.3e  err %.3e"%(noise, err)
        self.assertTrue(err < 10 * noise)

        # from pytriqs.plot.mpl_interface import *
        # plt.subplot(2,1,1)
        # oplot(gt[0,1], name='gt')
        # gt_fit = gt.copy()
        # gt_fit << LegendreToMatsubara(gl)
        # oplot(gt_fit[0,1], name='gt_fit')
        # plt.subplot(2,1,2)
        # oplot(g[0,1], x_window=(-100,100), name='gw')
        # oplot(g_fit[0,1], x_window=(-100,100), name='gw_fit')
        # plt.show()

if __name__ == '__main__':
    unittest.main()
