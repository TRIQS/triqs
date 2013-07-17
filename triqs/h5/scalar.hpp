/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2013 by O. Parcollet
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
#ifndef TRIQS_H5_SCALAR_H
#define TRIQS_H5_SCALAR_H
#include "./group.hpp"
namespace triqs { namespace h5 {

 template <typename S> 
  typename std::enable_if<std::is_arithmetic<S>::value>::type
  h5_write (group g, std::string const & name, S const & A) {
   try {
    g.unlink_key_if_exists(name);
    H5::DataSet ds = g.create_dataset( name, data_type_file<S>(), H5::DataSpace() );
    ds.write( (void *)(&A), data_type_memory<S>(), H5::DataSpace() );
   }
   TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
  }

 template <typename S> 
  typename std::enable_if<std::is_arithmetic<S>::value>::type
  h5_read (group g, std::string const & name,  S & A) {
   try {
    H5::DataSet ds = g.open_dataset(name);
    H5::DataSpace dataspace = ds.getSpace();
    int rank = dataspace.getSimpleExtentNdims();
    if (rank != 0) TRIQS_RUNTIME_ERROR << "triqs::array::h5::read. Rank mismatch : expecting a scalar (rank =0)"
     <<" while the array stored in the hdf5 file has rank = "<<rank;
    ds.read( (void *)(&A), data_type_memory<S>(), H5::DataSpace() , H5::DataSpace() );
   }
   TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
  }

 // the complex number is missing here...
}}
#endif



