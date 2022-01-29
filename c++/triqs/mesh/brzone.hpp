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
     * Construct mesh on a Brillouin-zone
     *
     * The unit vectors $U$ of the cluster-mesh are constructed such that
     *
     *   $$K = N * U$$
     *
     * where $K$ is the reciprocal matrix and $N$ the periodization matrix.
     *
     * @param bz Brillouin zone (domain)
     * @param periodization_matrix Periodiziation matrix N of shape 3x3
     */
    brzone(brillouin_zone const &bz, matrix<long> const &periodization_matrix) : bz(bz), cluster_mesh(nda::eye<double>(3), periodization_matrix) {
      units_ = inverse(matrix<double>(periodization_matrix)) * bz.units();
    }

    /** 
     * Construct a brzone mesh on a given brillouin zone
     * with n_k mesh-points in each reciprocal direction
     * i.e. using a diagonal periodization matrix
     *
     * @param bz Brillouin zone (domain)
     * @param n_k Number of mesh-points in each reciprocal direction

     */
    brzone(brillouin_zone const &bz_, int n_k)
       : brzone(bz_, matrix<long>{{{n_k, 0, 0}, {0, bz_.lattice().ndim() >= 2 ? n_k : 1, 0}, {0, 0, bz_.lattice().ndim() >= 3 ? n_k : 1}}}) {}

    /// ----------- Model the mesh concept  ----------------------
    using domain_t    = brillouin_zone;
    using domain_pt_t = typename domain_t::point_t;

    domain_t const &domain() const { return bz; }

    // -------------- Evaluation of a function on the grid --------------------------

    /// Reduce index modulo to the lattice.
    index_t index_modulo(index_t const &r) const { return index_t{_modulo(r[0], 0), _modulo(r[1], 1), _modulo(r[2], 2)}; }

    // ------------------- Comparison -------------------

    bool operator==(brzone const &M) const { return bz == M.domain() && cluster_mesh::operator==(M); }

    bool operator!=(brzone const &M) const { return !(operator==(M)); }

    // -------------------- print -------------------

    friend std::ostream &operator<<(std::ostream &sout, brzone const &m) {
      return sout << "Brillouin Zone Mesh with linear dimensions " << m.dims() << "\n -- units = " << m.units()
                  << "\n -- periodization_matrix = " << m.periodization_matrix() << "\n -- Domain: " << m.domain();
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

  // -------------- Evaluation of a function on the grid --------------------------

  static constexpr int n_pts_in_linear_interpolation = (1 << 3);

  inline std::array<std::pair<brzone::linear_index_t, double>, n_pts_in_linear_interpolation> get_interpolation_data(brzone const &m,
                                                                                                                     std::array<double, 3> const &k) {

    // Calculate k in the units basis
    auto kv      = nda::vector_const_view<double>{{3}, k.data()};
    auto k_units = transpose(inverse(m.units())) * kv;

    // 0----1----2----3----4----5---- : dim = 5, point 6 is 2 Pi
    std::array<std::array<long, 2>, 3> is;   // indices of the two neighbouring grid points
    std::array<std::array<double, 2>, 3> ws; // weights for the two neighbouring grid points
    for (int d = 0; d < 3; ++d) {
      long i   = std::floor(k_units[d]); // Take modulo later
      double w = k_units[d] - i;
      is[d]    = {i, i + 1};
      ws[d]    = {1 - w, w};
    }
    std::array<std::pair<brzone::linear_index_t, double>, n_pts_in_linear_interpolation> result;
    int c = 0;
    for (int ix = 0; ix < 2; ++ix)
      for (int iy = 0; iy < 2; ++iy)
        for (int iz = 0; iz < 2; ++iz) {
          result[c] = std::make_pair(m.index_to_linear(m.index_modulo(brzone::index_t{is[0][ix], is[1][iy], is[2][iz]})), ws[0][ix] * ws[1][iy] * ws[2][iz]);
          c++;
        }
    return result;
  }

  inline std::array<std::pair<brzone::linear_index_t, one_t>, 1> get_interpolation_data(brzone const &m, brzone::index_t const &x) {
    return {std::pair<brzone::linear_index_t, one_t>{m.index_to_linear(m.index_modulo(x)), {}}};
  }

} // namespace triqs::mesh
