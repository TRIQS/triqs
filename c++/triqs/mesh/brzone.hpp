/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once
#include <triqs/lattice/brillouin_zone.hpp>
#include "./bases/cluster_mesh.hpp"

namespace triqs::mesh {

  using lattice::brillouin_zone;

  ///Mesh on Brillouin zone
  class brzone : public cluster_mesh {
    private:
    brillouin_zone bz;

    public:
    brzone() = default;

    /**
     * Constructs $$\tilde{b}_i = \sum_j N^{-1}_{ji} b_j$$ where $b_j$ reciprocal vectors
     *
     * @param bz_ brillouin zone
     * @param periodization_matrix such that $\tilde{a}_i = \sum_j N_{ij} a_j$
     *
     */
    brzone(brillouin_zone const &bz_, matrix<int> const &periodization_matrix_)
       : bz(bz_), cluster_mesh(nda::eye<double>(3), transpose(periodization_matrix_)) {
      matrix<double> N_as_double = periodization_matrix_;
      matrix<double> Nt_inv      = inverse(transpose(N_as_double));
      units_                     = Nt_inv * bz_.units();
    }

    /** 
     * Construct a brzone mesh on a given brillouin zone
     * with n_l mesh-points in each reciprocal direction
     * i.e. using a diagonal periodization matrix
     *
     * @param bz The Brillouin zone (domain)
     * @param n_l The number of grid-points for each dimension
     */
    brzone(brillouin_zone const &bz_, int n_l)
       : brzone(bz_, matrix<int>{{{n_l, 0, 0}, {0, bz_.lattice().ndim() >= 2 ? n_l : 1, 0}, {0, 0, bz_.lattice().ndim() >= 3 ? n_l : 1}}}) {}

    /// ----------- Model the mesh concept  ----------------------
    using domain_t    = brillouin_zone;
    using domain_pt_t = typename domain_t::point_t;

    domain_t const &domain() const { return bz; }

    // -------------- Evaluation of a function on the grid --------------------------

    static constexpr int n_pts_in_linear_interpolation = (1 << 3);

    // FIXME : INCORRECT
    std::array<std::pair<linear_index_t, double>, n_pts_in_linear_interpolation> get_interpolation_data(std::array<double, 3> const &x) const {

      // FIXME pass in the units of the reciprocal lattice
      // ONLY VALID for SQUARE LATTICE

      // 0----1----2----3----4----5 : dim = 5, point 6 is 2 Pi
      std::array<std::array<long, 3>, 2> ia;   // compute the neighbouring points ia, ja in all dimensions
      std::array<std::array<double, 3>, 2> wa; // compute the weight in all dimensions
      for (int u = 0; u < 3; ++u) {
        double delta_k = 2 * M_PI / this->dims_[u];
        //double a = (x[u] + M_PI)/delta_k; // if the grid would be centered on 0
        double a = (x[u]) / delta_k; // centered at pi
        long i   = std::floor(a);
        assert(i >= 0);
        assert(i <= this->dims_[u]);
        double w = a - i;
        ia[0][u] = i;
        ia[1][u] = _modulo(ia[0][u] + 1, u);
        wa[0][u] = 1 - w;
        wa[1][u] = w;
      }
      std::array<std::pair<linear_index_t, double>, n_pts_in_linear_interpolation> result;
      int c = 0;
      for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
          for (int k = 0; k < 2; ++k) {
            result[c] = std::make_pair(index_to_linear(index_modulo(index_t{ia[i][0], ia[j][1], ia[k][2]})), wa[i][0] * wa[j][1] * wa[k][2]);
            c++;
          }
      return result;
    }

    // -------------- Evaluation of a function on the grid --------------------------

    /// Reduce index modulo to the lattice.
    index_t index_modulo(index_t const &r) const { return index_t{_modulo(r[0], 0), _modulo(r[1], 1), _modulo(r[2], 2)}; }

    std::array<std::pair<linear_index_t, one_t>, 1> get_interpolation_data(index_t const &x) const {
      return {std::pair<linear_index_t, one_t>{index_to_linear(index_modulo(x)), {}}};
    }

    // ------------------- Comparison -------------------

    bool operator==(brzone const &M) const { return bz == M.domain() && cluster_mesh::operator==(M); }

    bool operator!=(brzone const &M) const { return !(operator==(M)); }

    // -------------------- print -------------------

    friend std::ostream &operator<<(std::ostream &sout, brzone const &m) {
      return sout << "Brillouin Zone Mesh with linear dimensions " << m.dims() << "\n -- units = " << m.units()
                  << "\n -- periodization_matrix = " << m.periodization_matrix() << "\n -- Domain: " << m.domain();
    }

    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, const unsigned int version) {
      ar &units_;
      ar &periodization_matrix_;
      ar &dims_;
      ar &size_;
      ar &stride0;
      ar &stride1;
      ar &bz;
    }

    // -------------- HDF5  --------------------------

    static std::string hdf5_format() { return "MeshBrillouinZone"; }

    friend void h5_write(h5::group fg, std::string const &subgroup_name, brzone const &m) {
      h5_write_impl(fg, subgroup_name, m, "MeshBrillouinZone");
      h5::group gr = fg.open_group(subgroup_name);
      h5_write(gr, "brillouin_zone", m.bz);
    }

    friend void h5_read(h5::group fg, std::string const &subgroup_name, brzone &m) {
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
} // namespace triqs::mesh
