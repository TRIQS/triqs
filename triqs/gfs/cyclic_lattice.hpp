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
#include "../lattice/cyclic_lattice.hpp"
#include "./evaluators.hpp"

namespace triqs {
namespace gfs {

 namespace gfs_implementation {

  // h5 name
  template <typename Singularity> struct h5_name<cyclic_lattice, matrix_valued, Singularity> {
   static std::string invoke() { return "R"; }
  };

  /// ---------------------------  data access  ---------------------------------
  template <> struct data_proxy<cyclic_lattice, matrix_valued> : data_proxy_array<std::complex<double>, 3> {};
  template <> struct data_proxy<cyclic_lattice, scalar_valued> : data_proxy_array<std::complex<double>, 1> {};

  /// ---------------------------  evaluator ---------------------------------

#ifndef TRIQS_CPP11
  // simple evaluation : take the point on the grid...
  template <> struct evaluator_of_clef_expression<cyclic_lattice> {
   using mesh_t = gf_mesh<cyclic_lattice>;
   private :
   auto __as_meshpt(mesh_t const &m, typename mesh_t::index_t const &r) { return m.modulo_reduce(r); }
   auto __as_meshpt(mesh_t const &m, mesh_point<mesh_t> const &r) { return r; }

   public:
   template <typename Expr, int N, typename Arg>
   auto operator()(Expr const &expr, clef::placeholder<N>, gf_mesh<cyclic_lattice> const &m, Arg const &k) {
    //return clef::eval(expr, clef::placeholder<N>() = __as_meshpt(m,k));
    return clef::eval(expr, clef::placeholder<N>() = no_cast(__as_meshpt(m,k)));
   }
  };
#endif

  // --------------------------------------------------------------
  template <typename Target, typename Singularity> struct evaluator<cyclic_lattice, Target, Singularity> {
   static constexpr int arity = 1;
   template <typename G> evaluator(G *) {};

   template <typename G> AUTO_DECL operator()(G const *g, typename gf_mesh<cyclic_lattice>::index_t const &r) const RETURN((*g)[g->mesh().modulo_reduce(r)]);
   template <typename G> AUTO_DECL operator()(G const *g, mesh_point<gf_mesh<cyclic_lattice>> const &r) const RETURN((*g)[r]);
   template <typename G> AUTO_DECL operator()(G const *g, __no_cast<mesh_point<gf_mesh<cyclic_lattice>>> const &r) const RETURN((*g)[r.value]);
  };
 }
}
}

