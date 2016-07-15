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
#include "./cluster_mesh.hpp"
#include "./bravais_lattice.hpp"

namespace triqs {
namespace gfs {
 using triqs::arrays::make_unit_matrix;
 using lattice::bravais_lattice;
 struct cyclic_lattice {};

 ///Mesh on real-space lattice 
 template <> struct gf_mesh<cyclic_lattice> : cluster_mesh { 
  bravais_lattice bl;

  public:

  ///full constructor
  /**
    @param bl_ bravais lattice
    @param periodization_matrix such that $\tilde{a}_i = \sum_j N_{ij} a_j$
   */
  gf_mesh(bravais_lattice const& bl_, matrix<int> const & periodization_matrix_)
     : bl(bl_), cluster_mesh{bl_.units(), periodization_matrix_ } {}

  ///backward compatibility (also serves as default const.)
  gf_mesh(int L1=1, int L2=1, int L3=1): bl(make_unit_matrix<double>(3)), cluster_mesh(make_unit_matrix<double>(3), matrix<int>({{L1, 0, 0},{0, L2, 0},{0, 0, L3}})){
  }

  using domain_t = bravais_lattice;
  domain_t const& domain() const { return bl; }

 };
}
}

