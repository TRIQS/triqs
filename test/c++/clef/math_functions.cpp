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

#include "./common.hpp"
#include <triqs/clef/adapters/vector.hpp>

double foo(double x) { return x / 2; }
int foo(int x) { return x / 2; }

double bar(double x, double y) { return x + y; }

namespace triqs {
  namespace clef {

    using ::bar;
    using ::foo;

    template <typename T> typename std::enable_if<!triqs::clef::is_any_lazy<T>::value, T>::type inc(T const &x) { return x + 1; }
    // using ::inc;
    // moving the declaration up and using using:: does not work on gcc4.4
    // however not using using:: does not work on icc 11 !

    TRIQS_CLEF_MAKE_FNT_LAZY(bar);
    TRIQS_CLEF_MAKE_FNT_LAZY(inc);
    TRIQS_CLEF_MAKE_FNT_LAZY(foo);
  } // namespace clef
} // namespace triqs

namespace tql = triqs::clef;
int main() {
  using std::cout;
  using std::endl;
  TEST(tql::eval(cos(x_), x_ = 2));
  TEST(tql::eval(cos(2 * x_ + 1), x_ = 2));
  TEST(tql::eval(abs(2 * x_ - 1), x_ = 2));
  TEST(tql::eval(abs(2 * x_ - 1), x_ = -2));
  TEST(tql::eval(floor(2 * x_ - 1), x_ = 2.3));
  TEST(tql::eval(pow(2 * x_ + 1, 2), x_ = 2.0));
  TEST(tql::eval(foo(2 * x_ + 1), x_ = 2));
  TEST(tql::eval(foo(2 * x_ + 1), x_ = 2.0));
  TEST(tql::eval(bar(2 * x_ + 1, x_ - 1), x_ = 2));
  TEST(tql::eval(inc(2 * x_ + 1), x_ = 2));
}
