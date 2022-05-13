// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2016 Igor Krivenko
// Copyright (c) 2018-2020 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Nils Wentzell, tayral

#pragma once
#include "brillouin_zone.hpp"
#include "../mesh/brzone.hpp"
#include "../gfs.hpp"
#include <itertools/itertools.hpp>
#include <h5/std_addons/complex.hpp>
#include <h5/h5.hpp>
#include <nda/linalg.hpp>

namespace triqs {
  namespace lattice {

    /**
   For tightbinding Hamiltonian with fully localised orbitals
   Overlap between orbital is taken as unit matrix.
  */
    class tight_binding {

      bravais_lattice bl_;
      std::vector<nda::vector<long>> displ_vec_;
      std::vector<matrix<dcomplex>> overlap_mat_vec_;

      public:
      /**
       * Construct a tight_binding Hamiltonian on a given bravais_lattice,
       * given the displacements in units of the lattice basis vectors (units)
       * and the associated overlap (hopping) matrices.
       * The matrix structure is w.r.t. the atoms in the unit cell.
       *
       * @param bl The underlying bravais lattice
       * @param displ_vec The vector of displacement vectors in units of the lattice basis vectors
       * @param overlap_mat_vec The vector of overlap (hopping) matrices
       */
      tight_binding(bravais_lattice bl, std::vector<nda::vector<long>> displ_vec, std::vector<matrix<dcomplex>> overlap_mat_vec);

      /// Underlying lattice
      bravais_lattice const &lattice() const { return bl_; }

      /// Return a vector containing all displacement vectors
      auto const &displ_vec() const { return displ_vec_; }

      /// Return a vector containing all overlap matrices
      auto const &overlap_mat_vec() const { return overlap_mat_vec_; }

      /// Transform into real coordinates.
      template <typename R> r_t lattice_to_real_coordinates(R const &x) const { return bl_.lattice_to_real_coordinates(x); }

      /// Number of orbitals / bands, i.e. size of the matrix t(k)
      int n_orbitals() const { return bl_.n_orbitals(); }

      // calls F(R, t(R)) for all R
      template <typename F> friend void foreach (tight_binding const &tb, F f) {
        int n = tb.displ_vec_.size();
        for (int i = 0; i < n; ++i) f(tb.displ_vec_[i], tb.overlap_mat_vec_[i]);
      }

      /**
       * Calculate the fourier transform for a given momentum vector k (or array of vectors)
       *
       *   $$ h_k = \sum_j m_j * exp(2 \pi i * \mathbf{k} * \mathbf{r}_j) $$
       *
       * with lattice displacements {r_j} and associated overlap (hopping) matrices {m_j}.
       * k needs to be represented in units of the reciprocal lattice vectors
       *
       * @param k The momentum vector (or an array thereof) in units of the reciprocal lattice vectors
       * @return The value for $h_k$ as a complex matrix
       */
      template <typename K>
      requires(nda::ArrayOfRank<K, 1> or nda::ArrayOfRank<K, 2>) auto fourier(K const &k) const {
        // Make sure to account for ndim in lattice
        auto k_ndim = make_regular(k(nda::ellipsis(), range(lattice().ndim())));

        auto vals = [&](int j) {
          if constexpr (nda::ArrayOfRank<K, 1>) {
            return std::exp(2i * M_PI * nda::blas::dot_generic(k_ndim, displ_vec_[j])) * overlap_mat_vec_[j];
          } else { // Rank==2
            auto k_mat = nda::make_matrix_view(k_ndim);
            auto exp   = [](auto d) { return std::exp(d); };
            auto exp_j = make_regular(nda::map(exp)(2i * M_PI * k_mat * displ_vec_[j]));
            return nda::blas::outer_product(exp_j, overlap_mat_vec_[j]);
          }
        };
        auto res = make_regular(vals(0));
        for (int i = 1; i < displ_vec_.size(); ++i) res += vals(i);
        return res;
      }

      /**
       * Calculate the fourier transform on a given k-mesh
       * and return the associated Green-function object
       *
       * @param k_mesh The brillouin-zone mesh
       * @return Green function on the k_mesh initialized with the fourier transform
       */
      inline auto fourier(mesh::brzone const &k_mesh) {
        auto kvecs = nda::matrix<double>(k_mesh.size(), 3);
        for (auto const &[n, k] : itertools::enumerate(k_mesh)) { kvecs(n, range()) = nda::vector<double>(k); }
        auto kvecs_rec = make_regular(kvecs * k_mesh.domain().reciprocal_matrix_inv());
        auto h_k       = gfs::gf<mesh::brzone, gfs::matrix_valued>(k_mesh, {n_orbitals(), n_orbitals()});
        h_k.data()     = fourier(kvecs_rec);
        return h_k;
      }

      /**
       * Calculate the fourier transform on a regular k-mesh
       * with n_l grid-points in each reciprocal direction.
       * Return the associated Green-function object.
       *
       * @param n_l The number of grid-points for each dimension
       * @return Green function on the k_mesh initialized with the fourier transform
       */
      inline auto fourier(int n_l) {
        auto k_mesh = mesh::brzone(brillouin_zone{bl_}, n_l);
        return fourier(k_mesh);
      }

