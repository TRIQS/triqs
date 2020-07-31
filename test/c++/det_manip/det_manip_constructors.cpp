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
// Authors: Laura Messio, Olivier Parcollet, Nils Wentzell

#include <triqs/det_manip/det_manip.hpp>

struct fun {

  typedef double result_type;
  typedef double argument_type;

  //gives the coefficients of the matrix (function F of the documentation)
  double operator()(double x, double y) const { return (x - y + 1) * (x - y); }
};

int main() {
  try {
    fun f;
    int init_size = 10;
    std::vector<double> initial_x{1, 2, 2.5}, initial_y{3, 4, 9};

    //creation of an empty class det_manip
    triqs::det_manip::det_manip<fun> D1(f, init_size);

    //creation of a class det_manip with a 3 by 3 matrix
    triqs::det_manip::det_manip<fun> D2(f, initial_x, initial_y);

    //the initial matrix:
    std::cout << std::endl << "After construction: D.matrix()=" << D1.matrix() << std::endl;
    std::cout << std::endl << "After construction: D.matrix()=" << D2.matrix() << std::endl;
  } catch (std::exception const &e) { std::cout << "error " << e.what() << std::endl; }
}
