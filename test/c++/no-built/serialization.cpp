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

#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include "./common.hpp"
#include <triqs/arrays/array.hpp>
#include <iostream>
#include <fstream>

using std::cout;
using std::endl;
using namespace triqs::arrays;

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

int main(int argc, char **argv) {

  array<long, 2> A(2, 3);

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) A(i, j) = 10 * i + j;

  array_view<long, 1> SL(A(0, range(0, 3)));
  std::cout << "A = " << A << std::endl;
  std::cout << "SLICE : A(0,range(0,3))  " << SL << std::endl;

  // create and open a character archive for output
  std::ofstream ofs("arch1");
  {
    boost::archive::text_oarchive oa(ofs);
    // write class instance to archive
    oa << A;
    oa << SL;
    // archive and stream closed when destructors are called
  }

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) A(i, j) = 0;
  std::cout << "A = " << A << std::endl;
  std::cout << "SLICE : A(0,range(0,3))  " << SL << std::endl;

  {
    // create and open an archive for input
    std::ifstream ifs("arch1");
    boost::archive::text_iarchive ia(ifs);
    // read class state from archive
    ia >> A >> SL;
    // archive and stream closed when destructors are called
  }

  std::cout << "A is " << A << std::endl;

  std::cout << "SLICE : A(0,range(0,3))  " << SL << std::endl;

  std::cout << "Setting A(0,0) =56 " << std::endl;
  SL(0) = 56;
  std::cout << "A is " << A << std::endl;
  std::cout << "SLICE : A(0,range(0,3))  " << SL << std::endl;
}
