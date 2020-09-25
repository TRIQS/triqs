// Copyright (c) 2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016 Centre national de la recherche scientifique (CNRS)
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

#include <triqs/test_tools/gfs.hpp>

using namespace triqs::gfs;

TEST(Gf, SimpleAssign) {

  auto g7 = gf<prod<imtime, imtime>, tensor_valued<3>>{{{10.0, Fermion, 50}, {10.0, Boson, 50}}, {3, 3, 3}};
  auto g8 = gf<imtime>{{10.0, Fermion, 50}, {3, 3}};

  auto bg = make_block_gf({g7, g7, g7});

  for (int b = 0; b < 3; b++) {
    triqs::clef::placeholder<1> i_;
    triqs::clef::placeholder<2> j_;
    triqs::clef::placeholder<3> k_;
    triqs::clef::placeholder<4> l_;
    triqs::clef::placeholder<5> m_;

    g7(i_, j_)(k_, l_, m_) << 0.0;
    g7(i_, j_)(k_, l_, m_) << g8(i_)(k_, l_);
    g7(i_, j_)(k_, l_, m_) << g8(0)(l_, m_);
    g7(i_, j_)(k_, l_, m_) << g8(i_)(k_, l_) * g8(0)(l_, m_);
    g7(i_, j_)(k_, l_, m_) << g8(i_)(k_, l_) * g8(i_ - i_)(l_, m_);
    g7(i_, j_)(k_, l_, m_) << bg[b](i_, j_)(k_, l_, m_);
  }
}
MAKE_MAIN;
