// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
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

/**
 * @file
 * @brief Provides a forward iterator generating a regular grid of points in a cuboid.
 */

#pragma once

#include <itertools/itertools.hpp>
#include <nda/nda.hpp>

#include <iterator>

namespace triqs::lattice {

  /**
   * @addtogroup triqs-lattice
   * @{
   */

  /// Point type used by grid_generator: a real-valued vector (`nda::vector<double>`).
  using v_t = nda::vector<double>;

  /**
   * @brief Forward iterator generating uniformly-spaced points in the d-dimensional unit cube \f$ [0,1)^d \f$.
   *
   * @details For a given dimension \f$ d \in \{1,2,3\} \f$ and a number of points \f$ n \f$ per dimension, the 
   * generator iterates over the cuboid grid of \f$ n^d \f$ points
   * \f[
   *   \mathbf{p}_{(i_1,\dots,i_d)} = \left( \frac{i_1 + 1/2}{n}, \dots, \frac{i_d + 1/2}{n} \right) , \quad i_k \in 
   *   \{0,\dots,n-1\} \; ,
   * \f]
   * i.e. each point is shifted by half a step away from the cuboid faces so that the grid lies strictly inside the 
   * open unit cube. The iteration order is x (fastest), then y, then z (slowest).
   *
   * The generated points are always 3-dimensional. For \f$ d < 3 \f$ the trailing coordinates are kept at their
   * half-step value \f$ 1/2 \f$.
   *
   * Typical usage in C++ is via the implicit `bool` conversion and the increment operator inherited from
   * `itertools::iterator_facade`:
   * @code
   * for (auto gen = grid_generator{3, 10}; gen; ++gen) {
   *   nda::vector<double> const& k = *gen;
   *   // ... use k ...
   * }
   * @endcode
   */
  class grid_generator : public itertools::iterator_facade<grid_generator, v_t const, std::forward_iterator_tag> {
    friend struct itertools::iterator_facade<grid_generator, v_t const, std::forward_iterator_tag>;
    int dim{3}, nkpts{0}, nx{0}, ny{0}, nz{0}, N_X{0}, N_Y{0}, N_Z{0}, index_{0};
    double step_x{0.0}, step_y{0.0}, step_z{0.0};
    bool at_end{false};
    v_t pt;
    void init() {
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

    public:
    /**
     * @brief Construct a grid generator with the given dimension and number of points per dimension.
     *
     * @param dim Dimension of the grid (1, 2 or 3).
     * @param nkpts Number of points along each dimension.
     */
    grid_generator(int dim, int nkpts) : dim(dim), nkpts(nkpts), pt(3) { init(); }

    /// Default constructor: produces a 3-dimensional generator with zero points (an empty/end iterator).
    grid_generator() : pt(3) { init(); }

    /// Equal-to comparison: two generators are equal if they share the same dimension, number of points and index.
    bool operator==(grid_generator const &other) const { return ((other.dim == dim) && (other.index_ == index_) && (other.nkpts == nkpts)); }

    /// Total number of grid points, equal to \f$ N_X \cdot N_Y \cdot N_Z \f$ (trailing factors are 1 for `dim < 3`).
    int size() const { return (N_X * N_Y * N_Z); }

    /// Linear index of the current grid point in the iteration order (x fastest, then y, then z).
    int index() const { return index_; }

    /// Implicit conversion to `bool`: true while the generator has not yet been exhausted.
    operator bool() const { return !(at_end); }
  };

  /** @} */

} // namespace triqs::lattice
