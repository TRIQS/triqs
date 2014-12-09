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
#include "./tools.hpp"
#include "./gf.hpp"
#include "./local/tail.hpp"
#include "./domains/R.hpp"
#include "../lattice/regular_bz_mesh.hpp"
#include "./evaluators.hpp"

namespace triqs {
namespace gfs {

 namespace gfs_implementation {

  // h5 name
  template <typename Singularity> struct h5_name<brillouin_zone, matrix_valued, Singularity> {
   static std::string invoke() { return "BZ"; }
  };

  /// ---------------------------  data access  ---------------------------------
  template <> struct data_proxy<brillouin_zone, matrix_valued> : data_proxy_array<std::complex<double>, 3> {};
  template <> struct data_proxy<brillouin_zone, scalar_valued> : data_proxy_array<std::complex<double>, 1> {};

  /// ---------------------------  evaluator ---------------------------------

#ifndef TRIQS_CPP11
  // simple evaluation : take the point on the grid...
  template <> struct evaluator_of_clef_expression<brillouin_zone> {
   template <typename Expr, int N>
   auto operator()(Expr const &expr, clef::placeholder<N>, gf_mesh<brillouin_zone> const &m, lattice::k_t const &k) {
    auto n = m.locate_neighbours(k).index();
    return clef::eval(expr, clef::placeholder<N>() = no_cast(m[n]));
   }
  };
#endif

  // --------------------------------------------------------------
  template <typename Target, typename Singularity> struct evaluator<brillouin_zone, Target, Singularity> {
   static constexpr int arity = 1;
   template <typename G> evaluator(G *) {};
   template <typename G> auto operator()(G const *g, lattice::k_t const &k) const RETURN((*g)[g -> mesh().locate_neighbours(k)]);

   template <typename G> auto operator()(G const *g, __no_cast<typename gf_mesh<brillouin_zone>::mesh_point_t> const & p) const RETURN((*g)[p.value]);
   
  };
 }
}
}

