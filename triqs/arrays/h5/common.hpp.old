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
#ifndef TRIQS_ARRAYS_H5_COMMON_H
#define TRIQS_ARRAYS_H5_COMMON_H

#include <H5Cpp.h>
#include "hdf5_hl.h"
#include "../cache.hpp"
#include <boost/type_traits/is_complex.hpp>
#include <boost/utility/enable_if.hpp>

template<typename T> std::string get_triqs_hdf5_data_scheme(T const & ) { return "";}

namespace triqs { namespace arrays { namespace h5 { 
 using namespace H5;
 // conversion of C type to HDF5 native
 inline PredType native_type_from_C(char)               { return PredType::NATIVE_CHAR; }
 inline PredType native_type_from_C(signed char)        { return PredType::NATIVE_SCHAR; }
 inline PredType native_type_from_C(unsigned char)      { return PredType::NATIVE_UCHAR; }
 inline PredType native_type_from_C(short)              { return PredType::NATIVE_SHORT; }
 inline PredType native_type_from_C(unsigned short)     { return PredType::NATIVE_USHORT; }
 inline PredType native_type_from_C(int)                { return PredType::NATIVE_INT; }
 inline PredType native_type_from_C(unsigned)           { return PredType::NATIVE_UINT; }
 inline PredType native_type_from_C(long)               { return PredType::NATIVE_LONG; }
 inline PredType native_type_from_C(unsigned long)      { return PredType::NATIVE_ULONG; }
 inline PredType native_type_from_C(long long)          { return PredType::NATIVE_LLONG; }
 inline PredType native_type_from_C(unsigned long long) { return PredType::NATIVE_ULLONG; }
 inline PredType native_type_from_C(float)              { return PredType::NATIVE_FLOAT; }
 inline PredType native_type_from_C(double)             { return PredType::NATIVE_DOUBLE; }
 inline PredType native_type_from_C(long double)        { return PredType::NATIVE_LDOUBLE; }
 inline PredType native_type_from_C(bool)               { return PredType::NATIVE_SCHAR; }
 inline PredType native_type_from_C(std::string)        { return PredType::C_S1; }

 // conversion of C type to HDF5 native
 // NEED TO CHANGE THIS ? It is not standard... We should fix a standard or have a trait 
 inline PredType h5_type_from_C(char)               { return PredType::NATIVE_CHAR; }
 inline PredType h5_type_from_C(signed char)        { return PredType::NATIVE_SCHAR; }
 inline PredType h5_type_from_C(unsigned char)      { return PredType::NATIVE_UCHAR; }
 inline PredType h5_type_from_C(short)              { return PredType::NATIVE_SHORT; }
 inline PredType h5_type_from_C(unsigned short)     { return PredType::NATIVE_USHORT; }
 inline PredType h5_type_from_C(int)                { return PredType::NATIVE_INT; }
 inline PredType h5_type_from_C(unsigned)           { return PredType::NATIVE_UINT; }
 inline PredType h5_type_from_C(long)               { return PredType::NATIVE_LONG; }
 inline PredType h5_type_from_C(unsigned long)      { return PredType::NATIVE_ULONG; }
 inline PredType h5_type_from_C(long long)          { return PredType::NATIVE_LLONG; }
 inline PredType h5_type_from_C(unsigned long long) { return PredType::NATIVE_ULLONG; }
 inline PredType h5_type_from_C(float)              { return PredType::NATIVE_FLOAT; }
 inline PredType h5_type_from_C(double)             { return PredType::NATIVE_DOUBLE; }
 inline PredType h5_type_from_C(long double)        { return PredType::NATIVE_LDOUBLE; }
 inline PredType h5_type_from_C(bool)               { return PredType::NATIVE_SCHAR; }
 inline PredType h5_type_from_C(std::string)        { return PredType::C_S1; }

 // If it is complex<T> return T else T
 template<typename T> struct remove_complex { typedef T type;};
 template<typename T> struct remove_complex<std::complex<T> > { typedef T type;};

 template <typename ArrayType >
  PredType data_type_mem ( ArrayType const & A) { 
   return native_type_from_C(typename remove_complex<typename ArrayType::value_type>::type());
  }

 template <typename S >
  PredType data_type_mem_scalar ( S const & A) { 
   return native_type_from_C(typename remove_complex<S>::type());
  }

