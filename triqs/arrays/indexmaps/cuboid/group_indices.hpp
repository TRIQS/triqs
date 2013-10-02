/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#ifndef TRIQS_ARRAYS_INDEXMAP_CUBOID_GROUP_INDICES_H
#define TRIQS_ARRAYS_INDEXMAP_CUBOID_GROUP_INDICES_H
#include <triqs/arrays/array.hpp>
namespace triqs { namespace arrays {

 // a variadic push_back into a vector.
 template<typename V> void vector_push_back_v(V & v){}
 template<typename V, typename T0, typename ... T> void vector_push_back_v(V & v, T0 && t0, T && ... t){ v.push_back(std::forward<T0>(t0)); vector_push_back_v(v,t...);}

 // a list of compile time int...
 template<int... Is> struct m_index{};
 template<int I0, int... Is> struct m_index<I0,Is...> {static constexpr int head = I0; };
 template<int... Is> std::vector<int> m_index_to_vector( m_index<Is...>) { std::vector<int> v; vector_push_back_v(v,Is...); return v;}

 // a trait to get the min, max of a m_index
 template<typename CTL> struct get_min_max;
 template<int I0, int I1,  int ... Is> struct get_min_max<m_index<I0, I1, Is...>> { 
  typedef get_min_max<m_index<I1,Is...>> r;
  static constexpr int min = (I0 < r::min ? I0 : r::min); static constexpr int max = (I0 > r::max ? I0 : r::max);
 };
 template<int I0> struct get_min_max<m_index<I0>> { static constexpr int min = I0; static constexpr int max = I0; };

 // given a m_index of indices, a metafunction to map to their position into memory
 template<ull_t ML, typename CTL> struct index_group_to_mem_pos_list;
 template<ull_t ML,int ... Is> struct index_group_to_mem_pos_list<ML,m_index<Is...>> {
  typedef m_index < indexmaps::mem_layout::index_to_memory_rank(ML,Is)...> type; 
  static_assert( get_min_max<type>::max - get_min_max<type>::min + 1 == sizeof...(Is), "Indices not contiguous in memory");
 };

#ifndef TRIQS_WORKAROUND_INTEL_COMPILER_BUGS
 // count the number of Is strictly smaller than C
 constexpr int count_pos(int C){ return 0;} 
 template<typename  ... T> constexpr int count_pos(int C, int i, T... t){ return (i<C ? 1 : 0) + count_pos(C, t...);} 
 // a metafunction to map this counting on a list 
 template<int ... Is> struct count_pos_list { typedef m_index< count_pos(Is, Is...)...> type; };
#else
 template<int C, typename CTL> struct count_pos;
 template<int C, int I0, int ... Is> struct count_pos<C,m_index<I0,Is...>> : std::integral_constant<int,  count_pos<C,m_index<Is...>>::value + (I0<C ? 1 : 0)> {};
 template<int C> struct count_pos<C,m_index<>> : std::integral_constant<int,0>{};
 template<int ... Is> struct count_pos_list { typedef m_index<Is...> tmp; typedef m_index< count_pos<Is, tmp>::value...> type; };
#endif

 // a simple foreach
 template<typename Callee, typename T0, typename ... T> void for_each (Callee & C, T0 const & t0, T const & ... t ) { C(t0); for_each(C, t...);}
 template<typename Callee> void for_each (Callee & C){} 

 // make a permutation out of a m_index
 template<typename CTL> struct to_permu;
 template<int ... Is> struct to_permu<m_index<Is...>> { static constexpr ull_t value =  permutations::permutation(Is...);};

 // the main class 
 template<typename A, typename ... MIndex> struct group_indices_impl { 
  static constexpr int new_dim = sizeof...(MIndex);
  typedef typename count_pos_list<  index_group_to_mem_pos_list<A::indexmap_type::traversal_order,MIndex>::type::head ...>::type new_memory_pos;

  static constexpr ull_t traversal_layout = to_permu<new_memory_pos>::value;
  typedef array_view<typename A::value_type, new_dim,0,traversal_layout > type; 

  static type invoke(A const & a) {
   if (a.indexmap().memory_indices_layout_ull() != a.indexmap().traversal_order) 
    TRIQS_RUNTIME_ERROR << "Grouping indices is only possible for arrays when the memory_layout is the same as the traversal order \n"
     <<	"But here your memory_layout is "<< a.indexmap().memory_indices_layout() << " while the traversal order is "<< a.indexmap().traversal_order_indices_layout();
   std::vector< std::vector<int> > Indices;
   vector_push_back_v(Indices, m_index_to_vector(MIndex())...);
   mini_vector<size_t, type::rank> l; 
   mini_vector<std::ptrdiff_t, type::rank> s;
   for (size_t u=0; u<type::rank; ++u) {l[u]=1; s[u]=0;}
   size_t i =0; 
   for (auto & m_index : Indices) { 
    bool first = true;
    for (auto & ind : m_index) {
     l[i] *= a.indexmap().lengths()[ind]; 
     s[i] = (first ? a.indexmap().strides()[ind]: std::min(s[i], a.indexmap().strides()[ind]));
     first = false;
    }
    ++i;
   }
   //std::cerr  << "strides "<< s << std::endl ;
   typename type::indexmap_type im(l,s,0);
   return type(im,a.storage());
  }
 };

 template<typename A, typename ... MIndex >
  typename group_indices_impl<A,MIndex... >::type group_indices(A const & a, MIndex... ) { return group_indices_impl<A,MIndex...>::invoke(a);}
}}//namespace triqs::arrays 
#endif
