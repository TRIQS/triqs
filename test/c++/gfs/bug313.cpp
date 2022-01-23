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

#include <triqs/gfs.hpp>

using namespace triqs::gfs;
using triqs::clef::placeholder;

int main() {
  double beta = 100.0;
  size_t n_iw    = 1025;
  double e    = -1.25;

  auto G_iw = gf<imfreq>{{beta, Fermion, n_iw}, {1, 1}};

  triqs::clef::placeholder<0> om_;
  G_iw(om_) << om_ - e;
  G_iw = inverse(G_iw);

  G_iw = make_real_in_tau(G_iw);
}
