/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2014 by O. Parcollet
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
#include "./scalar.hpp"
namespace triqs { namespace h5 {

 namespace {

 // TODO : Fix complex number ....
 
 // S must be scalar 
 template <typename S> 
  void h5_write_impl (group g, std::string const & name, S const & A) {
   try {
    g.unlink_key_if_exists(name);
    H5::DataSet ds = g.create_dataset( name, data_type_file<S>(), H5::DataSpace() );
    ds.write( (void *)(&A), data_type_memory<S>(), H5::DataSpace() );
   }
   TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
  }

 template <typename S> 
  void h5_read_impl (group g, std::string const & name,  S & A) {
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
 }

 void h5_write(group g, std::string const &name, int const &x) { h5_write_impl(g, name, x); }
 void h5_write(group g, std::string const &name, long const &x) { h5_write_impl(g, name, x); }
 void h5_write(group g, std::string const &name, size_t const &x) { h5_write_impl(g, name, x); }
 void h5_write(group g, std::string const &name, char const &x) { h5_write_impl(g, name, x); }
 void h5_write(group g, std::string const &name, bool const &x) { h5_write_impl(g, name, x); }
 void h5_write(group g, std::string const &name, double const &x) { h5_write_impl(g, name, x); }
 void h5_write(group g, std::string const &name, std::complex<double> const &x) { h5_write_impl(g, name, x); }


 void h5_read(group g, std::string const &name, int &x) { h5_read_impl(g, name, x); }
 void h5_read(group g, std::string const &name, long &x) { h5_read_impl(g, name, x); }
 void h5_read(group g, std::string const &name, size_t &x) { h5_read_impl(g, name, x); }
 void h5_read(group g, std::string const &name, char &x) { h5_read_impl(g, name, x); }
 void h5_read(group g, std::string const &name, bool &x) { h5_read_impl(g, name, x); }
 void h5_read(group g, std::string const &name, double &x) { h5_read_impl(g, name, x); }
 void h5_read(group g, std::string const &name, std::complex<double> &x) { h5_read_impl(g, name, x); }


}}



