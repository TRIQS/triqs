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
#include "./mem_block.hpp"

namespace triqs { namespace arrays { namespace storages {

 /*
  * This is a shared memory block, similar to a shared_ptr<mem_block>
  * but a lot faster and adapted to model the Storage concept.  
  */
 template<typename ValueType, bool Weak=false>
  class shared_block {
   static_assert( !std::is_const<ValueType>::value, "oops : internal error : shared_block should never be instanciated with a const"); 

   mem_block<ValueType> * sptr;
   ValueType * restrict data_; // for optimization on some compilers. ?? obsolete : to be removed ?
   size_t s;

   void construct_delegate(size_t size) { 
    s = size;
    if (size ==0) { sptr = nullptr; data_=nullptr; }
    else { sptr = new mem_block<ValueType>(size); data_ = sptr->p;}
   }

   public:
   typedef ValueType value_type;
   static constexpr bool is_weak = Weak;

   ///  Construct a new block of memory of given size
   explicit shared_block(size_t size, Tag::no_init) { construct_delegate(size); }

   explicit shared_block(size_t size, Tag::default_init) {// C++11  : delegate to previous constructor when gcc 4.6 support is out. 
    construct_delegate(size); 
    const auto s = this->size(); for (size_t u=0; u<s; ++u) data_[u] = ValueType(); 
   }

#ifdef TRIQS_WITH_PYTHON_SUPPORT
   explicit shared_block(PyObject * arr, bool weak): sptr(new mem_block<ValueType>(arr,weak)) { data_ = sptr->p; s= sptr->size(); }
#endif

   explicit shared_block() { sptr =nullptr; data_=nullptr; s=0; }

   shared_block(shared_block const & X) noexcept { sptr =X.sptr; data_ = X.data_; s= X.s; if (sptr) inc_ref<Weak>(sptr);  }
   shared_block(shared_block<ValueType,!Weak> const & X) noexcept { sptr =X.sptr; data_ = X.data_; s= X.s; if (sptr) inc_ref<Weak>(sptr);  }

   shared_block(shared_block && X)      noexcept { sptr =X.sptr; data_ = X.data_; s= X.s; X.sptr=nullptr; }

   ~shared_block() { if (sptr) dec_ref<Weak>(sptr); }

   shared_block & operator=(shared_block const & X) noexcept { 
    if (sptr) dec_ref<Weak>(sptr); sptr =X.sptr; if (sptr) inc_ref<Weak>(sptr); 
    data_ = X.data_; s = X.s; 
    return *this;
   }
   
   shared_block & operator=(shared_block && X) noexcept { 
    if (sptr) dec_ref<Weak>(sptr); sptr =X.sptr; X.sptr=nullptr; 
    data_ = X.data_; s = X.s; 
    return *this;
   }

   /// True copy of the data
   shared_block clone() const { 
    shared_block res; 
    if (!empty()) { res.sptr = new mem_block<ValueType> (*sptr); res.data_ = res.sptr->p; res.s = s;}
    return res;
   }

   value_type & operator[](size_t i) const { return data_[i];}
   bool empty() const {return (sptr==nullptr);}
   size_t size() const {return s;}
   //size_t size() const {return (empty () ? 0 : sptr->size());} 

#ifdef TRIQS_WITH_PYTHON_SUPPORT    
   PyObject * new_python_ref() const {return sptr->new_python_ref();}
#endif

   private:
   friend class shared_block<ValueType,!Weak>;
   friend class boost::serialization::access;
   template<class Archive>
    void save(Archive & ar, const unsigned int version) const { ar << boost::serialization::make_nvp("ptr",sptr); }
   template<class Archive>
    void load(Archive & ar, const unsigned int version) { 
     if (sptr) dec_ref<Weak>(sptr); 
     ar >> boost::serialization::make_nvp("ptr",sptr); data_ = (sptr ? sptr->p : nullptr); s = (sptr ? sptr->size() : 0);
     if (sptr) inc_ref<Weak>(sptr);
    }
   BOOST_SERIALIZATION_SPLIT_MEMBER();
  };

}}}//namespace  
#endif

