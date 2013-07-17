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
#ifndef TRIQS_STORAGE_SHARED_POINTER_H
#define TRIQS_STORAGE_SHARED_POINTER_H
#include <string.h>
#include <limits>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/is_same.hpp> 
#include <boost/serialization/shared_ptr.hpp>
#include "../impl/make_const.hpp"
#ifdef TRIQS_WITH_PYTHON_SUPPORT
#include "./mem_block.hpp"
#else 
#include <vector>
//#include "basic_block.hpp"
#endif

namespace triqs { namespace arrays {
 namespace Tag {struct storage{}; struct shared_block:storage{}; }

 namespace storages {

  template <class V, class Opt> struct __init_value;
  template <class V> struct __init_value<V, Tag::no_init> { static void invoke (V * data, size_t s) {}};
  template <class V> struct __init_value< V, Tag::default_init> { 
   static void invoke (V * restrict data, size_t s) { if (data!=NULL) for (size_t u=0; u<s; ++u) data[u] = V();  }
  };
  template <class V> struct __init_value< V, Tag::nan_inf_init> { 
   static void invoke (V * restrict data, size_t s) {
    static_assert ( ( std::numeric_limits<V>::has_quiet_NaN || std::numeric_limits<V>::is_integer), "type has no Nan and is not integer. This init is impossible");
    if (data==NULL) return;
    if (std::numeric_limits<V>::has_quiet_NaN) for (size_t u=0; u<s; ++u) data[u] = std::numeric_limits<V>::quiet_NaN();
    if (std::numeric_limits<V>::is_integer)  for (size_t u=0; u<s; ++u) data[u] = std::numeric_limits<V>::max();
   }
  };

  template <class V> struct __init_value< std::complex<V>, Tag::nan_inf_init> { 
   static void invoke (std::complex<V> * restrict data, size_t s) {
    static_assert ( ( std::numeric_limits<V>::has_quiet_NaN || std::numeric_limits<V>::is_integer), "type has no Nan and is not integer. This init is impossible");
    if (data==NULL) return;
    if (std::numeric_limits<V>::has_quiet_NaN) for (size_t u=0; u<s; ++u) data[u] = std::complex<V>(std::numeric_limits<V>::quiet_NaN(),std::numeric_limits<V>::quiet_NaN()) ;
    if (std::numeric_limits<V>::is_integer)  for (size_t u=0; u<s; ++u) data[u] = std::complex<V>(std::numeric_limits<V>::max(),std::numeric_limits<V>::max());
   }
  };


  /*  Storage as a shared_ptr to a basic_block
   *  The shared pointer guarantees that the data will not be destroyed during the life of the array. 
   *  Impl: we are not using shared_array directly because of serialization support for shared_ptr */
  template<typename ValueType >
   class shared_block : Tag::shared_block { 
    typedef typename boost::add_const<ValueType>::type const_value_type;
    typedef typename boost::remove_const<ValueType>::type non_const_value_type;
#ifdef TRIQS_WITH_PYTHON_SUPPORT
    typedef details::mem_block<non_const_value_type> block_type;
#else
    //  typedef details::basic_block<non_const_value_type> block_type;
    typedef std::vector<non_const_value_type> block_type;
#endif

    public:
    typedef triqs::arrays::Tag::shared_block tag;
    typedef ValueType value_type;
    typedef shared_block<value_type> clone_type;
    typedef shared_block<const_value_type> const_clone_type;

    ///  Construct a new block of memory of given size
    template<typename InitOpt>
     explicit shared_block(size_t size, InitOpt opt ): sptr(size ? new block_type(size) : NULL) {
      init_data();
      __init_value<value_type,InitOpt>::invoke (this-> data_, this->size());
     }

    explicit shared_block(): sptr() { init_data(); }

#ifdef TRIQS_WITH_PYTHON_SUPPORT
    ///  Construct from a numpy object
    explicit shared_block(PyObject * arr, bool borrowed): sptr(new block_type(arr,borrowed)) { init_data();}
#endif

    /// Shallow copy
    shared_block(const shared_block<const_value_type> & X): sptr(X.sptr) { init_data(); }
  
    /// Shallow copy
    shared_block(const shared_block<non_const_value_type> & X): sptr(X.sptr) { init_data(); }

    void operator=(const shared_block & X) { sptr=X.sptr; init_data(); } 

    /// True copy of the data
    clone_type clone() const { 
     if (empty()) return clone_type ();
#ifdef TRIQS_WITH_PYTHON_SUPPORT    
     clone_type res; res.sptr = boost::make_shared<block_type > (*sptr); res.init_data();
#else
     clone_type res(this->size(), Tag::no_init() ); (*res.sptr) = (*sptr);
#endif
     return res;
    }

    // Make a clone forced to have const value_type
    const_clone_type const_clone() const {return clone();} 

    value_type & operator[](size_t i) const { return data_[i];}
    bool empty() const {return (sptr.get()==NULL);}
    size_t size() const {return (empty () ? 0 : sptr.get()->size());} 

#ifdef TRIQS_WITH_PYTHON_SUPPORT    
    PyObject * new_ref_to_guard() const {return sptr->new_ref_to_guard();}
#endif

    private:
    boost::shared_ptr<block_type > sptr;
    value_type * restrict data_; // for optimization on some compilers.
    void init_data(){ data_ = (sptr ? &((*sptr)[0]) : NULL); }
    friend class shared_block <non_const_value_type>; friend class shared_block <const_value_type>;
    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive & ar, const unsigned int version) { ar & boost::serialization::make_nvp("ptr",sptr); init_data(); }
   };
 }

 namespace details { 
  template<bool Const, typename T> struct make_const_type<Const,storages::shared_block<T> > { 
   typedef storages::shared_block<typename make_const_type<Const,T>::type> type;
  };
 }
}}//namespace triqs::arrays 
#endif

