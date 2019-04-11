/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by O. Parcollet
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
#include "./mpi.hpp"
#include <string>

namespace mpi {

  // ---------------- broadcast ---------------------
  inline void mpi_broadcast(std::string &s, communicator c, int root) {
    size_t len = s.size();
    broadcast(len, c, root);
    if (c.rank() != root) s.resize(len);
    if (len != 0) MPI_Bcast((void*)s.c_str(), s.size(), datatype<char>(), root, c.get());
  }

} // namespace mpi
