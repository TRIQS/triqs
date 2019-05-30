/*******************************************************************************
 * 
 * Copyright (C) 2015-2018, O. Parcollet
 * Copyright (C) 2019, The Simons Foundation
 *   author : N. Wentzell
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
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
