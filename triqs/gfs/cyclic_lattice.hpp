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

 struct cyclic_lattice {};

 template <typename Opt> struct gf_mesh<cyclic_lattice, Opt> : lattice::cyclic_lattice_mesh {
  template <typename... T> gf_mesh(T &&... x) : lattice::cyclic_lattice_mesh(std::forward<T>(x)...) {}
 };

 namespace gfs_implementation {

  // h5 name
  template <typename Singularity, typename Opt> struct h5_name<cyclic_lattice, matrix_valued, Singularity, Opt> {
   static std::string invoke() { return "R"; }
  };

  /// ---------------------------  data access  ---------------------------------
  template <typename Opt> struct data_proxy<cyclic_lattice, matrix_valued, Opt> : data_proxy_array<std::complex<double>, 3> {};
  template <typename Opt> struct data_proxy<cyclic_lattice, scalar_valued, Opt> : data_proxy_array<std::complex<double>, 1> {};

  /// ---------------------------  evaluator ---------------------------------

  // simple evaluation : take the point on the grid...
  template <> struct evaluator_fnt_on_mesh<cyclic_lattice> {
   size_t n;
   evaluator_fnt_on_mesh() = default;
   template <typename MeshType, typename R> evaluator_fnt_on_mesh(MeshType const &m, R const &r) { 
    n = m.modulo_reduce(r).linear_index(); 
   }
   template <typename F> AUTO_DECL operator()(F const &f) const RETURN(f(n));
   //template <typename F> decltype(auto) operator()(F const &f) const { return f(n); }
  };

  // --------------------------------------------------------------
  template <typename Target, typename Singularity, typename Opt> struct evaluator<cyclic_lattice, Target, Singularity, Opt> {
   static constexpr int arity = 1;

   template <typename G, typename R> auto operator()(G const *g, R const &r) const RETURN((*g)[g->mesh().modulo_reduce(r)]);

   //template <typename G, typename R> auto operator()(G const *g, R const &r) const {
   // auto n = g->mesh().modulo_reduce(r); 
   // return (*g)[n];
  // }
  };
 }
}
}

