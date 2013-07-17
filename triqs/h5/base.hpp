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
#ifndef TRIQS_H5_BASE_H
#define TRIQS_H5_BASE_H
#include <triqs/utility/mini_vector.hpp>
#include <type_traits>
#include <H5Cpp.h>
#include "hdf5_hl.h"
#include <boost/type_traits/is_complex.hpp>

namespace triqs { 

  inline std::string get_triqs_hdf5_data_scheme(bool)                 { return "bool";}
  inline std::string get_triqs_hdf5_data_scheme(int)                  { return "int";}
  inline std::string get_triqs_hdf5_data_scheme(long)                 { return "long";}
  inline std::string get_triqs_hdf5_data_scheme(long long)            { return "long long";}
  inline std::string get_triqs_hdf5_data_scheme(unsigned int)         { return "int";}
  inline std::string get_triqs_hdf5_data_scheme(unsigned long)        { return "unsigned long";}
  inline std::string get_triqs_hdf5_data_scheme(unsigned long long)   { return "unsigned long long";}
  inline std::string get_triqs_hdf5_data_scheme(float)                { return "float";}
  inline std::string get_triqs_hdf5_data_scheme(double)               { return "double";}
  inline std::string get_triqs_hdf5_data_scheme(long double)          { return "long double";}
  inline std::string get_triqs_hdf5_data_scheme(std::complex<double>) { return "complex";}

 namespace h5 { 

  using utility::mini_vector;

  // conversion of C type to HDF5 native
  inline H5::PredType native_type_from_C(char)               { return H5::PredType::NATIVE_CHAR; }
  inline H5::PredType native_type_from_C(signed char)        { return H5::PredType::NATIVE_SCHAR; }
  inline H5::PredType native_type_from_C(unsigned char)      { return H5::PredType::NATIVE_UCHAR; }
  inline H5::PredType native_type_from_C(short)              { return H5::PredType::NATIVE_SHORT; }
  inline H5::PredType native_type_from_C(unsigned short)     { return H5::PredType::NATIVE_USHORT; }
  inline H5::PredType native_type_from_C(int)                { return H5::PredType::NATIVE_INT; }
  inline H5::PredType native_type_from_C(unsigned)           { return H5::PredType::NATIVE_UINT; }
  inline H5::PredType native_type_from_C(long)               { return H5::PredType::NATIVE_LONG; }
  inline H5::PredType native_type_from_C(unsigned long)      { return H5::PredType::NATIVE_ULONG; }
  inline H5::PredType native_type_from_C(long long)          { return H5::PredType::NATIVE_LLONG; }
  inline H5::PredType native_type_from_C(unsigned long long) { return H5::PredType::NATIVE_ULLONG; }
  inline H5::PredType native_type_from_C(float)              { return H5::PredType::NATIVE_FLOAT; }
  inline H5::PredType native_type_from_C(double)             { return H5::PredType::NATIVE_DOUBLE; }
  inline H5::PredType native_type_from_C(long double)        { return H5::PredType::NATIVE_LDOUBLE; }
  inline H5::PredType native_type_from_C(bool)               { return H5::PredType::NATIVE_SCHAR; }
  inline H5::PredType native_type_from_C(std::string)        { return H5::PredType::C_S1; }

  // conversion of C type to HDF5 native
  // NEED TO CHANGE THIS ? It is not standard... We should fix a standard or have a trait 
  inline H5::PredType h5_type_from_C(char)               { return H5::PredType::NATIVE_CHAR; }
  inline H5::PredType h5_type_from_C(signed char)        { return H5::PredType::NATIVE_SCHAR; }
  inline H5::PredType h5_type_from_C(unsigned char)      { return H5::PredType::NATIVE_UCHAR; }
  inline H5::PredType h5_type_from_C(short)              { return H5::PredType::NATIVE_SHORT; }
  inline H5::PredType h5_type_from_C(unsigned short)     { return H5::PredType::NATIVE_USHORT; }
  inline H5::PredType h5_type_from_C(int)                { return H5::PredType::NATIVE_INT; }
  inline H5::PredType h5_type_from_C(unsigned)           { return H5::PredType::NATIVE_UINT; }
  inline H5::PredType h5_type_from_C(long)               { return H5::PredType::NATIVE_LONG; }
  inline H5::PredType h5_type_from_C(unsigned long)      { return H5::PredType::NATIVE_ULONG; }
  inline H5::PredType h5_type_from_C(long long)          { return H5::PredType::NATIVE_LLONG; }
  inline H5::PredType h5_type_from_C(unsigned long long) { return H5::PredType::NATIVE_ULLONG; }
  inline H5::PredType h5_type_from_C(float)              { return H5::PredType::NATIVE_FLOAT; }
  inline H5::PredType h5_type_from_C(double)             { return H5::PredType::NATIVE_DOUBLE; }
  inline H5::PredType h5_type_from_C(long double)        { return H5::PredType::NATIVE_LDOUBLE; }
  inline H5::PredType h5_type_from_C(bool)               { return H5::PredType::NATIVE_SCHAR; }
  inline H5::PredType h5_type_from_C(std::string)        { return H5::PredType::C_S1; }

  // If it is complex<T> return T else T
  template<typename T> struct remove_complex { typedef T type;};
  template<typename T> struct remove_complex<std::complex<T> > { typedef T type;};
  template<typename T> struct remove_complex<std::complex<const T> > { typedef T type;};
  template<typename T> struct remove_complex<const T>: remove_complex<T>{};

  //------------- compute the data type (removing complex) ----------------------------------

  // in memory
  template <typename S> H5::PredType data_type_memory () { return native_type_from_C(typename remove_complex<S>::type()); }

