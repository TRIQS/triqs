// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
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

#pragma once
#include "./brillouin_zone.hpp"
#include <itertools/itertools.hpp>

namespace triqs {
  namespace lattice {

    /**
  * Generate the point in a cuboid as an array<double,1> const &
  */
    class grid_generator : public itertools::iterator_facade<grid_generator, k_t const, std::forward_iterator_tag> {
      friend class itertools::iterator_facade<grid_generator, k_t const, std::forward_iterator_tag>;
      int dim, nkpts, nx, ny, nz, N_X, N_Y, N_Z, index_;
      double step_x, step_y, step_z;
      bool at_end;
      k_t pt;
      void init() {
        nx     = 0;
        ny     = 0;
        nz     = 0;
        at_end = false;
        index_ = 0;
        N_X    = nkpts;
        N_Y    = (dim > 1 ? nkpts : 1);
        N_Z    = (dim > 2 ? nkpts : 1);
        step_x = 1.0 / double(N_X);
        step_y = 1.0 / double(N_Y);
        step_z = 1.0 / double(N_Z);
        pt(0)  = step_x / 2;
        pt(1)  = step_y / 2;
        pt(2)  = step_z / 2;
      }

      void increment() {
        if (nx < N_X - 1) {
          ++nx;
          pt(0) += step_x;
          ++index_;
          return;
        }
        pt(0) = step_x / 2;
        nx    = 0;
        if (ny < N_Y - 1) {
          ++ny;
          pt(1) += step_y;
          ++index_;
          return;
        }
        pt(1) = step_y / 2;
        ny    = 0;
        if (nz < N_Z - 1) {
          ++nz;
          pt(2) += step_z;
          ++index_;
          return;
        }
        at_end = true;
      }

      value_type dereference() const { return pt; }
      bool equal(grid_generator const &other) const { return ((other.dim == dim) && (other.index_ == index_) && (other.nkpts == nkpts)); }

      public:
      /// dim : dimension, nkpts : number of k point in each dimension
      grid_generator(int dim_, int nkpts_) : dim(dim_), nkpts(nkpts_), pt(3) { init(); }
      grid_generator() : dim(3), nkpts(0), pt(3) { init(); }
      int size() const { return (N_X * N_Y * N_Z); }
      int index() const { return index_; }
      operator bool() const { return !(at_end); }
    };
  } // namespace lattice
} // namespace triqs
