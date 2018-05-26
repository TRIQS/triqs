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
#include "./base.hpp"
#include <boost/mpi.hpp>

namespace triqs {
  namespace mpi {

    // implement the communicator cast
    inline communicator::operator boost::mpi::communicator() const {
      return boost::mpi::communicator(_com, boost::mpi::comm_duplicate);
      // duplicate policy : cf http://www.boost.org/doc/libs/1_56_0/doc/html/boost/mpi/comm_create_kind.html
    }

    // reverse : construct (implicit) the communicator from the boost one.
    inline communicator::communicator(boost::mpi::communicator c) : _com(c) {}

  } // namespace mpi
} // namespace triqs
