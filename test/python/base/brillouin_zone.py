# Copyright (c) 2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2016 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2019-2020 Simons Foundation
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
# Authors: Nils Wentzell, tayral

from numpy import *
from numpy.linalg import *
from triqs.gf import *
from triqs.lattice import *
from triqs.lattice.lattice_tools import *
from math import pi

import unittest

class test_brillouin_zone(unittest.TestCase):

    def test_mesh_construction(self):

        # Helper function
        def run(units, per_mat):
            bl     = BravaisLattice(units)
            bz     = BrillouinZone(bl)
            k_mesh = MeshBrZone(bz, per_mat)

            p1, p2, p3 = per_mat
            k1, k2, k3 = bz.units
            assert array_equal(k_mesh.closest_index(k1), p1)
            assert array_equal(k_mesh.closest_index(k2), p2)
            assert array_equal(k_mesh.closest_index(k3), p3)
            assert norm(dot(bl.units, bz.units.T) - eye(3)*2*pi) < 1e-14

        # ---- square lattice : 16 x 16 x 1
        units = array([[1.,0.,0.],[0.,1.,0.],[0.,0.,1.]])
        per_mat = array([[16, 0, 0],[0, 16, 0],[0, 0, 1]])
        run(units, per_mat)

        # ---- triangular lattice : 6 x 6 x 1
        units = array([[1.,0.,0.],[0.5,sqrt(3)/2.,0.],[0.,0.,1.]])
        per_mat = array([[6, 0, 0],[0, 6, 0],[0, 0, 1]])
        run(units, per_mat)

        # ---- random ONB and per_mat
        random.seed(seed=12345)
        from scipy.stats import ortho_group
        units = ortho_group.rvs(3) # Random orthonormal 3x3 matrix
        per_mat = diag(random.randint(1, 10, size=(3,)))
        while linalg.det(per_mat) == 0:
            per_mat = diag(random.randint(1, 10, size=(3,)))
        run(units, per_mat)

    def test_interpolate(self):

        # ---- Construct Brillouin Zone Meshes for a Triangular Bravais Lattice -----

        n_orb = 3
        units = array([[1.,0.],[0.5,sqrt(3)/2.]])
        bl    = BravaisLattice(units=units,
                               orbital_positions=[(0., 0)]*n_orb)
        bz=BrillouinZone(bl)

        n_k = 40
        per_mat = array([[n_k, 0, 0],[0, n_k, 0],[0, 0, 1]])
        k_mesh  = MeshBrZone(bz, per_mat)

        U = linalg.inv(bz.units)
        in_rec_basis = lambda k: dot(k.value, U)

        n_k_fine = 5 * n_k
        per_mat = array([[n_k_fine, 0, 0],[0, n_k_fine, 0], [0, 0, 1]])
        k_fine_mesh  = MeshBrZone(bz, per_mat)

        # ----- Interpolate Smooth Function with given derivative -----

        f = lambda k: cos(2*pi * in_rec_basis(k)[0])
        dfdk = lambda k: -2*pi * sin(2*pi * in_rec_basis(k)[0])
        max_dfdk = max(abs(dfdk(k)) for k in k_mesh)

        f_k = Gf(mesh=k_mesh, target_shape=[])
        for k in k_mesh: f_k[k] = f(k)

        # Evaluate on finer mesh
        for k in k_fine_mesh:
            assert allclose(f_k(k.value), f(k), atol=max_dfdk/n_k)

        # ----- Interpolate Dispersion -----

        # Hopping[Displacement on the lattice]
        t = 1.0
        tb = TightBinding(bl, {  (1,0)  :  -t*diag([1,2,3]),
                                 (-1,0) :  -t*diag([1,2,3]),
                                 (0,1)  :  -t*diag([1,2,3]),
                                 (0,-1) :  -t*diag([1,2,3])})


        e_k = Gf(mesh=k_mesh, target_shape=(n_orb,n_orb))

        k_vec_rec = array([in_rec_basis(k) for k in k_mesh])
        e_k.data[:] = tb.fourier(k_vec_rec)[:]

        max_adj_diff = amax(abs(diff(e_k.data[:], axis=0)))

        # Evaluate on finer mesh
        for k in k_fine_mesh:
            e_k_exact = tb.fourier(in_rec_basis(k))
            assert allclose(e_k(k.value), e_k_exact, atol=max_adj_diff)

        # ----- Lattice Green function -----

        mu, beta, n_iw = 0.1, 10.0, 1000
        iw_mesh = MeshImFreq(beta, 'Fermion', n_iw)
        k_iw_mesh = MeshProduct(k_mesh, iw_mesh)
        G_k_iw = Gf(mesh=k_iw_mesh, target_shape=(n_orb,n_orb))

        iw_vec = array([iw.value * eye(n_orb) for iw in iw_mesh])
        mu_mat = mu * eye(n_orb)
        G_k_iw.data[:] = linalg.inv(iw_vec[None,...] + mu_mat[None,None,...] - e_k.data[::,None,...])

        # Evaluate on finer mesh
        for n in [0,10,100]: # Matsubara Idx
            G_cur = G_k_iw[::, Idx(n)]
            max_adj_diff = amax(abs(diff(G_cur.data[:], axis=0)))
            for k in k_fine_mesh:
                Gk_exact = linalg.inv(iw_mesh(n) * eye(n_orb) + mu_mat - tb.fourier(in_rec_basis(k)))
                assert allclose(G_cur(k.value), Gk_exact, atol=max_adj_diff)


if __name__ == '__main__':
    unittest.main()
