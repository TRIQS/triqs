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
    //static_assert(N==0, "ERROR");

    if (v.is_empty()) return array<T, 2>{};
    auto a = rotate_index_view<N>(v);    // Swap relevant dim to front. The view is passed by value, we modify it.
    long nrows = a.extent(0);            // # rows of the result, i.e. n-th dim, which is now at 0.
    long ncols = a.size() / nrows;        // # columns of the result. Everything but n-th dim.
    array<T, 2> mat(nrows, ncols); // result

    auto a_0 = a(0, ellipsis()); 
    for (long n : range(nrows)) {
      if constexpr (R == 1)
        mat(n, 0) = a(n);
      else
        nda::for_each(a_0.indexmap().lengths(), [&a, &mat, n, c = 0ll](auto &&... i) mutable { mat(n, c++) = a(n, i...); });
    }
    return std::move(mat);
  }

  //-------------------------------------

  template <int N, typename... Ms, typename Target> auto flatten_gf_2d(gf_const_view<mesh::prod<Ms...>, Target> g) {
    auto const &m = std::get<N>(g.mesh());
    using gf_t    = gf<std::decay_t<decltype(m)>, tensor_valued<1>>;
    if constexpr (Target::is_real) // FIXME Remove hard-copy once real fourier is implemented
      return gf_t{
         m, array<dcomplex, 2>(flatten_2d<N>(make_array_const_view(g.data()))), {}}; // FIXME : all the make_array_const_view : fix flatten_2d
    else
      return gf_t{m, flatten_2d<N>(make_array_const_view(g.data())), {}};
  }

  template <int N, typename Mesh, typename Target> gf<Mesh, tensor_valued<1>> flatten_gf_2d(gf_const_view<Mesh, Target> g) {
    static_assert(N == 0, "Internal error");
    if constexpr (Target::is_real) // FIXME Remove hard-copy once real fourier is implemented
      return {g.mesh(), array<dcomplex, 2>(flatten_2d<0>(make_array_const_view(g.data()))), {}};
    else
      return {g.mesh(), flatten_2d<0>(make_array_const_view(g.data())), {}};
  }

} // namespace triqs::gfs