 // the type of data to put in the file_or_group
 template <typename V >
  PredType data_type_file ( V const & ) { return h5_type_from_C(typename remove_complex<V>::type());}

 // the pointer on the start of data
 template <typename ArrayType > 
  TYPE_DISABLE_IF ( void *, boost::is_complex<typename ArrayType::value_type> )
  data ( ArrayType const & A) { return &(A.storage()[0]);}

 template <typename ArrayType > 
  TYPE_ENABLE_IF ( void *, boost::is_complex<typename ArrayType::value_type> )
  data ( ArrayType const & A) {
   typedef typename ArrayType::value_type::value_type T; 
   std::complex<T> * p =  &(A.storage()[0]);
   return reinterpret_cast<T*>(p);
  }

 // dataspace from lengths and strides. Correct for the complex. strides must be >0
 template<int R, bool IsComplex> 
  H5::DataSpace dataspace_from_LS (  
    mini_vector<hsize_t, R> const & Ltot, 
    mini_vector<hsize_t, R> const & L, 
    mini_vector<hsize_t, R> const & S, 
    mini_vector<hsize_t, R> const & offset = mini_vector<hsize_t,R>() ) { 
#ifdef TRIQS_ARRAYS_DEBUG_H5_SLICE
   std::cerr  << "total lens in dataspace_from_LS"<< Ltot.to_string() << std::endl ;
   std::cerr  << "lens in dataspace_from_LS"<< L.to_string() << std::endl ;
   std::cerr  << "Strides in dataspace_from_LS "<< S.to_string() << std::endl ;
   std::cerr  << "offset in dataspace_from_LS"<< offset.to_string() << std::endl ;
#endif
   static const unsigned int rank = R + (IsComplex ? 1 : 0);
   hsize_t totdimsf[rank], dimsf [rank], stridesf[rank], offsetf[rank];  // dataset dimensions
   for (size_t u=0; u<R ; ++u) { offsetf[u] = offset[u]; dimsf[u] = L[u]; totdimsf[u] = Ltot[u]; stridesf[u] = S[u]; }
   if (IsComplex) { offsetf[rank-1]=0; dimsf[rank-1]=2; totdimsf[rank-1] = 2; stridesf[rank-1]=1; }
   DataSpace ds ( rank, totdimsf );
   ds.selectHyperslab (H5S_SELECT_SET , dimsf, offsetf, stridesf);
   return ds;
  }

 // the dataspace corresponding to the array. Contiguous data only...
 template <typename ArrayType >
  H5::DataSpace data_space ( ArrayType const & A) { 
   static const unsigned int R = ArrayType::rank;
   mini_vector<hsize_t,R> S; 
   mini_vector<std::ptrdiff_t,R> const & S1 ( A.indexmap().strides() );
   for (size_t u=0; u<R ; ++u) { 
    if (S1[u]<=0) TRIQS_RUNTIME_ERROR<<" negative strides not permitted in h5"; 
    S[u] =1; 
   }   
   if (!A.indexmap().is_contiguous())  TRIQS_RUNTIME_ERROR<<" h5 : internal error : array not contiguous";
   static const bool is_complex =  boost::is_complex<typename ArrayType::value_type>::value;
   return dataspace_from_LS<R,is_complex > ( A.indexmap().domain().lengths(),A.indexmap().domain().lengths(), S);
  }

 /********************   resize or check the size ****************************************************/

 template <typename A> ENABLE_IF(is_amv_value_class<A>) 
  resize_or_check ( A & a, mini_vector<size_t,A::rank> const & dimsf ) { a.resize( indexmaps::cuboid::domain_t<A::rank>( dimsf)); }

 template <typename A> ENABLE_IF(is_amv_view_class<A>) 
  resize_or_check ( A const & a, mini_vector<size_t,A::rank> const & dimsf ) { 
   if (a.indexmap().domain().lengths() != dimsf) TRIQS_RUNTIME_ERROR<<"Dimension error : the view can not be resized : " 
    << "\n in file  : "<< dimsf.to_string() 
     << "\n in view  : "<<a.indexmap().domain().lengths().to_string() ;
  }


#define TRIQS_ARRAYS_H5_CATCH_EXCEPTION \
 catch( triqs::runtime_error error)  { throw triqs::runtime_error() << error.what();}\
 catch( H5::FileIException error ) { error.printError(); TRIQS_RUNTIME_ERROR<<"H5 File error"; }\
 catch( H5::DataSetIException error ) { error.printError(); TRIQS_RUNTIME_ERROR<<"H5 DataSet error"; }\
 catch( H5::DataSpaceIException error ) { error.printError();  TRIQS_RUNTIME_ERROR<<"H5 DataSpace error"; }\
 catch( H5::DataTypeIException error ) { error.printError(); TRIQS_RUNTIME_ERROR<<"H5 DataType error";  }\
 catch( H5::AttributeIException error ) { error.printError(); TRIQS_RUNTIME_ERROR<<"H5 Attribute error";  }\
 catch(...) { TRIQS_RUNTIME_ERROR<<"H5 unknown error";} 

