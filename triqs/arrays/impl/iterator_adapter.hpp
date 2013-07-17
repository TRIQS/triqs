
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

#ifndef TRIQS_ARRAYS_ARRAY_ITERATOR_H
#define TRIQS_ARRAYS_ARRAY_ITERATOR_H
#include "./make_const.hpp"
#include <boost/iterator/iterator_facade.hpp>

namespace triqs { namespace arrays { 
 /** 
  * Iterator on a couple (IndexMap, StorageType).
  * It transforms the mini-iterator on the IndexMap into a STL-compliant iterator.
  * Template parameters are :
  *    - Const : true -> const_iterator, false -> non const version. 
  *    - IndexMapIterator : an IndexMapIterator
  *    - StorageType : the storage.
  */

 template <bool Const, typename IndexMapIterator, typename StorageType > 
  class iterator_adapter : 
   public boost::iterator_facade< 
   iterator_adapter<Const,IndexMapIterator,StorageType>,
   typename details::make_const_type<Const,typename StorageType::value_type>::type,
   boost::forward_traversal_tag > 
 {
  public:
   typedef typename details::make_const_type<Const,typename StorageType::value_type>::type value_type;
   typedef IndexMapIterator indexmap_iterator_type;

   iterator_adapter(): storage_(), it() {}
   iterator_adapter(const typename IndexMapIterator::indexmap_type & Ind, const StorageType & ST, bool atEnd=false): 
    	storage_(ST), it(Ind,atEnd) {} 

   operator bool() const { return bool(it);}

   typename IndexMapIterator::indexmap_type::domain_type::index_value_type const & indices() const {return it.indices();} 
   IndexMapIterator const & indexmap_iterator() const { return it;}

  private:
   friend class boost::iterator_core_access;
   void increment() { ++it; }
   value_type & dereference() const { return storage_[*it];}
   bool equal(iterator_adapter const & other) const { return (other.it == it);}

   StorageType storage_;
   IndexMapIterator it;   
 };

}}//namespace triqs::arrays
#endif
