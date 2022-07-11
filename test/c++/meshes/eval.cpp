// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#include <triqs/mesh.hpp>
#include <triqs/test_tools/arrays.hpp>

using namespace triqs::mesh;

double beta = 10;

// a simple str such that * concatenate. Just to print the operations in the test
struct st { 
 std::string value;
};
st operator* (st const & a, st const & b){ auto r=  st{ a.value + " * " + b.value };  return r;}
st operator+ (st const & a, st const & b){ auto r=  st{ "(" + a.value + " + " + b.value + ")"}; return r;}
st operator* (double a, st const & b){ auto r = st{std::to_string(a) + " * " + b.value}; return r;}

auto f1 = [](auto &&x) { 
    std::stringstream fs; fs << " g["<< x<<  "]";
    return st{fs.str()};
  };

auto f2 = [](auto &&x, auto &&y) { 
    std::stringstream fs; fs << " g["<< x<< ", " << y << "]";
    return st{fs.str()};
 };

auto f3 = [](auto &&x, auto &&y, auto && z) { 
    std::stringstream fs; fs << " g["<< x<< ", " << y << ", "<< z<< "]";
    return st{fs.str()};
 };

// ---------------------------------

TEST(MeshEval, x_imtime) {
  auto m = imtime{beta, Fermion, 5};
 //for(auto x : m) std::cout  << x << std::endl;
{
auto r = evaluate(m, f1, 2);
//std::cout  << r.value << std::endl;
EXPECT_EQ(r.value, "(0.200000 *  g[0] + 0.800000 *  g[1])");
}
 {
auto r = evaluate(m, f1, 3.5);
//std::cout  << r.value << std::endl;
EXPECT_EQ(r.value, "(0.600000 *  g[1] + 0.400000 *  g[2])");
 }

{
auto r = evaluate(std::tuple{m,m}, f2, 2, 3.5);
//std::cout  << r.value << std::endl;
EXPECT_EQ(r.value, "(0.600000 * (0.200000 *  g[0, 1] + 0.800000 *  g[1, 1]) + 0.400000 * (0.200000 *  g[0, 2] + 0.800000 *  g[1, 2]))");
}

{
auto r = evaluate(std::tuple{m,m,m}, f3, 2, 3.5, 4.2);
//std::cout  << r.value << std::endl;
EXPECT_EQ(r.value, "(0.320000 * (0.600000 * (0.200000 *  g[0, 1, 1] + 0.800000 *  g[1, 1, 1]) + 0.400000 * (0.200000 *  g[0, 2, 1] + 0.800000 *  g[1, 2, 1])) + 0.680000 * (0.600000 * (0.200000 *  g[0, 1, 2] + 0.800000 *  g[1, 1, 2]) + 0.400000 * (0.200000 *  g[0, 2, 2] + 0.800000 *  g[1, 2, 2])))");
}

}
// ---------------------------

MAKE_MAIN;
