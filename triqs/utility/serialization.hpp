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

#if defined(TRIQS_SERIALIZATION_USE_BOOST) or not H5_VERSION_GE(1,8,9)
#include "./boost_serialization.hpp"

#define TRIQS_SERIALIZATION_WITH_BOOST
#else 
#include "../h5/serialization.hpp"
#define TRIQS_SERIALIZATION_WITH_HDF5

namespace triqs { 
 using h5::serialize;
 using h5::deserialize;
}
#endif
