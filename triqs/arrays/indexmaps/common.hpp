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
#ifndef TRIQS_ARRAYS_INDEXMAP_COMMON_H
#define TRIQS_ARRAYS_INDEXMAP_COMMON_H
#include "../impl/common.hpp"
#include <triqs/utility/mini_vector.hpp>
//#include "../impl/tuple_tools.hpp"
#include <boost/ref.hpp>
#include <iostream>

namespace boost { namespace serialization { class access;}}
namespace triqs { namespace arrays { namespace Tag {struct indexmap{}; }}}

namespace triqs { namespace arrays { 
 using utility::mini_vector;

 // make_shape
 // generalize with preproc or variadic template
//#define IMPL(z, NN, unused)                                \
// template <typename T> mini_vector<size_t,BOOST_PP_INC(NN)> make_shape(BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(NN), T I_)) \
// { return mini_vector<size_t,BOOST_PP_INC(NN)>(BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(NN), I_));} 
// BOOST_PP_REPEAT(ARRAY_NRANK_MAX , IMPL, nil)
//#undef IMPL
 template<typename... T> 
  mini_vector<size_t, sizeof...(T)+1> make_shape(size_t x0, T... args) { return  mini_vector<size_t, sizeof...(T)+1> (x0,args...);}

 namespace indexmaps { 

 // to be specialized for all IndexMap types.
 template<typename IndexMap, typename... Args> struct slicer; 

 /// Returns whether the 2 indexMaps are compatible with A op B (op : = , +, etc...)
 template< typename IndexMap1, typename IndexMap2>
  bool compatible_for_binary_ops (const IndexMap1 &, const IndexMap2 &);

 // The generator of indices for an index domain.
 template<typename Domain, typename Parameter> class indices_generator; 

 /**
  * Is a raw copy possible between 2 arrays with these indexMaps ?
  * They have to have the same ordering, and be contiguous typically. 
  */
 template< typename IndexMap1, typename IndexMap2>
  bool raw_copy_possible (const IndexMap1 &, const IndexMap2 &);

 /**
  * Metafunction. 
  * Given an IndexMapIterator and an IndexMap, 
  * ::type should be the iterator of IndexMap which traverses
  * the indices in the same order as IndexMapIterator.
  * Example : 
  *  - IndexMapIterator is a parallelepipedon_iterator < SomeParallelepipedon, So
  meIterationOrder>
  *  - IndexMap is AnotherParallelepipedon (e.g. with a different memory storage)
  .
  *  - type is then parallelepipedon_iterator < AnotherParallelepipedon, SomeIter
  ationOrder>
  *  Application example : 
  *     A = B with A, B in different order in memory.
  *     One takes the optimized iterator on A, this defines the order of indices 
  traversal, 
  *     and use indexmap_iterator_adapter to deduce the iterator on B that traver
  se the indices in the same way. 
  */
 //template <typename IndexMapIterator, typename IndexMap>
 // struct indexmap_iterator_adapter;

 }}}//namespace triqs::arrays 
#endif