 /****************** WRITE attribute *********************************************/

 /*inline void write_attribute2 ( H5::H5Object const & grp, std::string obj_name, std::string attr_name, std::string value ) { 
   herr_t err =  H5LTset_attribute_string(grp.getId(),obj_name.c_str(),attr_name.c_str(), , value.c_str() ) ;
   if (err<0) TRIQS_RUNTIME_ERROR << "Error in setting attribute "<< name << " to " << value;
   }
   */
 /****************** Write string attribute *********************************************/

 inline void write_string_attribute ( H5::H5Object const * obj, std::string name, std::string value ) { 
  DataSpace attr_dataspace = DataSpace(H5S_SCALAR);
  // Create new string datatype for attribute
  StrType strdatatype(PredType::C_S1, value.size()); 
  // Set up write buffer for attribute
  //const H5std_string strwritebuf (value);
  // Create attribute and write to it
  Attribute myatt_in = obj->createAttribute(name.c_str(), strdatatype, attr_dataspace);
  //myatt_in.write(strdatatype, strwritebuf);
  myatt_in.write(strdatatype, (void *)(value.c_str()));
 } 

 /****************** Read string attribute *********************************************/

 /// Return the attribute name of obj, and "" if the attribute does not exist.
 inline std::string read_string_attribute (H5::H5Object const * obj, std::string name ) { 
  std::string value ="";
  Attribute attr;
  if (H5LTfind_attribute(obj -> getId(), name.c_str() )==0)  return value;// not present
  // can not find how to get the size with hl. Using full interface 
  //herr_t err2 =  H5LTget_attribute_string(gr.getId(), x.c_str(), name.c_str() , &(buf.front())  ) ;
  //if (err2 < 0) TRIQS_RUNTIME_ERROR << "Reading a string attribute and got rank !=0";
  //value.append( &(buf.front()) );
  try { attr= obj->openAttribute(name.c_str());}
  catch (H5::AttributeIException) { return value;}
  try { 
   DataSpace dataspace = attr.getSpace();
   int rank = dataspace.getSimpleExtentNdims();
   if (rank != 0) TRIQS_RUNTIME_ERROR << "Reading a string attribute and got rank !=0";
   size_t size = attr.getStorageSize();
   StrType strdatatype(PredType::C_S1, size);
   std::vector<char> buf(size+1, 0x00);
   attr.read(strdatatype, (void *)(&buf[0])); 
   value.append( &(buf.front()) );
  }
  TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
  return value;
 } 

 /****************** Read/Write the special TRIQS_HDF5_data_scheme attribute *********************************************/

 template<typename T>  
  inline void write_triqs_hdf5_data_scheme( H5::Group g, T const & obj) {  
   write_string_attribute( &g, "TRIQS_HDF5_data_scheme" , get_triqs_hdf5_data_scheme(obj).c_str() ) ;
   // herr_t err =  H5LTset_attribute_string(F.getId(), subgroup_name.c_str(), "TRIQS_HDF5_data_scheme" , get_triqs_hdf5_data_scheme(p).c_str() ) ;
  }   

 inline std::string read_triqs_hdf5_data_scheme( H5::Group g) { return read_string_attribute( &g, "TRIQS_HDF5_data_scheme") ; }   


 /****************** WRITE attribute *********************************************/

 //inline void write_triqs_data_scheme ( H5::H5Object const & grp, std::string obj_name, std::string attr_name, std::string triqs_data_scheme ) { 
 // herr_t err =  H5LTset_attribute_string(grp.getId(),obj_name.c_str(),attr_name.c_str(), , value.c_str() ) ;
 // if (err<0) TRIQS_RUNTIME_ERROR << "Error in setting attribute "<< name << " to " << value;
 // }



}}}
#endif
