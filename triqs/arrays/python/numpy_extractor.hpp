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
#ifndef TRIQS_ARRAYS_NUMPY_EXTRACTOR_H
#define TRIQS_ARRAYS_NUMPY_EXTRACTOR_H

#ifdef TRIQS_WITH_PYTHON_SUPPORT
#include "../storages/shared_block.hpp"
#include "triqs/utility/exceptions.hpp"
#include "numpy/arrayobject.h"

namespace triqs { namespace arrays { namespace numpy_interface  {

 inline std::string object_to_string (PyObject * p) {
  if (!PyString_Check(p)) TRIQS_RUNTIME_ERROR<<" Internal error, expected a python string .....";
  return PyString_AsString(p);
 }

 template <class T> struct numpy_to_C_type;
#define CONVERT(C,P) template <> struct numpy_to_C_type<C> { enum {arraytype = P}; }
 CONVERT(bool, NPY_BOOL);
 CONVERT(char, NPY_CHAR);
 CONVERT(signed char, NPY_BYTE);
 CONVERT(unsigned char, NPY_UBYTE);
 CONVERT(short, NPY_SHORT);
 CONVERT(unsigned short, NPY_USHORT);
 CONVERT(int, NPY_INT);
 CONVERT(unsigned int, NPY_UINT);
 CONVERT(long, NPY_LONG);
 CONVERT(unsigned long, NPY_ULONG);
 CONVERT(long long, NPY_LONGLONG);
 CONVERT(unsigned long long, NPY_ULONGLONG);
 CONVERT(float, NPY_FLOAT);
 CONVERT(double, NPY_DOUBLE);
 CONVERT(long double, NPY_LONGDOUBLE);
 CONVERT(std::complex<float>, NPY_CFLOAT);
 CONVERT(std::complex<double>, NPY_CDOUBLE);
 CONVERT(std::complex<long double>, NPY_CLONGDOUBLE);
#undef CONVERT

 struct copy_exception : public triqs::runtime_error {};

