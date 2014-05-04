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

namespace triqs {
 namespace h5 {

  void h5_write (group g, std::string const & name, std::vector<std::string> const & V) {
    size_t s=0; for (auto & x : V) s = std::max(s,x.size());
    try {
     H5::DataSet ds;
     H5::StrType strdatatype(H5::PredType::C_S1, s);
     const size_t n = V.size();
     std::vector<char> buf(n*(s+1), 0x00);
     size_t i=0; for (auto & x : V) {strcpy( &buf[i*s], x.c_str()); ++i;}

     hsize_t L[1], S[1];
     L[0] = V.size();
     S[0] = 1;
     auto d_space = dataspace_from_LS(1,false,L,L,S);

     ds = g.create_dataset(name, strdatatype, d_space );
     ds.write( (void *)(&buf[0]),strdatatype, d_space );
    }
    TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
   }

  void h5_read (group g, std::string const & name, std::vector<std::string> & V) {
    try {
     H5::DataSet ds = g.open_dataset(name);
     H5::DataSpace dataspace = ds.getSpace();
     mini_vector<hsize_t,1> dims_out;
     int ndims = dataspace.getSimpleExtentDims( &dims_out[0], NULL);
     if (ndims !=1) TRIQS_RUNTIME_ERROR << "triqs::h5 : Trying to read 1d array/vector . Rank mismatch : the array stored in the hdf5 file has rank = "<<ndims;

     size_t Len = dims_out[0];
     V.resize(Len);
     size_t size = ds.getStorageSize();
     H5::StrType strdatatype(H5::PredType::C_S1, size);

     std::vector<char> buf(Len*(size+1), 0x00);

     hsize_t L[1], S[1];
     L[0] = V.size();
     S[0] = 1;
     auto d_space = dataspace_from_LS(1,false, L,L,S);

     ds.read( (void *)(&buf[0]),strdatatype, d_space );
     size_t i=0; for (auto & x : V) { x = ""; x.append(&buf[i*(size)]); ++i;}
    }
    TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
   }

  // implementation for vector of double and complex
  namespace {

   // the dataspace corresponding to the array. Contiguous data only...
   template <typename T> H5::DataSpace data_space_for_vector(std::vector<T> const &V) {
    hsize_t L[1], S[1];
    S[0] = 1;
    L[0] = V.size();
    return h5::dataspace_from_LS(1, triqs::is_complex<T>::value, L, L, S);
   }

   template<typename T>
    inline void h5_write_vector_impl (group g, std::string const & name, std::vector<T> const & V) {
     try {
      H5::DataSet ds = g.create_dataset(name, h5::data_type_file<T>(), data_space_for_vector(V) );
      ds.write( &V[0],  h5::data_type_memory<T>(), data_space_for_vector(V) );
      // if complex, to be python compatible, we add the __complex__ attribute
      if (triqs::is_complex<T>::value)  h5::write_string_attribute(&ds,"__complex__","1");
     }
     TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
    }

   template<typename T>
    inline void h5_read_impl (group g, std::string const & name, std::vector<T> & V) {
     try {
      H5::DataSet ds = g.open_dataset(name);
      H5::DataSpace dataspace = ds.getSpace();
      static const unsigned int Rank =  1 + (triqs::is_complex<T>::value ? 1 : 0);
      int rank = dataspace.getSimpleExtentNdims();
      if (rank != Rank) TRIQS_RUNTIME_ERROR << "triqs : h5 : read vector. Rank mismatch : the array stored in the hdf5 file has rank = "<<rank;
      mini_vector<hsize_t,Rank> dims_out;
      dataspace.getSimpleExtentDims( &dims_out[0], NULL);
      V.resize(dims_out[0]);
      ds.read( &V[0], h5::data_type_memory<T>(), data_space_for_vector(V) , dataspace );
     }
     TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
    }
  }// impl namespace


  void h5_write (group f, std::string const & name, std::vector<double> const & V)          { h5_write_vector_impl(f,name,V);}
  void h5_write (group f, std::string const & name, std::vector<std::complex<double>> const & V) { h5_write_vector_impl(f,name,V);}
  void h5_read (group f, std::string const & name, std::vector<double>  & V)         { h5_read_impl(f,name,V);}
  void h5_read (group f, std::string const & name, std::vector<std::complex<double>> & V) { h5_read_impl(f,name,V);}

 }
} 


