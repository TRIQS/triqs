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
#include "./vector.hpp"
#include "./base.hpp"

namespace triqs {
namespace h5 {

 void h5_write(group g, std::string const &name, std::vector<std::string> const &V) {
  size_t s = 0;
  for (auto &x : V) s = std::max(s, x.size());

  datatype strdatatype = H5Tcopy (H5T_C_S1);
  auto status = H5Tset_size (strdatatype, s);
  //auto status = H5Tset_size (strdatatype, H5T_VARIABLE);

  const size_t n = V.size();
  std::vector<char> buf(n * (s + 1), 0x00);
  size_t i = 0;
  for (auto &x : V) {
   strcpy(&buf[i * s], x.c_str());
   ++i;
  }

  hsize_t L[1], S[1];
  L[0] = V.size();
  S[0] = 1;
  auto d_space = dataspace_from_LS(1, false, L, L, S);

  h5::dataset ds = g.create_dataset(name, strdatatype, d_space);
  auto err = H5Dwrite(ds, strdatatype, d_space, H5S_ALL, H5P_DEFAULT, &buf[0]);
  if (err < 0) TRIQS_RUNTIME_ERROR << "Error writing the vector<string> " << name << " in the group" << g.name();
 }

 // ----- read -----
 void h5_read(group g, std::string const &name, std::vector<std::string> &V) {
  dataset ds = g.open_dataset(name);
  h5::dataspace d_space = H5Dget_space(ds);

  mini_vector<hsize_t, 1> dims_out;
  int ndims =  H5Sget_simple_extent_dims(d_space, dims_out.ptr(), NULL);
  if (ndims != 1) TRIQS_RUNTIME_ERROR
  << "triqs::h5 : Trying to read 1d array/vector . Rank mismatch : the array stored in the hdf5 file has rank = " << ndims;

  size_t Len = dims_out[0];
  V.resize(Len);
  size_t size = H5Dget_storage_size(ds);

  datatype strdatatype = H5Tcopy (H5T_C_S1);
  auto status = H5Tset_size (strdatatype, size);
  //auto status = H5Tset_size (strdatatype, H5T_VARIABLE);

  std::vector<char> buf(Len * (size + 1), 0x00);

  hsize_t L[1], S[1];
  L[0] = V.size();
  S[0] = 1;
  auto d_space2 = dataspace_from_LS(1, false, L, L, S);

  auto err = H5Dread(ds, strdatatype, d_space2, H5S_ALL, H5P_DEFAULT, &buf[0]);
  if (err < 0) TRIQS_RUNTIME_ERROR << "Error reading the vector<string> " << name << " in the group" << g.name();

  size_t i = 0;
  for (auto &x : V) {
   x = "";
   x.append(&buf[i * (size)]);
   ++i;
  }
 }

 // implementation for vector of double and complex
 namespace {

  // the dataspace corresponding to the array. Contiguous data only...
  template <typename T> dataspace data_space_for_vector(std::vector<T> const &V) {
   hsize_t L[1], S[1];
   S[0] = 1;
   L[0] = V.size();
   return h5::dataspace_from_LS(1, triqs::is_complex<T>::value, L, L, S);
  }

  //------------------------------------

  template <typename T> inline void h5_write_vector_impl(group g, std::string const &name, std::vector<T> const &V) {

   dataset ds = g.create_dataset(name, h5::data_type_file<T>(), data_space_for_vector(V));

   auto err = H5Dwrite(ds, h5::data_type_memory<T>(), data_space_for_vector(V), H5S_ALL, H5P_DEFAULT, &V[0]);
   if (err < 0) TRIQS_RUNTIME_ERROR << "Error writing the vector<....> " << name << " in the group" << g.name();

   // if complex, to be python compatible, we add the __complex__ attribute
   if (triqs::is_complex<T>::value) h5::write_string_attribute(ds, "__complex__", "1");
  }

  //------------------------------------

  template <typename T> inline void h5_read_impl(group g, std::string const &name, std::vector<T> &V) {

   dataset ds = g.open_dataset(name);
   h5::dataspace d_space = H5Dget_space(ds);

   static const unsigned int Rank = 1 + (triqs::is_complex<T>::value ? 1 : 0);
   int rank = H5Sget_simple_extent_ndims(d_space);
   if (rank != Rank)
    TRIQS_RUNTIME_ERROR << "triqs : h5 : read vector. Rank mismatch : the array stored in the hdf5 file has rank = " << rank;
   hsize_t dims_out[Rank];
   H5Sget_simple_extent_dims(d_space, dims_out, NULL);
   V.resize(dims_out[0]);

   auto err = H5Dread(ds, h5::data_type_memory<T>(), data_space_for_vector(V), d_space, H5P_DEFAULT, &V[0]);
   if (err < 0) TRIQS_RUNTIME_ERROR << "Error reading the vector<...> " << name << " in the group" << g.name();
  }
 } // impl namespace


 void h5_write(group f, std::string const &name, std::vector<int> const &V) { h5_write_vector_impl(f, name, V); }
 void h5_write(group f, std::string const &name, std::vector<double> const &V) { h5_write_vector_impl(f, name, V); }
 void h5_write(group f, std::string const &name, std::vector<std::complex<double>> const &V) { h5_write_vector_impl(f, name, V); }
 
 void h5_read(group f, std::string const &name, std::vector<int> &V) { h5_read_impl(f, name, V); }
 void h5_read(group f, std::string const &name, std::vector<double> &V) { h5_read_impl(f, name, V); }
 void h5_read(group f, std::string const &name, std::vector<std::complex<double>> &V) { h5_read_impl(f, name, V); }
}
}

