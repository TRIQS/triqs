// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Michel Ferrero, Nils Wentzell

#include "./gf_view.hpp"
#include "gf.hpp"
namespace triqs::gfs {
  /**
   * Makes a copy of the array in matrix, whose first dimension is the n-th dimension of a
   * and the second dimension are the flattening of the other dimensions, in the original order
   *
   * @param a : array
   * @param n : the dimension to preserve.
   *
   * @return : a matrix, copy of the data
   * */
  template <int N, typename T, int R> array<T, 2> flatten_2d(array_const_view<T, R> v) {

    if (v.is_empty()) return array<T, 2>{};
    auto a     = nda::rotate_index_view<N>(v); // Swap relevant dim to front. The view is passed by value, we modify it.
    long nrows = a.extent(0);                  // # rows of the result, i.e. n-th dim, which is now at 0.
    long ncols = a.size() / nrows;             // # columns of the result. Everything but n-th dim.
    array<T, 2> out(nrows, ncols);             // result
    constexpr auto _ = ellipsis();

    if constexpr (R == 1)
      out(_, 0) = a(_);
    else
      for (long n : range(nrows)) {
        auto a_sl = a(n, _); // if the array is long, it is faster to precompute the view ...
        nda::for_each(a_sl.shape(), [&a_sl, &out, n, p = long(0)](auto &&...i) mutable { out(n, p++) = a_sl(i...); });
        //nda::for_each(a_0.indexmap().lengths(), [&a, &mat, n, c = 0ll](auto &&...i) mutable { mat(n, c++) = a(n, i...); });
      }
    return std::move(out);
  }

  // -------------------------------------------------------
  /// Inverse of flatten_2d
  template <int N, typename T, int R> array<T, 2> unflatten_2d(array_view<T, R> out, array_const_view<T, 2> flat_array) {
    auto a           = nda::rotate_index_view<N>(out);
    long nrows       = a.extent(0);      // # rows of the result, i.e. n-th dim, which is now at 0.
    long ncols       = a.size() / nrows; // # columns of the result. Everything but n-th dim.
    constexpr auto _ = ellipsis();

    if constexpr (R == 1)
      a(_, 0) = flat_array(_);
    else
      for (long n : range(nrows)) {
        auto a_sl = a(n, _); // if the array is long, it is faster to precompute the view ...
        nda::for_each(a_sl.shape(), [&a_sl, &flat_array, n, p = long(0)](auto &&...i) mutable { return a_sl(i...) = flat_array(n, p++); });
      }
  }

  //-------------------------------------

  template <int N, typename... Ms, typename Target> auto flatten_gf_2d(gf_const_view<mesh::prod<Ms...>, Target> g) {
    auto const &m = std::get<N>(g.mesh());
    using gf_t    = gf<std::decay_t<decltype(m)>, tensor_valued<1>>;
    if constexpr (Target::is_real)                                                        // FIXME Remove hard-copy once real fourier is implemented
      return gf_t{m, array<dcomplex, 2>(flatten_2d<N>(make_array_const_view(g.data())))}; // FIXME : all the make_array_const_view : fix flatten_2d
    else
      return gf_t{m, flatten_2d<N>(make_array_const_view(g.data()))};
  }

  //-------------------------------------

  template <int N, typename Mesh, typename Target> gf<Mesh, tensor_valued<1>> flatten_gf_2d(gf_const_view<Mesh, Target> g) {
    static_assert(N == 0, "Internal error");
    if constexpr (Target::is_real) // FIXME Remove hard-copy once real fourier is implemented
      return {g.mesh(), array<dcomplex, 2>(flatten_2d<0>(make_array_const_view(g.data())))};
    else
      return {g.mesh(), flatten_2d<0>(make_array_const_view(g.data()))};
  }

} // namespace triqs::gfs
