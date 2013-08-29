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
#ifndef TRIQS_ARRAYS_INDEXMAP_CUBOID_FOREACH_H
#define TRIQS_ARRAYS_INDEXMAP_CUBOID_FOREACH_H
#include "./map.hpp"

namespace triqs { namespace arrays {

 template<typename A, typename Enable = void> struct get_traversal_order : indexmaps::mem_layout::fortran_order_tr<A::domain_type::rank>{};
 //template<typename A, typename Enable = void> struct get_traversal_order : indexmaps::mem_layout::c_order_tr<A::domain_type::rank>{};
 template<typename A> struct get_traversal_order<A,typename A::indexmap_type::has_traversal_order_tag> : std::integral_constant<ull_t, A::indexmap_type::traversal_order>{};

 template <typename T, typename Function>
  typename std::enable_if<ImmutableCuboidArray<T>::value >::type
  foreach (T const & x, Function const & F) { indexmaps::cuboid::foreach<get_traversal_order<T>::value> (x.domain(), F); }

 template <typename T, typename Function>
  struct assign_foreach_adapter {
   T& x; Function const & f;
   assign_foreach_adapter( T& x_, Function const & ff): x(x_), f(ff){}
   template<typename ... Args> void operator()(Args const & ... args) const { x(args...) = f(args...);}
  };

 template <typename T, typename Function>
  typename std::enable_if<MutableCuboidArray<T>::value >::type
  assign_foreach (T & x, Function const & F) { indexmaps::cuboid::foreach<get_traversal_order<T>::value> (x.domain(),assign_foreach_adapter<T,Function>(x,F)); }

}}//namespace
#endif
