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
#include "./mpi.hpp"
#include <string>

namespace mpi {

  // ---------------- broadcast ---------------------
  inline void mpi_broadcast(std::string &s, communicator c, int root) {
    size_t len = s.size();
    broadcast(len, c, root);
    if (c.rank() != root) s.resize(len);
    if (len != 0) MPI_Bcast((void*)s.c_str(), s.size(), mpi_type<char>::get(), root, c.get());
  }

} // namespace mpi
