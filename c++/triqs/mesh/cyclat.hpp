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
#include <triqs/lattice/bravais_lattice.hpp>
#include "./bases/cluster_mesh.hpp"

namespace triqs::mesh {

  using lattice::bravais_lattice;
  using nda::eye;

  ///
  class cyclat : public cluster_mesh {
    private:
    bravais_lattice bl;

    public:
    /**
     * @param bl_ bravais lattice
     * @param periodization_matrix such that $\tilde{a}_i = \sum_j N_{ij} a_j$
     */
    cyclat(bravais_lattice const &bl_, matrix_const_view<int> periodization_matrix_) : bl(bl_), cluster_mesh{bl_.units(), periodization_matrix_} {}

    ///Construct  from three linear sizes assuming a cubic lattice (backward compatibility)
    cyclat(int L1 = 1, int L2 = 1, int L3 = 1)
       : bl{eye<double>(3)}, cluster_mesh{eye<double>(3), matrix<int>{{{L1, 0, 0}, {0, L2, 0}, {0, 0, L3}}}} {}

    ///Construct from domain (bravais_lattice) and int L (linear size of Cluster mesh)
    cyclat(bravais_lattice const &bl_, int L)
       : bl(bl_), cluster_mesh{bl_.units(), matrix<int>{{{L, 0, 0}, {0, bl_.ndim() >= 2 ? L : 1, 0}, {0, 0, bl_.ndim() >= 3 ? L : 1}}}} {}

    using domain_t = bravais_lattice;
    domain_t const &domain() const { return bl; }

    // -------------- Evaluation of a function on the grid --------------------------

    std::array<std::pair<linear_index_t, one_t>, 1> get_interpolation_data(index_t const &x) const {
      return {std::pair<linear_index_t, one_t>{index_to_linear(index_modulo(x)), {}}};
    }

    // ------------------- Comparison -------------------

    bool operator==(cyclat const &M) const { return bl == M.domain() && cluster_mesh::operator==(M); }

    bool operator!=(cyclat const &M) const { return !(operator==(M)); }

    // -------------------- print -------------------

    friend std::ostream &operator<<(std::ostream &sout, cyclat const &m) {
      return sout << "Cyclic Lattice Mesh with linear dimensions " << m.dims() << "\n -- units = " << m.units()
                  << "\n -- periodization_matrix = " << m.periodization_matrix() << "\n -- Domain: " << m.domain();
    }

    // -------------- HDF5  --------------------------

    static std::string hdf5_format() { return "MeshCyclicLattice"; }

    friend void h5_write(h5::group fg, std::string const &subgroup_name, cyclat const &m) {
      h5_write_impl(fg, subgroup_name, m, "MeshCyclicLattice");
      h5::group gr = fg.open_group(subgroup_name);
      h5_write(gr, "bravais_lattice", m.bl);
    }

    friend void h5_read(h5::group fg, std::string const &subgroup_name, cyclat &m) {
      h5_read_impl(fg, subgroup_name, m, "MeshCyclicLattice");
      h5::group gr = fg.open_group(subgroup_name);
      try { // Care for Backward Compatibility
        h5_read(gr, "bl", m.bl);
        return;
      } catch (std::runtime_error const &re) {}
      try {
        h5_read(gr, "bravais_lattice", m.bl);
      } catch (std::runtime_error const &re) {}
    }
  };
} // namespace triqs::mesh
