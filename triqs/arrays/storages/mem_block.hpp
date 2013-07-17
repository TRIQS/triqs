
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
#ifndef TRIQS_MEM_BLOCK_H 
#define TRIQS_MEM_BLOCK_H
#include "../../utility/exceptions.hpp"
#include <Python.h>
#include <numpy/arrayobject.h>
#include "./memcopy.hpp"

#ifdef TRIQS_ARRAYS_DEBUG_TRACE_MEM
#include <iostream>
#define TRACE_MEM_DEBUG(X) std::cerr<< X << std::endl; 
#else
#define TRACE_MEM_DEBUG(X) 
#endif
 
namespace triqs { namespace arrays { namespace storages { namespace details { 

 template<typename ValueType> class mem_block {

   typedef typename boost::remove_const<ValueType>::type non_const_value_type;
   size_t size_;
   non_const_value_type * restrict p;
   PyObject * py_obj;
   static void import_numpy_array() { if (_import_array()!=0) TRIQS_RUNTIME_ERROR <<"Internal Error in importing numpy";}

   public : 

   mem_block():size_(0),p(NULL),py_obj(NULL){}

   mem_block (size_t s):size_(s),py_obj(NULL){
    //p = new non_const_value_type[s]; // check speed penalty for try ??
    try { p = new non_const_value_type[s];}
    catch (std::bad_alloc& ba) { TRIQS_RUNTIME_ERROR<< "Memory allocation error : bad_alloc : "<< ba.what();}
   }

   mem_block (PyObject * obj, bool borrowed ) { 
    TRACE_MEM_DEBUG(" construct memblock from pyobject"<<obj<< " # ref ="<<obj->ob_refcnt<<" borrowed = "<< borrowed); 
    assert(obj); import_numpy_array(); 
    if (borrowed) Py_INCREF(obj);
    if (!PyArray_Check(obj)) TRIQS_RUNTIME_ERROR<<"Internal error : mem_block construct from pyo : obj is not an array";
    PyArrayObject * arr = (PyArrayObject *)(obj);
    size_ = PyArray_SIZE(arr);
    this->p = (non_const_value_type*)PyArray_DATA(arr); 
    py_obj = obj;
   }

   ~mem_block(){ // delete memory manually iif py_obj is not set. Otherwise the python interpreter will do that for us.
    TRACE_MEM_DEBUG("deleting mem block p ="<<p<< "  py_obj = "<< py_obj << "    ref of py obj if exists"<<(py_obj ? py_obj->ob_refcnt: -1));
    if (py_obj==NULL) { if (p) { delete[] p;} } 
    else Py_DECREF(py_obj); 
   } 

  void operator=(const mem_block & X) {
    //assert( py_obj==NULL); 
    assert(size_==X.size_);assert(p); assert(X.p);
    storages::memcopy (p, X.p, size_);
   }

   mem_block ( mem_block const & X): size_(X.size()), py_obj(NULL) {
    //p = new non_const_value_type[s]; // check speed penalty for try ??
    try { p = new non_const_value_type[X.size()];}
    catch (std::bad_alloc& ba) { TRIQS_RUNTIME_ERROR<< "Memory allocation error : bad_alloc : "<< ba.what();}
    if ((X.py_obj==NULL) || (PyCObject_Check(X.py_obj))) { (*this) = X; }
    else { 
     // else make a new copy of the numpy ...
     import_numpy_array(); 
     //assert(PyArray_Check(X.py_obj));
     if (!is_scalar_or_pod<ValueType>::value) TRIQS_RUNTIME_ERROR << "Internal Error : memcpy on non-scalar";
#ifdef TRIQS_NUMPY_VERSION_LT_17
     if ( ( PyArray_ISFORTRAN(X.py_obj)) || (PyArray_ISCONTIGUOUS(X.py_obj)))  {
      memcpy (p,PyArray_DATA(X.py_obj),size_ * sizeof(ValueType));
     }
#else
      // STRANGE : uncommenting this leads to a segfault on mac ???
      // TO BE INVESTIGATED, IT IS NOT NORMAL
      //if (!PyArray_Check(X.py_obj)) TRIQS_RUNTIME_ERROR<<"Internal error : is not an array";
      PyArrayObject * arr3 = (PyArrayObject *)(X.py_obj);
      if ( ( PyArray_ISFORTRAN(arr3)) || (PyArray_ISCONTIGUOUS(arr3)))  { 
      memcpy (p,PyArray_DATA(arr3),size_ * sizeof(ValueType));
     }
#endif
     else { // if the X.py_obj is not contiguous, first let numpy copy it properly
      PyObject * na = PyObject_CallMethod(X.py_obj,(char *)"copy",NULL);
      assert(na); 
#ifdef TRIQS_NUMPY_VERSION_LT_17
      assert( ( PyArray_ISFORTRAN(na)) || (PyArray_ISCONTIGUOUS(na)));
      memcpy (p,PyArray_DATA(na),size_ * sizeof(ValueType));
#else
      if (!PyArray_Check(na)) TRIQS_RUNTIME_ERROR<<"Internal error : is not an array";
      PyArrayObject * arr = (PyArrayObject *)(na);
      assert( ( PyArray_ISFORTRAN(arr)) || (PyArray_ISCONTIGUOUS(arr)));
      memcpy (p,PyArray_DATA(arr),size_ * sizeof(ValueType));
#endif
      Py_DECREF(na);
     }
    }
   }

   static void delete_pointeur( void *ptr ) { 
    TRACE_MEM_DEBUG("deleting data block"<<(non_const_value_type*) ptr); 
    delete [] ( (non_const_value_type*) ptr) ;
   }   

   PyObject * new_ref_to_guard() { 
    if (py_obj==NULL) { 
     TRACE_MEM_DEBUG(" activating python guard for C++ block"<<p<< "  py_obj = "<< py_obj);
     py_obj = PyCObject_FromVoidPtr( (void*) p, &mem_block<ValueType>::delete_pointeur);
    } 
    Py_INCREF(py_obj); 
    return py_obj;
   } 

   non_const_value_type & operator[](size_t i) {return p[i];}

   size_t size() const {return size_;}

   template<class Archive>
    void save(Archive & ar, const unsigned int version) const { 
     ar << boost::serialization::make_nvp("size",size_);
     for (size_t i=0; i<size_; ++i) ar << boost::serialization::make_nvp("data",p[i]); 
    }

   template<class Archive>
    void load(Archive & ar, const unsigned int version) { 
     ar >> size_;
     assert (p==NULL); 
     p = new non_const_value_type[size_]; 
     for (size_t i=0; i<size_; ++i) ar >> p[i]; 
    }
   BOOST_SERIALIZATION_SPLIT_MEMBER();
 };
}}}}//namespace triqs::arrays 

#undef TRACE_MEM_DEBUG
#endif

