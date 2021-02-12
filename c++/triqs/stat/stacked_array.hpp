/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2018 by Simons Foundation
 *   author : O. Parcollet, H. Strand, P. Dumitrescu
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
#include <triqs/utility/itertools.hpp>
#include <type_traits>
#include <vector>

namespace nda {

  template <typename T, int N> array<T, N + 1> copy_and_push_back(array<T, N + 1> const &x, array_const_view<T, N> y) {
    auto sha = x.shape();
    sha[0]++;
    auto tmp                  = array<T, N + 1>(sha);
    auto R                    = range(0, x.shape()[0]);
    tmp(R, ellipsis())        = x; // copy the data
    tmp(R.last(), ellipsis()) = y;
    return tmp;
  }

  template <typename T, int N> struct stacked_array {
    array<T, N + 1> _data;

    stacked_array() = default;

    stacked_array(array<T, N + 1> &&d) : _data(std::move(d)) {}

    template <typename ArrayType> stacked_array(long n, ArrayType const &a) : _data(join(make_shape(n), a.shape())) { _data = 0; }

    array_view<T, N> operator[](long i) { return _data(i, ellipsis{}); }
    array_const_view<T, N> operator[](long i) const { return _data(i, ellipsis{}); }

    long size() const { return _data.shape()[0]; }

    // costly
    void push_back(array_const_view<T, N> x) { _data = copy_and_push_back(_data, x); }

    private:
    auto _make_iter(range::const_iterator it) const {
      return utility::details::make_transform_iter(it, [this](long n) { return (*this)[n]; });
    }

    public:
    // make it iterable
    auto begin() const { return _make_iter(std::cbegin(range(0, _data.shape()[0]))); }
    auto end() const { return _make_iter(std::cend(range(0, _data.shape()[0]))); }
    auto cbegin() const { return begin(); }
    auto cend() const { return end(); }

    friend stacked_array mpi_reduce(stacked_array const &a, mpi::communicator c, int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
      return {mpi_reduce(a._data, c, root, all, op)};
    }

    // FIXME redondant ...
    //friend stacked_array mpi_all_reduce(mpi::communicator c, stacked_array const &x) {
    //  return {mpi_all_reduce(c, x._data)};
    //}
  };

  template <typename T, int N> void h5_write(h5::group g, std::string const &name, stacked_array<T, N> const &x) { h5_write(g, name, x._data); }

  template <typename T, int N> void h5_read(h5::group g, std::string const &name, stacked_array<T, N> &x) { h5_read(g, name, x._data); }

} // namespace nda
