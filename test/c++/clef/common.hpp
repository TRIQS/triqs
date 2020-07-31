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

#include <triqs/clef/clef.hpp>
#include <triqs/clef/io.hpp>
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> " << (X) << std::endl;

struct F1 {
  double v;
  F1(double v_) : v(v_) {}
  F1(F1 const &) = delete; // non copyable
  F1(F1 &&x) : v(x.v) { std::cerr << "Moving F1 " << v << std::endl; }

  double operator()(double x) const { return 10 * x; }

  TRIQS_CLEF_IMPLEMENT_LAZY_CALL(F1);

  template <typename Fnt> friend void triqs_clef_auto_assign(F1 &x, Fnt f) {
    x.v++;
    std::cerr << " called triqs_clef_auto_assign " << f(100) << std::endl;
  }

  friend std::ostream &operator<<(std::ostream &out, F1 const &x) { return out << "F1"; }
};

struct F2 {

  double v;
  F2() { v = 0; }

  double operator()(double x, double y) const { return 10 * x + y; }

  TRIQS_CLEF_IMPLEMENT_LAZY_CALL(F2);

  template <typename Fnt> friend void triqs_clef_auto_assign(F2 const &x, Fnt f) {
    std::cerr << " called F2 triqs_clef_auto_assign " << f(10, 20) << std::endl;
  }

  friend std::ostream &operator<<(std::ostream &out, F2 const &x) { return out << "F2"; }
};

using namespace triqs::clef;

triqs::clef::placeholder<1> x_;
triqs::clef::placeholder<2> y_;
triqs::clef::placeholder<3> z_;
namespace tql = triqs::clef;
