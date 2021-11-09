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
#include <h5/std_addons/complex.hpp>
#include <h5/h5.hpp>

namespace triqs {
  namespace lattice {

    /**
   For tightbinding Hamiltonian with fully localised orbitals
   Overlap between orbital is taken as unit matrix.
  */
    class tight_binding {

      bravais_lattice bl_;
      std::vector<std::vector<long>> displ_vec_;
      std::vector<matrix<dcomplex>> overlap_mat_vec_;

      public:
      /** 
       * Construct a tight_binding Hamiltonian on a given bravais_lattice,
       * given the displacements {r_j} in units of the lattice basis vectors (units)
       * and the associated overlap (hopping) matrices {m_j}.
       *
       * @param bl The underlying bravais lattice
       * @param displ_vec The vector of displacement vectors in units of the lattice basis vectors
       * @param overlap_mat_vec The vector of overlap (hopping) matrices
       */
      tight_binding(bravais_lattice const &bl, std::vector<std::vector<long>> displ_vec, std::vector<matrix<dcomplex>> overlap_mat_vec);

      /// Underlying lattice
      bravais_lattice const &lattice() const { return bl_; }

      /// Number of bands, i.e. size of the matrix t(k)
      int n_bands() const { return bl_.n_orbitals(); }

      // calls F(R, t(R)) for all R
      template <typename F> friend void foreach (tight_binding const &tb, F f) {
        int n = tb.displ_vec_.size();
        for (int i = 0; i < n; ++i) f(tb.displ_vec_[i], tb.overlap_mat_vec_[i]);
      }

      // a simple function on the domain brillouin_zone
      struct fourier_impl {
        tight_binding const &tb;
        const int nb;

        using domain_t = brillouin_zone;
        brillouin_zone domain() const { return {tb.lattice()}; }

        TRIQS_CLEF_IMPLEMENT_LAZY_CALL();

        template <typename K> std::enable_if_t<!clef::is_clef_expression<K>, matrix<dcomplex>> operator()(K const &k) const {
          matrix<dcomplex> res(nb, nb);
          res() = 0;
          foreach (tb, [&](std::vector<long> const &displ, matrix<dcomplex> const &m) {
            double dot_prod = 0;
            int imax        = displ.size();
            for (int i = 0; i < imax; ++i) dot_prod += k(i) * displ[i];
            //double dot_prod = k(0) * displ[0] + k(1) * displ[1];
            res += m * exp(2i * M_PI * dot_prod);
          })
            ;
          return res;
        }
      };

      fourier_impl friend fourier(tight_binding const &tb) { return {tb, tb.n_bands()}; }

      // ------------------- Comparison -------------------

      bool operator==(tight_binding const &tb) const {
        return bl_ == tb.bl_ && overlap_mat_vec_ == tb.overlap_mat_vec_ && displ_vec_ == tb.displ_vec_;
      }

      bool operator!=(tight_binding const &tb) const { return !(operator==(tb)); }

      // ------------------- HDF5 Read / Write -------------------

      static std::string hdf5_format() { return "tight_binding"; }

      // Function that writes the solver_core to hdf5 file
      friend void h5_write(h5::group g, std::string subgroup_name, tight_binding const &tb) {
        auto grp = g.create_group(subgroup_name);
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
        auto displ_vec       = h5::h5_read<std::vector<std::vector<long>>>(grp, "displ_vec");
        auto overlap_mat_vec = h5::h5_read<std::vector<matrix<dcomplex>>>(grp, "overlap_mat_vec");
        return tight_binding(bl, displ_vec, overlap_mat_vec);
      }

    }; // tight_binding

    /**
   Factorized version of hopping (for speed)
   k_in[:,n] is the nth vector
   In the result, R[:,:,n] is the corresponding hopping t(k)
   */
    array<dcomplex, 3> hopping_stack(tight_binding const &TB, nda::array_const_view<double, 2> k_stack);
    // not optimal ordering here

    std::pair<array<double, 1>, array<double, 2>> dos(tight_binding const &TB, int nkpts, int neps);
    std::pair<array<double, 1>, array<double, 1>> dos_patch(tight_binding const &TB, const array<double, 2> &triangles, int neps, int ndiv);
    array<double, 2> energies_on_bz_path(tight_binding const &TB, k_t const &K1, k_t const &K2, int n_pts);
    array<dcomplex, 3> energy_matrix_on_bz_path(tight_binding const &TB, k_t const &K1, k_t const &K2, int n_pts);
    array<double, 2> energies_on_bz_grid(tight_binding const &TB, int n_pts);
  } // namespace lattice
} // namespace triqs
