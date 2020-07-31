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

#include "./common.hpp"

int main() {
  F1 f(7);

  TEST((x_ >> (2 * x_ + 1))(10));

  triqs::clef::function<double(double, double)> f2;
  f2(x_, y_) = x_ + y_;
  TEST(f2(2, 3));
  f2(x_, y_) << x_ + 2 * y_;
  TEST(f2(2, 3));

  std::function<double(double, double)> ff2(f2);
  TEST(ff2(2, 3));

  triqs::clef::function<double(double)> f1(f(x_) + 2 * x_, x_);
  TEST(f1(3));

  triqs::clef::function<double(double, double)> g2;
  g2(x_, y_) << x_ - y_ + f2(x_, 2 * y_);
  TEST(g2(2, 3));

  std::function<double(double, double)> ff8 = make_function(x_ + y_, x_, y_);
  TEST(ff8(2, 3));

  std::function<double(double)> f3;
  f3 = x_ >> f(x_) + 2 * x_;
  TEST(f3(3));

  auto h = make_function(2 * x_ + y_ + 1, x_);
  //  is a lazy expression expression with placeholder y_, returning a function...
  std::cout << tql::eval(h, y_ = 1) << std::endl;
  TEST(tql::eval(h, y_ = 1)(10));
  TEST(h);

  auto hh = var(x_, y_) >> 2 * x_ + y_;
  std::cout << hh(3, 1) << std::endl;
}
