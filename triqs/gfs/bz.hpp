/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2013 by O. Parcollet
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
#include "../lattice/bz_mesh.hpp"
#include "./evaluators.hpp"

namespace triqs {
namespace gfs {

 struct bz {};

 template <typename Opt> struct gf_mesh<bz, Opt> : lattice::bz_mesh {
  template <typename... T> gf_mesh(T &&... x) : lattice::bz_mesh(std::forward<T>(x)...) {}
 };

 namespace gfs_implementation {

  // h5 name
  template <typename Singularity, typename Opt> struct h5_name<bz, matrix_valued, Singularity, Opt> {
   static std::string invoke() { return "BZ"; }
  };

  /// ---------------------------  data access  ---------------------------------
  template <typename Opt> struct data_proxy<bz, matrix_valued, Opt> : data_proxy_array<std::complex<double>, 3> {};
  template <typename Opt> struct data_proxy<bz, scalar_valued, Opt> : data_proxy_array<std::complex<double>, 1> {};

  /// ---------------------------  evaluator ---------------------------------

  // simple evaluation : take the point on the grid...
  template <> struct evaluator_fnt_on_mesh<bz> {
   size_t n;
   evaluator_fnt_on_mesh() = default;
   template <typename MeshType> evaluator_fnt_on_mesh(MeshType const &m, lattice::k_t const &k) { 
    n = m.locate_neighbours(k); // TO BE IMPROVED
   }
   template <typename F> auto operator()(F const &f) const DECL_AND_RETURN(f(n));
   //template <typename F> decltype(auto) operator()(F const &f) const { return f(n); }
  };

  // ------------- evaluator  -------------------
  // handle the case where the matsu. freq is out of grid...
  template <typename Target, typename Singularity, typename Opt> struct evaluator<bz, Target, Singularity, Opt> {
   static constexpr int arity = 1;

   template <typename G>
   std::c14::conditional_t<std::is_same<Target, matrix_valued>::value, arrays::matrix_const_view<std::complex<double>>,
                          std::complex<double>>
   operator()(G const *g, lattice::k_t const &k) const {
    auto n = g->mesh().locate_neighbours(k); // TO BE IMPROVED
    return (*g)[n];
   }
  };
 }
}
}

