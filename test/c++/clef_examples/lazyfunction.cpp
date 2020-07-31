// Copyright (c) 2013 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013 Centre national de la recherche scientifique (CNRS)
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
// Authors: Olivier Parcollet

#include <triqs/clef.hpp>
#include <iostream>
namespace tql = triqs::clef;

int main() {
  // two placeholders
  tql::placeholder<1> x_;
  tql::placeholder<2> y_;

  // a function of one variable
  triqs::clef::function<double(double)> h;
  h(x_) = 2 + 2 * x_;
  std::cout << h(3) << std::endl;

  // expression can be given at construction
  triqs::clef::function<double(double)> h2(2 + 2 * x_, x_);
  std::cout << h2(3) << std::endl;

  // two variable functions
  triqs::clef::function<double(double, double)> f, g;

  f(x_, y_) = x_ + y_;

  std::cout << f(2, 3) << std::endl;

  // affecting a new expression to f
  f(x_, y_) = x_ + 2 * y_;

  std::cout << f(2, 3) << std::endl;

  // f can be called with lazy expressions too to form more complex expressions.
  g(x_, y_) = x_ - y_ + f(x_, 2 * y_);

  std::cout << g(2, 3) << std::endl;

  // lazy function are interoperable with std::function
  std::function<double(double, double)> bf(f);
  std::cout << bf(2, 3) << std::endl;
}