  // the type of data to put in the file_or_group
  template <typename V> H5::PredType data_type_file () { return h5_type_from_C(typename remove_complex<V>::type());}

  //------------- compute void * pointer to the data ----------------------------------
  template <typename S>
   typename std::enable_if< boost::is_complex<S>::value, typename std::conditional<std::is_const<S>::value, const void *, void *>::type >::type 
    get_data_ptr (S * p) { 
     typedef typename std::conditional<std::is_const<S>::value, const typename S::value_type, typename S::value_type>::type T; 
     return reinterpret_cast<T*>(p); 
    }

  template <typename S >
   typename std::enable_if< !boost::is_complex<S>::value, typename std::conditional<std::is_const<S>::value, const void *, void *>::type >::type 
   get_data_ptr (S * p) {return p;}

  // dataspace from lengths and strides. Correct for the complex. strides must be >0
  template<int R, bool IsComplex> 
   H5::DataSpace dataspace_from_LS (  
     mini_vector<hsize_t, R> const & Ltot, 
     mini_vector<hsize_t, R> const & L, 
     mini_vector<hsize_t, R> const & S, 
     mini_vector<hsize_t, R> const & offset = mini_vector<hsize_t,R>() ) 
   { 
    static const unsigned int rank = R + (IsComplex ? 1 : 0);
    hsize_t totdimsf[rank], dimsf [rank], stridesf[rank], offsetf[rank];  // dataset dimensions
    for (size_t u=0; u<R ; ++u) { offsetf[u] = offset[u]; dimsf[u] = L[u]; totdimsf[u] = Ltot[u]; stridesf[u] = S[u]; }
    if (IsComplex) { offsetf[rank-1]=0; dimsf[rank-1]=2; totdimsf[rank-1] = 2; stridesf[rank-1]=1; }
    H5::DataSpace ds ( rank, totdimsf );
    ds.selectHyperslab (H5S_SELECT_SET , dimsf, offsetf, stridesf);
    return ds;
   }

#define TRIQS_ARRAYS_H5_CATCH_EXCEPTION \
  catch( triqs::runtime_error const & error)  { throw triqs::runtime_error() << error.what();}\
  catch( H5::AttributeIException const & error ) { error.printError(); TRIQS_RUNTIME_ERROR<<"H5 Attribute error";  }\
  catch( H5::DataSetIException const & error ) { error.printError(); TRIQS_RUNTIME_ERROR<<"H5 DataSet error"; }\
  catch( H5::DataSpaceIException const & error ) { error.printError();  TRIQS_RUNTIME_ERROR<<"H5 DataSpace error"; }\
  catch( H5::DataTypeIException const & error ) { error.printError(); TRIQS_RUNTIME_ERROR<<"H5 DataType error";  }\
  catch( H5::FileIException const & error ) { error.printError(); TRIQS_RUNTIME_ERROR<<"H5 File error"; }\
  catch( H5::GroupIException const & error ) { error.printError(); TRIQS_RUNTIME_ERROR<<"H5 Group error"; }\
  catch( H5::IdComponentException const & error ) { error.printError(); TRIQS_RUNTIME_ERROR<<"H5 IdComponentException error"; }\
  catch( H5::LibraryIException const & error ) { error.printError(); TRIQS_RUNTIME_ERROR<<"H5 LibraryIException error"; }\
  catch( H5::PropListIException const & error ) { error.printError(); TRIQS_RUNTIME_ERROR<<"H5 PropListIException error"; }\
  catch( H5::ReferenceException const & error ) { error.printError(); TRIQS_RUNTIME_ERROR<<"H5 ReferenceException error"; }\
  catch(...) { TRIQS_RUNTIME_ERROR<<"H5 unknown error";} 

  /****************** Write string attribute *********************************************/

  inline void write_string_attribute ( H5::H5Object const * obj, std::string name, std::string value ) { 
   H5::DataSpace attr_dataspace = H5::DataSpace(H5S_SCALAR);
   // Create new string datatype for attribute
   H5::StrType strdatatype(H5::PredType::C_S1, value.size()); 
   // Set up write buffer for attribute
   //const H5std_string strwritebuf (value);
   // Create attribute and write to it
   H5::Attribute myatt_in = obj->createAttribute(name.c_str(), strdatatype, attr_dataspace);
   //myatt_in.write(strdatatype, strwritebuf);
   myatt_in.write(strdatatype, (void *)(value.c_str()));
  } 


  /****************** Read string attribute *********************************************/

  /// Return the attribute name of obj, and "" if the attribute does not exist.
  inline std::string read_string_attribute (H5::H5Object const * obj, std::string name ) { 
   std::string value ="";
   H5::Attribute attr;
   if (H5LTfind_attribute(obj -> getId(), name.c_str() )==0)  return value;// not present
   // can not find how to get the size with hl. Using full interface 
   //herr_t err2 =  H5LTget_attribute_string(gr.getId(), x.c_str(), name.c_str() , &(buf.front())  ) ;
   //value.append( &(buf.front()) );
   try { attr= obj->openAttribute(name.c_str());}
   catch (H5::AttributeIException) { return value;}
   try { 
    H5::DataSpace dataspace = attr.getSpace();
    int rank = dataspace.getSimpleExtentNdims();
    if (rank != 0) TRIQS_RUNTIME_ERROR << "Reading a string attribute and got rank !=0";
    size_t size = attr.getStorageSize();
    H5::StrType strdatatype(H5::PredType::C_S1, size+1);
    std::vector<char> buf(size+1, 0x00);
    attr.read(strdatatype, (void *)(&buf[0])); 
    value.append( &(buf.front()) );
   }
   TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
   return value;
  } 

 }}
#endif

