// Copyright (c) 2020 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet

#pragma once

namespace triqs::gfs {

  template <typename V, typename T, int A> auto fourier(block_gf<V, T, A> const &g) {
    return make_lazy_transform([](auto &&x) { return fourier(x); }, g);
  };

  template <typename V, typename T, int A> auto fourier(block_gf<V, T, A> &g) {
    return make_lazy_transform([](auto &&x) { return fourier(x); }, g);
  };

  template <typename V, typename T, int A, bool C> auto fourier(block_gf_view<V, T, A, C> g) {
    return make_lazy_transform([](auto &&x) { return fourier(x); }, g);
  };

} // namespace triqs::gfs
