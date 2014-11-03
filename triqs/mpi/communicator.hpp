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
#include <triqs/utility/c14.hpp>
#include <mpi.h>

namespace boost { // forward declare in case we do not include boost.
namespace mpi {
 class communicator;
}
}

namespace triqs {
namespace mpi {

 /// Environment
 struct environment {
  // MPICH does not allow Init without argc, argv, so we do not allow default constructors
  // for portability, cf #133
  environment(int argc, char *argv[]) { MPI_Init(&argc, &argv); }
  ~environment() { MPI_Finalize(); }
 };

 // 
 inline bool is_initialized() noexcept { 
  int flag;
  MPI_Initialized(&flag);
  return flag;
 }

 /// The communicator. Todo : add more constructors.
 class communicator {
  MPI_Comm _com = MPI_COMM_WORLD;

  public:
  communicator() = default;

  MPI_Comm get() const { return _com; }

  inline communicator(boost::mpi::communicator);

  /// Cast to the boost mpi communicator
  inline operator boost::mpi::communicator() const;

  int rank() const {
   int num;
   MPI_Comm_rank(_com, &num);
   return num;
  }

  int size() const {
   int num;
   MPI_Comm_size(_com, &num);
   return num;
  }

  void barrier() const { MPI_Barrier(_com); }
 };
}
}
