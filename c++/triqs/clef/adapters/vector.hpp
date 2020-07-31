// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Olivier Parcollet, Nils Wentzell

#ifndef TRIQS_CLEF_VECTOR_ADAPTER_H
#define TRIQS_CLEF_VECTOR_ADAPTER_H
#include "../clef.hpp"
#include <vector>

namespace triqs {
  namespace clef {

    template <typename T, typename RHS> void triqs_clef_auto_assign__std_vector_impl(T &x, RHS &&rhs) { x = std::forward<RHS>(rhs); }

    template <typename Expr, int... Is, typename T> void triqs_clef_auto_assign__std_vector_impl(T &x, make_fun_impl<Expr, Is...> &&rhs) {
      triqs_clef_auto_assign_subscript(x, std::forward<make_fun_impl<Expr, Is...>>(rhs));
    }

    template <typename T, typename Fnt> void triqs_clef_auto_assign_subscript(std::vector<T> &v, Fnt f) {
      for (size_t i = 0; i < v.size(); ++i) triqs_clef_auto_assign__std_vector_impl(v[i], f(i));
    };

    template <typename T> std::ostream &triqs_clef_formal_print(std::ostream &out, std::vector<T> const &x) { return out << "std::vector"; }

  } // namespace clef
} // namespace triqs

#endif
