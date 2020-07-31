// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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
#include <triqs/clef.hpp>

struct Obj {
  double v;                  // put something in it
  Obj(double v_) : v(v_) {}  // constructor
  Obj(Obj const &) = delete; // a non copyable object, to illustrate that we do NOT copy...

  // a method
  double my_method(double x) const { return 2 * x; }

  // CLEF overload
  TRIQS_CLEF_IMPLEMENT_LAZY_METHOD(Obj, my_method);

  // Just to print itself nicely in the expressions
  friend std::ostream &operator<<(std::ostream &out, Obj const &x) { return out << "Obj"; }
};

int main() {
  Obj f(7);
  triqs::clef::placeholder<1> x_;
  triqs::clef::placeholder<2> y_;

  std::cout << "Clef expression     : " << f.my_method(y_) + 2 * x_ << std::endl;
  std::cout << "Complete evaluation : " << eval(f.my_method(x_) + 2 * x_, x_ = 1) << std::endl;
  std::cout << "Partial evaluation  : " << eval(f.my_method(y_) + 2 * x_, y_ = 1) << std::endl;
  std::cout << "Complete evalution  : " << eval(f.my_method(y_) + 2 * x_, x_ = 3, y_ = 1) << std::endl;
}
