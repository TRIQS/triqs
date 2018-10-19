#!/usr/bin/env python

import unittest
import os
import sys

from pytriqs.gf import *
from pytriqs.gf.gf_fnt import *
import numpy as np

class test_tail_issues(unittest.TestCase):

    def test_fit_iOmega_n(self):
        # Init Gf with iOmega_n
        g =  GfImFreq(indices = [0,1], beta = 10, n_points = 1000)
        g << iOmega_n

        # Check that tail fit error exceeds fourier error boundrary of 1e-2
        tail, err = g.fit_tail()
        self.assertTrue(err > 1e-2)

    def test_multi_fft(self):
        # Init Gf with Single Pole
        g = GfImFreq(indices = [0,1], beta = 10, n_points = 1000)
        g << inverse(iOmega_n + 2.0)
        
        # Iterate FFT and check violation of G_ij(iw) = G*_ji(-iw)
        it = 100
        gt = make_gf_from_fourier(g, 10000)
        err = np.zeros(it)
        for i in range(it):
            gt << InverseFourier(g)
            g << Fourier(gt)
            err[i] = np.linalg.norm(g.data[0,:,:] - np.transpose(np.conj(g.data[-1,:,:])))
        self.assertTrue(np.max(err) < 1e-12)

    def test_exact_moments(self):
        # Init G with Hermitian Hamiltonian
        g =  GfImFreq(indices = [0,1], beta = 10, n_points = 1000)
        H = np.array([[1.0, 0.1j],[-0.1j, 2.0]])
        g << inverse(iOmega_n - H)

        # Check error of tail coefficients
        tail, err = g.fit_tail()
        for n, tail_mom in enumerate(tail[1:4]):
            exact_mom = np.linalg.matrix_power(H, n)
            err = np.max(np.abs(exact_mom-tail_mom))
            self.assertTrue(err < 1e-8)

        # Check error of hermitian tail coefficients
        tail, err = g.fit_hermitian_tail()
        for n, tail_mom in enumerate(tail[1:4]):
            exact_mom = np.linalg.matrix_power(H, n)
            err = np.max(np.abs(exact_mom-tail_mom))
            self.assertTrue(err < 1e-8)

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
        self.assertTrue(max_im < 1e-14)

    # def test_noisy_gf(self):
        # # Init Gf using Flat Descriptor
        # g = GfImFreq(indices = [0,1], beta = 10, n_points = 1000)
        # g[1, 1] << (4.0 * Flat(0.4) - 2.0 * Flat(0.2)) / 2.0
        # g[0, 0] << (8.0 * Flat(0.8) - 2.0 * Flat(0.2)) / 6.0

        # # Generate gt and add noise
        # gt = make_gf_from_fourier(g, 10000)
        # np.random.seed(666)
        # noise = 1e-6
        # gt.data[:] = gt.data + noise * np.random.randn(*np.shape(gt.data))

        # # Resymmetrize gt
        # gt.data[:,0,1] = gt.data[:,1,0].conjugate()

        # # FFT and InverseFFT
        # g << Fourier(gt)
        # gt2 = make_gf_from_fourier(g, 10000)

        # # Check magnitude of error
        # err = np.max(np.abs(gt.data - gt2.data))
        # # print "noise %.3e  err %.3e"%(noise, err)
        # self.assertTrue(err < 10 * noise)

if __name__ == '__main__':
    unittest.main()
