// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell, tayral

#pragma once
#include "./brillouin_zone.hpp"
#include "./cluster_mesh.hpp"

namespace triqs {
  namespace gfs {

    using lattice::brillouin_zone;

    ///Mesh on Brillouin zone
    template <> struct gf_mesh<brillouin_zone> : public cluster_mesh {
      private:
      brillouin_zone bz;

      public:
      using var_t = brillouin_zone;

      gf_mesh() = default;

      /**
       * Construct a brzone mesh on a given brillouin zone
       * The underlying cluster-mesh will be constructed with the provided periodization matrix N.
       *
       * The unit vectors of the cluster-mesh are constructed to respect the periodicity of the
       * brillouin_zone, i.e.
       *
       *   $$K = N * U$$
       *
       * where $K$ is the reciprocal matrix and $U$ are the unit vectors of the cluster-mesh.
       *
       * @param bz The Brillouin zone (domain)
       * @param periodization_matrix Periodiziation matrix N of shape 3x3
       */
      gf_mesh(brillouin_zone const &bz_, matrix<int> const &periodization_matrix_)
         : bz(bz_), cluster_mesh(make_unit_matrix<double>(3), periodization_matrix_) {
        units = inverse(matrix<double>(periodization_matrix)) * bz.units();
      }

      /** 
       * Construct a brzone mesh on a given brillouin zone
       * with n_l mesh-points in each reciprocal direction
       * i.e. using a diagonal periodization matrix
       *
       * @param bz The Brillouin zone (domain)
       * @param n_l The number of grid-points for each dimension
       */
      gf_mesh(brillouin_zone const &bz_, int n_l)
         : gf_mesh(bz_, matrix<long>{{{n_l, 0, 0}, {0, bz_.lattice().dim() >= 2 ? n_l : 1, 0}, {0, 0, bz_.lattice().dim() >= 3 ? n_l : 1}}}) {}

      /// ----------- Model the mesh concept  ----------------------
      using domain_t    = brillouin_zone;
      using domain_pt_t = typename domain_t::point_t;

      domain_t const &domain() const { return bz; }

      // -------------- Evaluation of a function on the grid --------------------------

      static constexpr int n_pts_in_linear_interpolation = (1 << 3);

      interpol_data_lin_t<index_t, n_pts_in_linear_interpolation> get_interpolation_data(std::array<double, 3> const &k) const {

        // Calculate k in the units basis
        auto kv      = arrays::vector<double>{k[0], k[1], k[2]};
        auto k_units = inverse(transpose(units)) * kv;

        // 0----1----2----3----4----5---- : dim = 5, point 6 is 2 Pi
        std::array<std::array<long, 2>, 3> is;   // indices of the two neighbouring grid points
        std::array<std::array<double, 2>, 3> ws; // weights for the two neighbouring grid points
        for (int d = 0; d < 3; ++d) {
          long i   = std::floor(k_units[d]); // Take modulo later
          double w = k_units[d] - i;
          is[d]    = {i, i + 1};
          ws[d]    = {1 - w, w};
        }
        interpol_data_lin_t<index_t, n_pts_in_linear_interpolation> result;
        int c = 0;
        for (int ix = 0; ix < 2; ++ix)
          for (int iy = 0; iy < 2; ++iy)
            for (int iz = 0; iz < 2; ++iz) {
              result.idx[c] = index_modulo(index_t{is[0][ix], is[1][iy], is[2][iz]});
              result.w[c]   = ws[0][ix] * ws[1][iy] * ws[2][iz];
              c++;
            }
        return result;
      }

      template <typename F> auto evaluate(F const &f, std::array<double, 3> const &x) const { return multivar_eval(f, get_interpolation_data(x)); }

      // -------------- Evaluation of a function on the grid --------------------------

      /// Reduce index modulo to the lattice.
      index_t index_modulo(index_t const &r) const { return index_t{_modulo(r[0], 0), _modulo(r[1], 1), _modulo(r[2], 2)}; }

      interpol_data_0d_t<index_t> get_interpolation_data(index_t const &x) const { return {index_modulo(x)}; }

      template <typename F> auto evaluate(F const &f, index_t const &x) const {
        auto id = get_interpolation_data(x);
        return f[id.idx[0]];
      }

      // ------------------- Comparison -------------------

      bool operator==(gf_mesh<brillouin_zone> const &M) const { return bz == M.domain() && cluster_mesh::operator==(M); }

      bool operator!=(gf_mesh<brillouin_zone> const &M) const { return !(operator==(M)); }

      // -------------------- print -------------------

      friend std::ostream &operator<<(std::ostream &sout, gf_mesh const &m) {
        return sout << "Brillouin Zone Mesh with linear dimensions " << m.dims << "\n -- units = " << m.units
                    << "\n -- periodization_matrix = " << m.periodization_matrix << "\n -- Domain: " << m.domain();
      }

      friend class boost::serialization::access;
      template <class Archive> void serialize(Archive &ar, const unsigned int version) {
        ar &units;
        ar &periodization_matrix;
        ar &dims;
        ar &_size;
        ar &s2;
        ar &s1;
        ar &bz;
      }

      // -------------- HDF5  --------------------------

      static std::string hdf5_format() { return "MeshBrillouinZone"; }

      friend void h5_write(h5::group fg, std::string const &subgroup_name, gf_mesh const &m) {
        h5_write_impl(fg, subgroup_name, m, "MeshBrillouinZone");
        h5::group gr = fg.open_group(subgroup_name);
        h5_write(gr, "brillouin_zone", m.bz);
      }

      friend void h5_read(h5::group fg, std::string const &subgroup_name, gf_mesh &m) {
        h5_read_impl(fg, subgroup_name, m, "MeshBrillouinZone");
        h5::group gr = fg.open_group(subgroup_name);
        if (gr.has_key("bz")) {
          h5_read(gr, "bz", m.bz);
        } else if (gr.has_key("brillouin_zone")) {
          h5_read(gr, "brillouin_zone", m.bz);
        } else {
          std::cout << "WARNING: Reading old MeshBrillouinZone without BrillouinZone\n";
        }
      }
    };
  } // namespace gfs
} // namespace triqs
