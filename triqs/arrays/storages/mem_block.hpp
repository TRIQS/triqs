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

#ifdef TRIQS_WITH_PYTHON_SUPPORT
#include <Python.h>
#include <numpy/arrayobject.h>
#else
// if no python, same code but remove python parts...
typedef void PyObject;
#define Py_DECREF(P)
#endif

#include "./memcopy.hpp"
#include <triqs/utility/macros.hpp>

//#define TRIQS_ARRAYS_DEBUG_TRACE_MEM
#ifdef TRIQS_ARRAYS_DEBUG_TRACE_MEM
#include <iostream>
#define TRACE_MEM_DEBUG(X) std::cerr<< X << std::endl;
#else
#define TRACE_MEM_DEBUG(X)
#endif


namespace triqs { namespace arrays { namespace storages { //namespace details {

 template<typename ValueType> struct mem_block; // forward

// debug only, to check also weak refs. This will slow down a bit critical loops..
// I am not sure this is really useful ...
//#define TRIQS_ARRAYS_CHECK_WEAK_REFS

 // the ref counting functions : weak means : no use of the reference counting system
 template<bool Weak, typename ValueType> ENABLE_IFC(Weak)  inc_ref(mem_block<ValueType> * const m) {
#ifdef TRIQS_ARRAYS_CHECK_WEAK_REFS
  m->weak_ref_count++;
#endif
 }
 template<bool Weak, typename ValueType> DISABLE_IFC(Weak) inc_ref(mem_block<ValueType> * const m) { m->ref_count++;}

 // force inline on gcc ?
 //template<typename ValueType> void dec_ref(mem_block<ValueType> * const m) __attribute__((always_inline));

 template<bool Weak, typename ValueType> ENABLE_IFC(Weak) dec_ref(mem_block<ValueType> * const m) {
#ifdef TRIQS_ARRAYS_CHECK_WEAK_REFS
  m->weak_ref_count--;
#endif
 }

 template<bool Weak, typename ValueType> DISABLE_IFC(Weak) dec_ref(mem_block<ValueType> * const m) {
  m->ref_count--; if (m->ref_count ==0) {
#ifdef TRIQS_ARRAYS_CHECK_WEAK_REFS
  //std::cout  << " detroying "<< m->weak_ref_count <<std::endl;
  if (m->weak_ref_count !=0) TRIQS_RUNTIME_ERROR << "Deleting an memory block of an array with still "<< m->weak_ref_count<< " weak references";
#endif
   delete m;
  }
 }

