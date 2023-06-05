// Copyright (c) 2021 Simons Foundation
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
// Authors: Philipp Dumitrescu, Nils Wentzell

#pragma once

#include <nda/nda.hpp>

namespace triqs::stat {

  template <typename T> auto make_real(T &&t) { return nda::make_regular(nda::real(std::forward<T>(t))); }

  template <typename T> using get_real_t = decltype(make_real(std::declval<T>()));

} // namespace triqs::stat
