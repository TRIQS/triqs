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
#include "./gf_classes.hpp"
#include "./meshes/segment.hpp"

namespace triqs {
namespace gfs {

 struct refreq {};

 template <> struct gf_mesh<refreq> : segment_mesh {
  template <typename... T> gf_mesh(T &&... x) : segment_mesh(std::forward<T>(x)...) {}
  // using segment_mesh::segment_mesh;
 };

 // singularity
 template <> struct gf_default_singularity<refreq, matrix_valued> {
  using type = tail;
 };
 template <> struct gf_default_singularity<refreq, scalar_valued> {
  using type = tail;
 };

 template <typename Singularity> struct gf_h5_name<refreq, matrix_valued, Singularity> {
  static std::string invoke() { return "ReFreq"; }
 };
 template <typename Singularity, int R> struct gf_h5_name<refreq, tensor_valued<R>, Singularity> {
  static std::string invoke() { return "ReFreqTv"+std::to_string(R); }
 };
}
}

