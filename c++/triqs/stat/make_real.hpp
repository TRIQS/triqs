/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2017 by O. Parcollet, T. Ayral.
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

#include <type_traits>
#include <nda/nda.hpp>

namespace triqs::stat {

template <typename T> auto make_real(T &&t) { return std::forward<T>(t); }

// template <typename T> auto make_real(T &&t) requires(std::is_same_v<void, std::void_t<decltype(std::declval<T>().real())>>) { return t.real(); }

template <typename T> auto make_real(T &&t) requires(std::is_same_v<void, std::void_t<decltype(real(std::declval<T>()))>>) { return nda::make_regular(real(std::forward<T>(t))); }


template <typename T> using get_real_t = std::decay_t<decltype(make_real(std::declval<T>()))>;

} // namespace triqs::stat
