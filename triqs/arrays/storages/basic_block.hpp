
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

#ifndef TRIQS_ARRAYS_BASIC_BLOCK_H
#define TRIQS_ARRAYS_BASIC_BLOCK_H
#include <boost/noncopyable.hpp>

// A minimal basic block : replace by a vector<> or is it quicker ?
namespace triqs { namespace arrays { namespace storages { namespace details { 

 template<typename ValueType >
  struct basic_block : boost::noncopyable {
   typedef typename boost::remove_const<ValueType>::type non_const_value_type;
   size_t size_;
   non_const_value_type * restrict p;

   basic_block():size_(0),p(NULL){}

   basic_block (size_t s):size_(s),p(new non_const_value_type[s]) { }

   ~basic_block(){if (p) {delete[] p; } }

   void operator=(const basic_block & X) {
    assert(size_==X.size_);assert(p); assert(X.p);
    memcpy (p,X.p,size_ * sizeof(ValueType));
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
}}}}//namespace triqs::arrays ...
#endif

