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
     * Construct a periodic Mesh on a BravaisLattice
     *
     * @param bl Object representing the underlying Bravais Lattice (domain)
     * @param N Periodization matrix (diagonal), shape=3x3
     */
    cyclat(bravais_lattice const &bl, matrix_const_view<long> N) : bl(bl), cluster_mesh{bl.units(), N} {}

    /**
     *  Construct a Mesh with on a BravaisLattice with periodicity L in each spacial direction
     *
     *  @param bl Object representing the underlying Bravais Lattice (domain)
     *  @param L Number of mesh-points in each spacial direction
     */
    cyclat(bravais_lattice const &bl, int L)
       : bl(bl), cluster_mesh{bl.units(), matrix<long>{{{L, 0, 0}, {0, bl.ndim() >= 2 ? L : 1, 0}, {0, 0, bl.ndim() >= 3 ? L : 1}}}} {}

    ///Construct  from three linear sizes assuming a cubic lattice (backward compatibility)
    cyclat(int L1 = 1, int L2 = 1, int L3 = 1)
       : bl{eye<double>(3)}, cluster_mesh{eye<double>(3), matrix<long>{{{L1, 0, 0}, {0, L2, 0}, {0, 0, L3}}}} {}

    using domain_t = bravais_lattice;
    domain_t const &domain() const { return bl; }

    // -------------- Evaluation of a function on the grid --------------------------



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

    inline std::array<std::pair<cyclat::linear_index_t, one_t>, 1> get_interpolation_data(cyclat const & m, cyclat::index_t const &x)  {
      return {std::pair<cyclat::linear_index_t, one_t>{m.index_to_linear(m.index_modulo(x)), {}}};
    }


} // namespace triqs::mesh
