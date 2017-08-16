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

#include "../storages/shared_block.hpp"
#include "triqs/utility/macros.hpp"
#include "triqs/utility/exceptions.hpp"
#ifdef TRIQS_WITH_PYTHON_SUPPORT
#include "numpy/arrayobject.h"

namespace triqs { namespace arrays { namespace numpy_interface  {

 using utility::mini_vector;

 inline std::string object_to_string (PyObject * p) {
  if (!PyString_Check(p)) TRIQS_RUNTIME_ERROR<<" Internal error, expected a python string .....";
  return PyString_AsString(p);
 }

 template <class T> struct numpy_to_C_type;
 template <class T> struct numpy_to_C_type<T const> : numpy_to_C_type<T> {};
#define CONVERT(C, P)                                                                                                            \
 template <> struct numpy_to_C_type<C> {                                                                                         \
  enum {                                                                                                                         \
   arraytype = P                                                                                                                 \
  };                                                                                                                             \
  static const char* name() { return AS_STRING(C); }                                                                              \
 }
  CONVERT(bool, NPY_BOOL);
 CONVERT(char, NPY_STRING);
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
 PyObject* numpy_extractor_impl(PyObject* X, bool allow_copy, std::string type_name, int elementsType, int rank, size_t* lengths,
                                std::ptrdiff_t* strides, size_t size_of_ValueType);

 // a little template class
 template<typename IndexMapType, typename ValueType > struct numpy_extractor {

  numpy_extractor (PyObject * X, bool allow_copy) {
   using numpy_t = numpy_to_C_type<std14::remove_const_t<ValueType>>;
   numpy_obj = numpy_extractor_impl(X, allow_copy, numpy_t::name(), numpy_t::arraytype, IndexMapType::rank, &lengths[0],
                                    &strides[0], sizeof(ValueType));
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
