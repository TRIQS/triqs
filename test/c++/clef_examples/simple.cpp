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
#include <triqs/utility/macros.hpp>

namespace tql = triqs::clef;
// This macro just prints its argument literaly, and then its value

int main() {

  tql::placeholder<1> x_;
  tql::placeholder<2> y_;

  PRINT(x_ + 2 * y_);
  PRINT(eval(x_ + 2 * y_, x_ = 1, y_ = 2));
  PRINT(eval(x_ + 2 * y_, x_ = 1));
  PRINT(eval(x_ + 2 * y_, x_ = x_ + y_));
}
