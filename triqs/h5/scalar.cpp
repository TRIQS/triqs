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
#include "./base.hpp"

namespace triqs {
namespace h5 {

 namespace {

  // TODO : Fix complex number ....

  // S must be scalar
  template <typename S> void h5_write_impl(group g, std::string const &key, S a) {

   g.unlink_key_if_exists(key);

   dataspace space = H5Screate(H5S_SCALAR);
   auto ds = g.create_dataset(key, data_type_file<S>(), space);

   auto err = H5Dwrite(ds, data_type_memory<S>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, (void *)(&a));
   if (err < 0) TRIQS_RUNTIME_ERROR << "Error writing the scalar dataset " << key << " in the group" << g.name();
  }

  //-------------------------------------------------------------

  template <typename S> void h5_read_impl(group g, std::string const &name, S &A) {

   dataset ds = g.open_dataset(name);
   dataspace d_space = H5Dget_space(ds);

   // check that rank is 0, it is a scalar.
   int rank = H5Sget_simple_extent_ndims(d_space);
   if (rank != 0)
    TRIQS_RUNTIME_ERROR << "triqs::array::h5::read. Rank mismatch : expecting a scalar (rank =0)"
                        << " while the array stored in the hdf5 file has rank = " << rank;

   herr_t err = H5Dread(ds, data_type_memory<S>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, (void *)(&A));
   if (err < 0) TRIQS_RUNTIME_ERROR << "Error reading the scalar dataset " << name << " in the group" << g.name();
  }
 }

// template <> void h5_write_impl(group g, std::string const &key, std::complex<double> a);
// template <> void h5_read_impl(group g, std::string const &key, std::complex<double> &a);

 void h5_write(group g, std::string const &name, int const &x) { h5_write_impl(g, name, x); }
 void h5_write(group g, std::string const &name, long const &x) { h5_write_impl(g, name, x); }
 void h5_write(group g, std::string const &name, long long const &x) { h5_write_impl(g, name, x); }
 void h5_write(group g, std::string const &name, unsigned long long const &x) { h5_write_impl(g, name, x); }
 void h5_write(group g, std::string const &name, size_t const &x) { h5_write_impl(g, name, x); }
 void h5_write(group g, std::string const &name, char const &x) { h5_write_impl(g, name, x); }
 void h5_write(group g, std::string const &name, bool const &x) { h5_write_impl(g, name, x); }
 void h5_write(group g, std::string const &name, double const &x) { h5_write_impl(g, name, x); }
 void h5_write(group g, std::string const &name, std::complex<double> const &x) { h5_write_impl(g, name, x); }

 void h5_read(group g, std::string const &name, int &x) { h5_read_impl(g, name, x); }
 void h5_read(group g, std::string const &name, long &x) { h5_read_impl(g, name, x); }
 void h5_read(group g, std::string const &name, long long &x) { h5_read_impl(g, name, x); }
 void h5_read(group g, std::string const &name, unsigned long long &x) { h5_read_impl(g, name, x); }
 void h5_read(group g, std::string const &name, size_t &x) { h5_read_impl(g, name, x); }
 void h5_read(group g, std::string const &name, char &x) { h5_read_impl(g, name, x); }
 void h5_read(group g, std::string const &name, bool &x) { h5_read_impl(g, name, x); }
 void h5_read(group g, std::string const &name, double &x) { h5_read_impl(g, name, x); }
 void h5_read(group g, std::string const &name, std::complex<double> &x) { h5_read_impl(g, name, x); }
}
}

