/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
#include "./gf_classes.hpp"
#include "./domains/legendre.hpp"
#include "./meshes/discrete.hpp"

namespace triqs {
namespace gfs {

 struct legendre {};

 // mesh type and its factories
 template <> struct gf_mesh<legendre> : discrete_mesh<legendre_domain> {
  typedef discrete_mesh<legendre_domain> B;
  gf_mesh() = default;
  gf_mesh(double beta, statistic_enum S, size_t n_leg) : B(typename B::domain_t(beta, S, n_leg)) {}
 };


 template <> struct gf_h5_name<legendre, matrix_valued, nothing> {
  static std::string invoke() { return "Legendre"; }
 };
 template <int R> struct gf_h5_name<legendre, tensor_valued<R>, nothing> {
  static std::string invoke() { return "LegendreTv"+std::to_string(R); }
 };
 
 // If the data is real, write a real array, otherwise a complex array
 template <typename T, typename S, typename E> struct gf_h5_write_data<legendre, T, S, E> : gf_h5_write_data_real_or_complex_runtime{};
 
 /// ---------------------------  gf_evaluator ---------------------------------

 // Not finished, not tested
 template <> struct gf_evaluator<legendre, matrix_valued, nothing> {
  template <typename G> gf_evaluator(G*) {};
  static constexpr int arity = 1;
  template <typename G> arrays::matrix_view<dcomplex> operator()(G const& g, long n) const {
   return g.data()(n, arrays::range(), arrays::range());
  }
 };

}
}

