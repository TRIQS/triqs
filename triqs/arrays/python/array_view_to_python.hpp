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
#ifndef TRIQS_ARRAYS_TO_PYTHON_H
#define TRIQS_ARRAYS_TO_PYTHON_H 
#ifndef TRIQS_WITH_PYTHON_SUPPORT
#error "You must define the macro TRIQS_WITH_PYTHON_SUPPORT to use Python interface"
#endif
#include <complex>
#include "../impl/indexmap_storage_pair.hpp"
//#include "../array.hpp"

namespace triqs { namespace arrays { namespace numpy_interface  {

 template<typename ArrayViewType >
  PyObject * array_view_to_python ( ArrayViewType const & A, bool copy=false) {
   _import_array();
   typedef typename ArrayViewType::value_type value_type;
   static const int rank = ArrayViewType::rank;
   const int elementsType (numpy_to_C_type<typename boost::remove_const<value_type>::type>::arraytype);
   npy_intp dims[rank],  strides[rank];
   for(size_t i =0; i<rank; ++i) { dims[i] = A.indexmap().lengths()[i]; strides[i] = A.indexmap().strides()[i]*sizeof(value_type); }
   const value_type * data = A.data_start();
   //int flags = NPY_ARRAY_BEHAVED & ~NPY_ARRAY_OWNDATA;;// for numpy2
#ifdef TRIQS_NUMPY_VERSION_LT_17
   int flags = NPY_BEHAVED & ~NPY_OWNDATA;
#else
   int flags = NPY_ARRAY_BEHAVED & ~NPY_ARRAY_OWNDATA;
#endif
   PyObject* res  = PyArray_NewFromDescr(&PyArray_Type, PyArray_DescrFromType(elementsType), (int) rank, dims, strides, (void*) data,  flags, NULL);

   if (!res) { 
    if (PyErr_Occurred()) {PyErr_Print();PyErr_Clear();}
    TRIQS_RUNTIME_ERROR<<" array_view_from_numpy : the python numpy object could not be build";
   }
   if (!PyArray_Check(res)) TRIQS_RUNTIME_ERROR<<" array_view_from_numpy : internal error : the python object is not a numpy";
   PyArrayObject * arr = (PyArrayObject *)(res);
   //PyArray_SetBaseObject(arr,  A.storage().new_python_ref());
#ifdef TRIQS_NUMPY_VERSION_LT_17
   arr->base =  A.storage().new_python_ref();
   assert( arr->flags == (arr->flags & ~NPY_OWNDATA));
#else
   int r = PyArray_SetBaseObject(arr,A.storage().new_python_ref());
   if (r!=0) TRIQS_RUNTIME_ERROR << "Internal Error setting the guard in numpy !!!!";
   assert( PyArray_FLAGS(arr) == (PyArray_FLAGS(arr) & ~NPY_ARRAY_OWNDATA));
#endif
   if (copy)  { 
    PyObject * na = PyObject_CallMethod(res,(char*)"copy",NULL);
    Py_DECREF(res);
    // POrt this for 1.7
    //assert(((PyArrayObject *)na)->base ==NULL);
    res = na;
   }
   return res;
  }

}}}
#endif 