      /**
       * Calculate the dispersion, i.e. the eigenvalue-spectrum of $h_k$,
       * for a given momentum vector k (or array of vectors).
       *
       * @param k The momentum vector (or an array thereof) in units of the reciprocal lattice vectors
       * @return The value for $h_k$ as a complex matrix
       */
      template <typename K>
      auto dispersion(K const &k) const requires(nda::ArrayOfRank<K, 1> or nda::ArrayOfRank<K, 2>) {
        if constexpr (nda::ArrayOfRank<K, 1>) {
          return nda::linalg::eigenvalues(fourier(k));
        } else { // Rank==2
          auto h_k = fourier(k);
          auto n_k = h_k.shape()[0];
          auto res = nda::array<double, 2>(n_k, n_orbitals());
          for (auto l : range(n_k)) res(l, range()) = nda::linalg::eigenvalues(h_k(l, nda::ellipsis()));
          return res;
        }
      }

      /**
       * Calculate the dispersion on a given k-mesh
       * and return the associated Green-function object
       *
       * @param k_mesh The brillouin-zone mesh
       * @return Green function on the k_mesh initialized with the dispersion values
       */
      inline auto dispersion(mesh::brzone const &k_mesh) {
        auto h_k = fourier(k_mesh);
        auto e_k = gfs::gf<mesh::brzone, gfs::tensor_real_valued<1>>(k_mesh, {n_orbitals()});
        for (auto const &k : k_mesh) e_k[k] = nda::linalg::eigenvalues(h_k[k]);
        return e_k;
      }

      /**
       * Calculate the dispersion on a regular k-mesh
       * with n_l grid-points in each reciprocal direction.
       * Return the associated Green-function object.
       *
       * @param n_l The number of grid-points for each dimension
       * @return Green function on the k_mesh initialized with the dispersion values
       */
      inline auto dispersion(int n_l) {
        auto k_mesh = mesh::brzone(brillouin_zone{bl_}, n_l);
        return dispersion(k_mesh);
      }

      // ------------------- Comparison -------------------

      bool operator==(tight_binding const &tb) const {
        return bl_ == tb.bl_ && overlap_mat_vec_ == tb.overlap_mat_vec_ && displ_vec_ == tb.displ_vec_;
      }

      bool operator!=(tight_binding const &tb) const { return !(operator==(tb)); }

      // -------------------- print -------------------

      friend std::ostream &operator<<(std::ostream &sout, tight_binding const &tb) {
        sout << "Tight Binding Hamiltonian on " << tb.lattice() << "\nwith hoppings [";
        for (auto const &[displ, overlap_mat] : itertools::zip(tb.displ_vec(), tb.overlap_mat_vec()))
          sout << "\n   " << displ << " : " << overlap_mat;
        return sout << " ]";
      }

      // ------------------- HDF5 Read / Write -------------------

      static std::string hdf5_format() { return "tight_binding"; }

      // Function that writes the solver_core to hdf5 file
      friend void h5_write(h5::group fg, std::string subgroup_name, tight_binding const &tb) {
        auto grp = fg.create_group(subgroup_name);
        write_hdf5_format(grp, tb);
        h5_write(grp, "bravais_lattice", tb.bl_);
        h5_write(grp, "displ_vec", tb.displ_vec_);
        h5_write(grp, "overlap_mat_vec", tb.overlap_mat_vec_);
      }

      // Function to read tight_binding object from hdf5 file
      CPP2PY_IGNORE
      static tight_binding h5_read_construct(h5::group g, std::string subgroup_name) {
        auto grp = g.open_group(subgroup_name);
        auto bl              = h5::h5_read<bravais_lattice>(grp, "bravais_lattice");
        auto displ_vec       = h5::h5_read<std::vector<nda::vector<long>>>(grp, "displ_vec");
        auto overlap_mat_vec = h5::h5_read<std::vector<matrix<dcomplex>>>(grp, "overlap_mat_vec");
        return tight_binding(bl, displ_vec, overlap_mat_vec);
      }

    }; // tight_binding

    std::pair<array<double, 1>, array<double, 2>> dos(tight_binding const &TB, int nkpts, int neps);
    std::pair<array<double, 1>, array<double, 1>> dos_patch(tight_binding const &TB, const array<double, 2> &triangles, int neps, int ndiv);

    [[deprecated("Use tight_binding member-function 'dispersion' instead")]] array<dcomplex, 3>
    hopping_stack(tight_binding const &TB, nda::array_const_view<double, 2> k_stack);

    [[deprecated("Use tight_binding member-function 'dispersion' instead")]] array<double, 2>
    energies_on_bz_path(tight_binding const &TB, k_t const &K1, k_t const &K2, int n_pts);

    [[deprecated("Use tight_binding member-function 'fourier' instead")]] array<dcomplex, 3>
    energy_matrix_on_bz_path(tight_binding const &TB, k_t const &K1, k_t const &K2, int n_pts);

    [[deprecated("Use tight_binding member-function 'dispersion' instead")]] array<double, 2> energies_on_bz_grid(tight_binding const &TB,
                                                                                                                            int n_pts);
  } // namespace lattice
} // namespace triqs
