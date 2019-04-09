/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015-2018, O. Parcollet
 * Copyright (C) 2019, The Simons Foundation
 *   author : N. Wentzell
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once

#include <omp.h>

#include <itertools/itertools.hpp>

namespace itertools {

  /**
    * Function to chunk a range, distributing it uniformly over all OMP threads.
    *
    * This range-adapting function should be used inside an omp parallel region
    *
    * @tparam T The type of the range
    *
    * @param range The range to chunk
    */
  template <typename T> auto omp_chunk(T &&range) {
    auto total_size           = std::distance(std::cbegin(range), std::cend(range));
    auto [start_idx, end_idx] = chunk_range(0, total_size, omp_get_num_threads(), omp_get_thread_num());
    return itertools::slice(std::forward<T>(range), start_idx, end_idx);
  }
} // namespace itertools
