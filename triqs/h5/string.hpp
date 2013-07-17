/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
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
#ifndef TRIQS_H5_STRING_H
#define TRIQS_H5_STRING_H
#include "./group.hpp"
#include <cstring>
namespace triqs { 
 
 inline std::string get_triqs_hdf5_data_scheme(std::string const & ) { return "string";}
 
 namespace h5 {

 /**
  * \brief Write a string  into an hdf5 file
  * \param f The h5 file or group of type H5::H5File or H5::Group
  * \param name The name of the hdf5 array in the file/group where the stack will be stored
  * \param value The string
  * \exception The HDF5 exceptions will be caught and rethrown as TRIQS_RUNTIME_ERROR (with a full stackstrace, cf triqs doc).
  */
 inline void h5_write (group g, std::string const & name, std::string const & value) {
  try {
   H5::StrType strdatatype(H5::PredType::C_S1, value.size());
   H5::DataSet ds = g.create_dataset(name, strdatatype, H5::DataSpace());
   ds.write((void*)(value.c_str()), strdatatype );
  }
  TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
 }

 /**
  * \brief Read a string from an hdf5 file
  * \param f The h5 file or group of type H5::H5File or H5::Group
  * \param name The name of the hdf5 array in the file/group where the stack will be stored
  * \param value The string to fill
  * \exception The HDF5 exceptions will be caught and rethrown as TRIQS_RUNTIME_ERROR (with a full stackstrace, cf triqs doc).
  */
 inline void h5_read (group g, std::string const & name, std::string & value) {
  try {
   H5::DataSet ds = g.open_dataset(name);
   H5::DataSpace dataspace = ds.getSpace();
   int rank = dataspace.getSimpleExtentNdims();
   if (rank != 0) TRIQS_RUNTIME_ERROR << "Reading a string and got rank !=0";
   size_t size = ds.getStorageSize();
   H5::StrType strdatatype(H5::PredType::C_S1, size);
   std::vector<char> buf(size+1, 0x00);
   ds.read( (void *)(&buf[0]), strdatatype, H5::DataSpace(), H5::DataSpace() );
   value = ""; value.append( &(buf.front()) );
  }
  TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
 }


 /*********************************** 1d array and std::vector of string *********************************/
 namespace detail { 

  template<typename ArrayVectorOfStringType>
   void write_1darray_vector_of_string_impl (group g, std::string const & name, ArrayVectorOfStringType const & V) {
    size_t s=0; for (auto & x : V) s = std::max(s,x.size());
    try {
     H5::DataSet ds;
     H5::StrType strdatatype(H5::PredType::C_S1, s);
     const size_t n = V.size();
     std::vector<char> buf(n*(s+1), 0x00);
     size_t i=0; for (auto & x : V) {strcpy( &buf[i*s], x.c_str()); ++i;}

     mini_vector<hsize_t,1> L; L[0]=V.size();
     mini_vector<hsize_t,1> S; S[0]=1;
     auto d_space = dataspace_from_LS<1,false > (L,L,S);

     ds = g.create_dataset(name, strdatatype, d_space );
     ds.write( (void *)(&buf[0]),strdatatype, d_space );
    }
    TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
   }

  //------------------------------------------------------------

  template<typename ArrayVectorOfStringType>
   void read_1darray_vector_of_string_impl (group g, std::string const & name, ArrayVectorOfStringType & V) {
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

     mini_vector<hsize_t,1> L; L[0]=V.size();
     mini_vector<hsize_t,1> S; S[0]=1;
     auto d_space = dataspace_from_LS<1,false > (L,L,S);

     ds.read( (void *)(&buf[0]),strdatatype, d_space );
     size_t i=0; for (auto & x : V) { x = ""; x.append(&buf[i*(size)]); ++i;}
    }
    TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
   }
 }// detail

}}
#endif