 // return a NEW (owned) reference
 //
 inline PyObject * numpy_extractor_impl ( PyObject * X, bool allow_copy, std::string type_name, 
   int elementsType, int rank, size_t * lengths, std::ptrdiff_t * strides, size_t size_of_ValueType) {

  PyObject * numpy_obj;

  if (X==NULL) TRIQS_RUNTIME_ERROR<<"numpy interface : the python object is NULL !";
  if (_import_array()!=0) TRIQS_RUNTIME_ERROR <<"Internal Error in importing numpy";

  static const char * error_msg = "   A deep copy of the object would be necessary while views are supposed to guarantee to present a *view* of the python data.\n";

  if (!allow_copy) {
   if (!PyArray_Check(X)) throw copy_exception () << error_msg<<"   Indeed the object was not even an array !\n";
   if ( elementsType != PyArray_TYPE((PyArrayObject*)X))
    throw copy_exception () << error_msg<<"   The deep copy is caused by a type mismatch of the elements. Expected "<< type_name<< " and found XXX \n";
   PyArrayObject *arr = (PyArrayObject *)X;
#ifdef TRIQS_NUMPY_VERSION_LT_17
   if ( arr->nd != rank) throw copy_exception () << error_msg<<"   Rank mismatch . numpy array is of rank "<< arr->nd << "while you ask for rank "<< rank<<". \n";
#else
   if ( PyArray_NDIM(arr) != rank) throw copy_exception () << error_msg<<"   Rank mismatch . numpy array is of rank "<<  PyArray_NDIM(arr) << "while you ask for rank "<< rank<<". \n";
#endif
   numpy_obj = X; Py_INCREF(X);
  }
  else {
   // From X, we ask the numpy library to make a numpy, and of the correct type.
   // This handles automatically the cases where :
   //   - we have list, or list of list/tuple
   //   - the numpy type is not the one we want.
   //   - adjust the dimension if needed
   // If X is an array :
   //   - if Order is same, don't change it
   //   - else impose it (may provoque a copy).
   // if X is not array :
   //   - Order = FortranOrder or SameOrder - > Fortran order otherwise C

   //bool ForceCast = false;// Unless FORCECAST is present in flags, this call will generate an error if the data type cannot be safely obtained from the object.
   int flags = 0; //(ForceCast ? NPY_FORCECAST : 0) ;// do NOT force a copy | (make_copy ?  NPY_ENSURECOPY : 0);
   if (!(PyArray_Check(X) ))
    //flags |= ( IndexMapType::traversal_order == indexmaps::mem_layout::c_order(rank) ? NPY_C_CONTIGUOUS : NPY_F_CONTIGUOUS); //impose mem order
#ifdef TRIQS_NUMPY_VERSION_LT_17
    flags |= (NPY_C_CONTIGUOUS); //impose mem order
#else
    flags |= (NPY_ARRAY_C_CONTIGUOUS); //impose mem order
#endif
   numpy_obj= PyArray_FromAny(X,PyArray_DescrFromType(elementsType), rank,rank, flags , NULL );

   // do several checks
   if (!numpy_obj) {// The convertion of X to a numpy has failed !
    if (PyErr_Occurred()) {
     //PyErr_Print();
     PyErr_Clear();
    }
    TRIQS_RUNTIME_ERROR<<"numpy interface : the python object  is not convertible to a numpy. ";
   }
   assert (PyArray_Check(numpy_obj)); assert((numpy_obj->ob_refcnt==1) || ((numpy_obj ==X)));

   PyArrayObject *arr_obj;
   arr_obj = (PyArrayObject *)numpy_obj;
   try {
#ifdef TRIQS_NUMPY_VERSION_LT_17
    if (arr_obj->nd!=rank)  TRIQS_RUNTIME_ERROR<<"numpy interface : internal error : dimensions do not match";
    if (arr_obj->descr->type_num != elementsType)
     TRIQS_RUNTIME_ERROR<<"numpy interface : internal error : incorrect type of element :" <<arr_obj->descr->type_num <<" vs "<<elementsType;
#else
    if ( PyArray_NDIM(arr_obj) !=rank)  TRIQS_RUNTIME_ERROR<<"numpy interface : internal error : dimensions do not match";
    if ( PyArray_DESCR(arr_obj)->type_num != elementsType)
     TRIQS_RUNTIME_ERROR<<"numpy interface : internal error : incorrect type of element :" <<PyArray_DESCR(arr_obj)->type_num <<" vs "<<elementsType;
#endif
   }
   catch(...) { Py_DECREF(numpy_obj); throw;} // make sure that in case of problem, the reference counting of python is still ok...
  }

  // extract strides and lengths
  PyArrayObject *arr_obj;
  arr_obj = (PyArrayObject *)numpy_obj;
#ifdef TRIQS_NUMPY_VERSION_LT_17
  const size_t dim =arr_obj->nd; // we know that dim == rank
  for (size_t i=0; i< dim ; ++i) {
   lengths[i] = size_t(arr_obj-> dimensions[i]);
   strides[i] = std::ptrdiff_t(arr_obj-> strides[i])/ size_of_ValueType;
  }
#else
  const size_t dim = PyArray_NDIM(arr_obj); // we know that dim == rank
  for (size_t i=0; i< dim ; ++i) {
   lengths[i] = size_t( PyArray_DIMS(arr_obj)[i]);
   strides[i] = std::ptrdiff_t( PyArray_STRIDES(arr_obj)[i])/ size_of_ValueType;
  }
#endif

  return numpy_obj;
 }

 // a little template class
 template<typename IndexMapType, typename ValueType > struct numpy_extractor {

  numpy_extractor (PyObject * X, bool allow_copy) {
   numpy_obj = numpy_extractor_impl (X, allow_copy, typeid(ValueType).name(), numpy_to_C_type<typename boost::remove_const<ValueType>::type>::arraytype, IndexMapType::rank,
     &lengths[0], &strides[0],sizeof(ValueType));
  }

  ~numpy_extractor(){ Py_DECREF(numpy_obj);}

  IndexMapType indexmap() const { return IndexMapType (lengths,strides,0); }

  storages::shared_block<ValueType> storage() const { return storages::shared_block<ValueType> (numpy_obj,true); }
  // true means borrowed : object is owned by this class, which will decref it in case of exception ...

  private:
  PyObject * numpy_obj;
  mini_vector<size_t,IndexMapType::rank> lengths;
  mini_vector<std::ptrdiff_t,IndexMapType::rank> strides;
 };
}}}
#endif
#endif
