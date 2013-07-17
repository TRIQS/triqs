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
#ifndef TRIQS_H5_VECTOR_H
#define TRIQS_H5_VECTOR_H
#include "./group.hpp"
#include "./string.hpp"
#include <vector>

namespace triqs {

 inline std::string get_triqs_hdf5_data_scheme(std::vector<std::string> const & ) { return "vector<string>";}

 template <typename T>
  std::string get_triqs_hdf5_data_scheme(std::vector<T> const&) { 
   using triqs::get_triqs_hdf5_data_scheme;// for the basic types, not found by ADL
   std::stringstream fs; 
   fs<<"std::vector<"<<get_triqs_hdf5_data_scheme(T())<<">";
   return fs.str();
  } 

 namespace h5 {

  // special case of vector of string
  inline void h5_write (group f, std::string const & name, std::vector<std::string> const & V) {
   detail::write_1darray_vector_of_string_impl(f,name,V);
  }

  inline void h5_read (group f, std::string const & name, std::vector<std::string> & V) {
   detail::read_1darray_vector_of_string_impl(f,name,V);
  }

  // implementation for vector of double and complex
  namespace vector_impl { 

   // the dataspace corresponding to the array. Contiguous data only...
   template <typename T>
    H5::DataSpace data_space_for_vector (std::vector<T> const & V) { 
     mini_vector<hsize_t,1> L,S; S[0]=1;L[0] = V.size();
     return h5::dataspace_from_LS<1, boost::is_complex<T>::value > (L,L,S);
    }

   template<typename T>
    inline void h5_write_vector_impl (group g, std::string const & name, std::vector<T> const & V) {
     try {
      H5::DataSet ds = g.create_dataset(name, h5::data_type_file<T>(), data_space_for_vector(V) );
      ds.write( &V[0],  h5::data_type_memory<T>(), data_space_for_vector(V) );
      // if complex, to be python compatible, we add the __complex__ attribute
      if (boost::is_complex<T>::value)  h5::write_string_attribute(&ds,"__complex__","1");
     }
     TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
    }

   template<typename T>
    inline void h5_read_impl (group g, std::string const & name, std::vector<T> & V) {
     try {
      H5::DataSet ds = g.open_dataset(name);
      H5::DataSpace dataspace = ds.getSpace();
      static const unsigned int Rank =  1 + (boost::is_complex<T>::value ? 1 : 0);
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


  inline void h5_write (group f, std::string const & name, std::vector<double> const & V)          { vector_impl::h5_write_vector_impl(f,name,V);}
  inline void h5_write (group f, std::string const & name, std::vector<std::complex<double>> const & V) { vector_impl::h5_write_vector_impl(f,name,V);}

  inline void h5_read (group f, std::string const & name, std::vector<double>  & V)         { vector_impl::h5_read_impl(f,name,V);}
  inline void h5_read (group f, std::string const & name, std::vector<std::complex<double>> & V) { vector_impl::h5_read_impl(f,name,V);}

 }
} 
#endif


