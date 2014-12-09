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
#include "./meshes/segment.hpp"
#include "./evaluators.hpp"

namespace triqs {
namespace gfs {

 struct retime {};

 template <> struct gf_mesh<retime> : segment_mesh {
  template <typename... T> gf_mesh(T &&... x) : segment_mesh(std::forward<T>(x)...) {}
 };

 // singularity
 template <> struct gf_default_singularity<retime, matrix_valued> {
  using type = tail;
 };
 template <> struct gf_default_singularity<retime, scalar_valued> {
  using type = tail;
 };

 namespace gfs_implementation {

  // h5 name
  template <typename Singularity> struct h5_name<retime, matrix_valued, Singularity> {
   static std::string invoke() { return "ReTime"; }
  };

  /// ---------------------------  evaluator ---------------------------------
  template <> struct evaluator_of_clef_expression<retime> : evaluator_grid_linear_interpolation {};

  template <typename Singularity, typename Target> struct evaluator<retime, Target, Singularity> : evaluator_one_var<retime> {
   template <typename G> evaluator(G *) {};
  };

  /// ---------------------------  data access  ---------------------------------
  template <> struct data_proxy<retime, matrix_valued> : data_proxy_array<std::complex<double>, 3> {};
  template <> struct data_proxy<retime, scalar_valued> : data_proxy_array<std::complex<double>, 1> {};

 } // gfs_implementation
}
}