 /**
  *  This is a block of memory (pointer p and size size_).
  *  INTERNAL USE only, by shared_block only
  *
  *  The memory can be :
  *
  *   - allocated (and deleted in C++)
  *   - owned by a numpy python object (py_numpy)
  *
  *  The block contains its own reference system, to avoid the use of shared_ptr in shared_block
  *  (which was very slow in critical codes).
  *
  *  In addition, the python guard system can return to python an array allocated in C++.
  *
  *  The memory block has 4 possible states (and only 4) :
  *
  *  * State 0) :  p ==nullptr && py_numpy == nullptr && py_guard == nullptr
  *    Default state. Also obtained when the object has been moved from
  *
  *  * State 1) : p !=nullptr && py_numpy == nullptr && py_guard == nullptr
  *    Memory block allocated and used in C++.
  *
  *  * State 2) :  p !=nullptr && py_numpy != nullptr && py_guard == nullptr
  *    Memory block was allocated by python, and is used in C++.
  *    The block keeps a python ref (owned!) in py_numpy which is released at destruction
  *
  *  * State 3) :  p !=nullptr && py_numpy == nullptr && py_guard != nullptr
  *    Memory block allocated by C++, but passed to python.
  *    The guard is an owned reference to a python object, which itself owns a c++ reference to the mem_block
  *    When the python is done with this object, it releases this reference and the mem_block
  *    can be destroyed (when nobody else uses it).
  *    This guarantees that :
  *       * as long as python does not clean the guard (which is then later attached to a numpy array, cf numpy interface)
  *         the block will not be deleted.
  *       * when python is done with this numpy, hence the guard, the c++ reference is dec_refed and
  *         the usage can continue normally in c++ (without *any* python ref contrary to a previous design).
  *
  *  * Invariants :
  *    * py_numpy == nullptr || py_guard == nullptr :
  *    * ref_count >=1.
  */
 template<typename ValueType> struct mem_block {

  size_t size_;                  // size of the block
  ValueType * restrict p;        // the memory block. Owned by this, except when py_numpy is not null
  size_t ref_count;              // number of refs. :  >=1
  size_t weak_ref_count;              // number of refs. :  >=1
  PyObject * py_numpy;           // if not null, an owned reference to a numpy which is the data of this block
  PyObject * py_guard;           // if not null, a BORROWED reference to the guard. If null, the guard does not exist
  static_assert(!std::is_const<ValueType>::value, "internal error");

#ifdef TRIQS_WITH_PYTHON_SUPPORT
  static void import_numpy_array() { if (_import_array()!=0) TRIQS_RUNTIME_ERROR <<"Internal Error in importing numpy";}
#endif

  //Construct to state 0
  mem_block():size_(0),p(nullptr),py_numpy(nullptr), py_guard(nullptr), ref_count(1), weak_ref_count(0){}

  // construct to state 1 with a given size.
  mem_block (size_t s):size_(s),py_numpy(nullptr), py_guard(nullptr){
   try { p = new ValueType[s];}
   catch (std::bad_alloc& ba) { TRIQS_RUNTIME_ERROR<< "Memory allocation error : bad_alloc : "<< ba.what();}
   ref_count=1;
   weak_ref_count =0;
  }

#ifdef TRIQS_WITH_PYTHON_SUPPORT
  // construct to state 2. python_object_is_borrowed : whether the python ref is borrowed
  mem_block (PyObject * obj, bool python_object_is_borrowed) {
   TRACE_MEM_DEBUG(" construct memblock from pyobject"<<obj<< " # ref ="<<obj->ob_refcnt<<" python_object_is_borrowed = "<< python_object_is_borrowed);
   assert(obj); import_numpy_array();
   if (python_object_is_borrowed) Py_INCREF(obj);
   if (!PyArray_Check(obj)) TRIQS_RUNTIME_ERROR<<"Internal error : mem_block construct from pyo : obj is not an array";
   PyArrayObject * arr = (PyArrayObject *)(obj);
   size_ = PyArray_SIZE(arr);
   p = (ValueType*)PyArray_DATA(arr);
   py_numpy = obj;
   py_guard = nullptr;
   ref_count=1;
   weak_ref_count =0;
  }
#endif

  // destructor : release memory only in state 1. This should NEVER be called in state 3 (first need to get back to 1).
  ~mem_block(){ // delete memory manually iif py_obj is not set. Otherwise the python interpreter will do that for us.
   TRACE_MEM_DEBUG("deleting mem block p ="<<p<< "  py_obj = "<< py_obj << "    ref of py obj if exists"<<(py_obj ? py_obj->ob_refcnt: -1));
   assert(ref_count<=1); assert(py_guard==nullptr);// state 3 forbidden
   if (py_numpy) Py_DECREF(py_numpy); // state 1
   else { if (p) delete[] p; } // state 2 or state 0
  }

  // can not be copied or moved.
  mem_block & operator=(mem_block const & X) = delete;
  mem_block & operator=(mem_block && X) = delete;
  mem_block(mem_block && X) noexcept {
   size_ = X.size_; p = X.p; ref_count = X.ref_count; weak_ref_count = X.weak_ref_count; py_numpy=X.py_numpy; py_guard = X.py_guard;
   X.p =nullptr; X.py_numpy= nullptr; X.py_guard = nullptr; // state 0, ready to destruct
  }

  // deep copy of data from another block.
  // MUST be of the same size...
  void copy_from(const mem_block & X) {
   assert(size_==X.size_);assert(p); assert(X.p);
   storages::memcopy (p, X.p, size_);
  }

  // copy construct into state 1, always.
  // This is a choice, even if X is state 2 (a numpy).
  // We copy a numpy into a regular C++ array, which can then be used at max speed.
  mem_block (mem_block const & X): size_(X.size()), py_numpy(nullptr), py_guard(nullptr) {
   try { p = new ValueType[X.size()];}
   catch (std::bad_alloc& ba) { TRIQS_RUNTIME_ERROR<< "Memory allocation error : bad_alloc : "<< ba.what();}
   ref_count=1;
   weak_ref_count =0;
   // now we copy the data
#ifndef TRIQS_WITH_PYTHON_SUPPORT
   copy_from(X);
#else
   // if X is in state 1 or 3
   if (X.py_numpy==nullptr) { copy_from(X); }
   else { // X was in state 2
    // else make a new copy of the numpy ...
    import_numpy_array();
    if (!is_scalar_or_pod<ValueType>::value) TRIQS_RUNTIME_ERROR << "Internal Error : memcpy on non-scalar";
#ifdef TRIQS_NUMPY_VERSION_LT_17
    PyObject * arr3 = X.py_numpy;
#else
    // STRANGE : uncommenting this leads to a segfault on mac ???
    // TO BE INVESTIGATED, IT IS NOT NORMAL
    //if (!PyArray_Check(X.py_numpy)) TRIQS_RUNTIME_ERROR<<"Internal error : is not an array";
    PyArrayObject * arr3 = (PyArrayObject *)(X.py_numpy);
#endif
    // if we can make a memcpy, do it.
    if ( ( PyArray_ISFORTRAN(arr3)) || (PyArray_ISCONTIGUOUS(arr3)))  {
     memcpy (p,PyArray_DATA(arr3),size_ * sizeof(ValueType));
    }
    else { // if the X.py_numpy is not contiguous, first let numpy copy it properly, then memcpy
     PyObject * na = PyObject_CallMethod(X.py_numpy,(char *)"copy",nullptr);
     assert(na);
#ifdef TRIQS_NUMPY_VERSION_LT_17
     PyObject * arr = na;
#else
     if (!PyArray_Check(na)) TRIQS_RUNTIME_ERROR<<"Internal error : is not an array";
     PyArrayObject * arr = (PyArrayObject *)(na);
#endif
     assert( ( PyArray_ISFORTRAN(arr)) || (PyArray_ISCONTIGUOUS(arr)));
     memcpy (p,PyArray_DATA(arr),size_ * sizeof(ValueType));
     Py_DECREF(na);
    }
   }
#endif
  }

#ifdef TRIQS_WITH_PYTHON_SUPPORT
  // Precondition : state 3, postcondition: state 1
  static void delete_python_guard(void *ptr ) {
   mem_block * m = static_cast<mem_block*>(ptr);
   TRACE_MEM_DEBUG("deleting data block"<<m);
   assert(m->ref_count>0); assert(m->py_guard !=nullptr);assert(m->py_numpy ==nullptr);
   m->py_guard=nullptr;
   dec_ref<false>(m);// release the reference which was owned by the guard
  }

  // returns a NEW python ref either to the numpy or to the guard.
  // if the guard does not yet exists, create it.
  PyObject * new_python_ref() {
   // if we just have a borrowed numpy, just return a new ref to it
   if (py_numpy) { Py_INCREF(py_numpy); return py_numpy;}
   // if the guard is already set, then return it, otherwise create it...
   if (py_guard) { Py_INCREF(py_guard); return py_guard;}
   else {
    TRACE_MEM_DEBUG(" activating python guard for C++ block"<<p<< "  py_guard = "<< py_guard);
    inc_ref<false>(this); // the guard owns a C++ reference !!
    py_guard = PyCObject_FromVoidPtr( static_cast<void *>(this), &mem_block<ValueType>::delete_python_guard);
   }
   return py_guard;
  }
#endif

  ValueType & operator[](size_t i) {return p[i];}

  size_t size() const {return size_;}

  template<class Archive>
   void save(Archive & ar, const unsigned int version) const {
    ar << boost::serialization::make_nvp("size",size_);
    for (size_t i=0; i<size_; ++i) ar << boost::serialization::make_nvp("data",p[i]);
   }

  template<class Archive>
   void load(Archive & ar, const unsigned int version) {
    ar >> size_;
    assert (p==nullptr);
    p = new ValueType[size_];
    for (size_t i=0; i<size_; ++i) ar >> p[i];
   }
  BOOST_SERIALIZATION_SPLIT_MEMBER();
 };

}}}//namespace triqs::arrays

#undef TRACE_MEM_DEBUG
#endif

