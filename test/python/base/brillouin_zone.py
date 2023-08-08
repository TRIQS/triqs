# Copyright (c) 2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2016 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2019-2023 Simons Foundation
# Copyright (c) 2023 Hugo U.R. Strand
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
# Authors: Thomas Ayral, Hugo U. R. Strand, Nils Wentzell

import numpy as np
from numpy import eye, dot, linalg
from triqs.gf import *
from triqs.lattice import *
from triqs.lattice.lattice_tools import *
from math import pi

import unittest

class test_brillouin_zone(unittest.TestCase):

    def test_mesh_point_construction(self):

        # ---- Sr2RuO4 lattice : 3 x 3 x 3
        #units = np.array([[1.,0.,0.],[0.,1.,0.],[0.,0.,1.]])

        units = [[-1.9315,  1.9315,  6.362 ],
                 [ 1.9315, -1.9315,  6.362 ],
                 [ 1.9315,  1.9315, -6.362 ]]
        
        nk = 3
        dims = [nk]*3
        bl     = BravaisLattice(units)
        bz     = BrillouinZone(bl)
        k_mesh = MeshBrZone(bz, dims)
        
        tol = 1e-9
        
        for idx, (k, k_val) in enumerate(zip(k_mesh, k_mesh.values())):

            assert( max(abs(k.value - k_val)) < tol )
            
            a = idx // nk**2
            b = ( idx - nk**2 * a ) // nk
            c = idx - nk**2 * a - nk * b

            idx_vec = np.array([a, b, c])

            didx = k_mesh.to_data_index(idx_vec)
            assert( didx == idx )

            idx_vec_ref = k_mesh.to_index(didx)
            assert( max(abs(idx_vec - idx_vec_ref)) < tol )

            k_ref = idx_vec / nk @ bz.units
            assert( max(abs(k.value - k_ref)) < tol )


    def test_mesh_construction(self):
        # Helper function
        def run(units, dims):
            bl     = BravaisLattice(units)
            bz     = BrillouinZone(bl)
            k_mesh = MeshBrZone(bz, dims)

            k1, k2, k3 = bz.units
            assert np.array_equal(k_mesh.closest_index(k1), [0,0,0])
            assert np.array_equal(k_mesh.closest_index(k2), [0,0,0])
            assert np.array_equal(k_mesh.closest_index(k3), [0,0,0])
            assert linalg.norm(dot(bl.units, bz.units.T) - eye(3)*2*pi) < 1e-14

        # ---- square lattice : 16 x 16 x 1
        units = np.array([[1.,0.,0.],[0.,1.,0.],[0.,0.,1.]])
        dims = [16, 16, 1]
        run(units, dims)

        # ---- triangular lattice : 6 x 6 x 1
        units = np.array([[1.,0.,0.],[0.5,np.sqrt(3)/2.,0.],[0.,0.,1.]])
        dims = [6, 6, 1]
        run(units, dims)

        # ---- random ONB and dims
        np.random.seed(seed=12345)
        from scipy.stats import ortho_group
        units = ortho_group.rvs(3) # Random orthonormal 3x3 matrix
        dims = np.random.randint(1, 10, size=(3,))
        run(units, dims)

    def test_interpolate(self):
        # ---- Construct Brillouin Zone Meshes for a Triangular Bravais Lattice -----

        n_orb = 3
        units = np.array([[1.,0.],[0.5,np.sqrt(3)/2.]])
        bl    = BravaisLattice(units=units,
                               orbital_positions=[(0., 0)]*n_orb)
        bz=BrillouinZone(bl)

        n_k = 40
        dims = [n_k, n_k, 1]
        k_mesh  = MeshBrZone(bz, dims)

        U = linalg.inv(bz.units)
        in_rec_basis = lambda k: dot(k.value, U)

        n_k_fine = 5 * n_k
        dims = [n_k_fine, n_k_fine, 1]
        k_fine_mesh  = MeshBrZone(bz, dims)

        # ----- Interpolate Smooth Function with given derivative -----

        f = lambda k: np.cos(2*pi * in_rec_basis(k)[0])
        dfdk = lambda k: -2*pi * np.sin(2*pi * in_rec_basis(k)[0])
        max_dfdk = max(abs(dfdk(k)) for k in k_mesh)

        f_k = Gf(mesh=k_mesh, target_shape=[])
        for k in k_mesh: f_k[k] = f(k)

        # Evaluate on finer mesh
        for k in k_fine_mesh:
            assert np.allclose(f_k(k.value), f(k), atol=max_dfdk/n_k)

        # ----- Interpolate Dispersion -----

        # Hopping[Displacement on the lattice]
        t = 1.0
        tb = TightBinding(bl, {  (1,0)  :  -t*np.diag([1,2,3]),
                                 (-1,0) :  -t*np.diag([1,2,3]),
                                 (0,1)  :  -t*np.diag([1,2,3]),
                                 (0,-1) :  -t*np.diag([1,2,3])})


        e_k = Gf(mesh=k_mesh, target_shape=(n_orb,n_orb))

        k_vec_rec = np.array([in_rec_basis(k) for k in k_mesh])
        e_k.data[:] = tb.fourier(k_vec_rec)[:]

        max_adj_diff = np.amax(abs(np.diff(e_k.data[:], axis=0)))

        # Evaluate on finer mesh
        for k in k_fine_mesh:
            e_k_exact = tb.fourier(in_rec_basis(k))
            assert np.allclose(e_k(k.value), e_k_exact, atol=max_adj_diff)

        # ----- Lattice Green function -----

        mu, beta, n_iw = 0.1, 10.0, 1000
        iw_mesh = MeshImFreq(beta, 'Fermion', n_iw)
        k_iw_mesh = MeshProduct(k_mesh, iw_mesh)
        G_k_iw = Gf(mesh=k_iw_mesh, target_shape=(n_orb,n_orb))

        iw_vec = np.array([iw.value * eye(n_orb) for iw in iw_mesh])
        mu_mat = mu * eye(n_orb)
        G_k_iw.data[:] = linalg.inv(iw_vec[None,...] + mu_mat[None,None,...] - e_k.data[::,None,...])

        # Evaluate on finer mesh
        for n in [0,10,100]: # Matsubara Idx
            G_cur = G_k_iw[::, Idx(n)]
            max_adj_diff = np.amax(abs(np.diff(G_cur.data[:], axis=0)))
            for k in k_fine_mesh:
                Gk_exact = linalg.inv(iw_mesh(n) * eye(n_orb) + mu_mat - tb.fourier(in_rec_basis(k)))
                assert np.allclose(G_cur(k.value), Gk_exact, atol=max_adj_diff)


if __name__ == '__main__':
    unittest.main()
