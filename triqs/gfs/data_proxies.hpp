/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
#ifndef TRIQS_GF_DATA_PROXIES_H
#define TRIQS_GF_DATA_PROXIES_H
#include <triqs/utility/first_include.hpp>
#include <utility>
#include <triqs/arrays.hpp>
#include "../arrays/matrix_tensor_proxy.hpp"

#define TRIQS_GF_DATA_PROXIES_WITH_SIMPLE_VIEWS

namespace triqs { namespace gfs {

 //---------------------------- common stuff for array proxies ----------------------------------

 template <typename T, int D> struct data_proxy_array_common {
  using storage_t = arrays::array<T, D>;
  using storage_view_t = typename storage_t::view_type;
  using storage_const_view_t = typename storage_t::const_view_type;

  // from the shape of the mesh and the target, make the shape of the array. default is to glue them
  template <typename S1, typename S2> static auto join_shape(S1 const& s1, S2 const& s2) RETURN(join(s1, s2));

  template<typename S, typename RHS> static void assign_to_scalar (S & data, RHS && rhs)           { data() = std::forward<RHS>(rhs);}
  template <typename ST, typename RHS> static void rebind(ST& data, RHS&& rhs) { data.rebind(rhs.data()); }
 };

 //---------------------------- generic case array of dim R----------------------------------

 template <typename T, int R> struct data_proxy_array : data_proxy_array_common<T, R> {
  using B = data_proxy_array_common<T, R>;
/// The data access
#ifdef TRIQS_GF_DATA_PROXIES_WITH_SIMPLE_VIEWS
  template <typename S> auto operator()(S& data, long i) const DECL_AND_RETURN(data(i, arrays::ellipsis()));
#else
  auto operator()(B::storage_t& data, long i) const DECL_AND_RETURN(arrays::make_tensor_proxy(data, i));
  auto operator()(B::storage_t const& data, long i) const DECL_AND_RETURN(arrays::make_const_tensor_proxy(data, i));
  auto operator()(B::storage_view_t& data, long i) const DECL_AND_RETURN(arrays::make_tensor_proxy(data, i));
  auto operator()(B::storage_view_t const& data, long i) const DECL_AND_RETURN(arrays::make_const_tensor_proxy(data, i));
  auto operator()(B::storage_const_view_t& data, long i) const DECL_AND_RETURN(arrays::make_const_tensor_proxy(data, i));
  auto operator()(B::storage_const_view_t const& data, long i) const DECL_AND_RETURN(arrays::make_const_tensor_proxy(data, i));
#endif
 };

 //---------------------------- 3d array : returns matrices in this case ! ----------------------------------

 template <typename T> struct data_proxy_array<T, 3> : data_proxy_array_common<T, 3> {
  using B = data_proxy_array_common<T, 3>;
#ifdef TRIQS_GF_DATA_PROXIES_WITH_SIMPLE_VIEWS
  template <typename S> auto operator()(S & data, long i) const RETURN(make_matrix_view(data(i, arrays::ellipsis())));
#else
  /// The data access
  auto operator()(B::storage_t& data, long i) const DECL_AND_RETURN(arrays::make_matrix_proxy(data, i));
  auto operator()(B::storage_t const& data, long i) const DECL_AND_RETURN(arrays::make_const_matrix_proxy(data, i));
  auto operator()(B::storage_view_t& data, long i) const DECL_AND_RETURN(arrays::make_matrix_proxy(data, i));
  auto operator()(B::storage_view_t const& data, long i) const DECL_AND_RETURN(arrays::make_const_matrix_proxy(data, i));
  auto operator()(B::storage_const_view_t& data, long i) const DECL_AND_RETURN(arrays::make_const_matrix_proxy(data, i));
  auto operator()(B::storage_const_view_t const& data, long i) const DECL_AND_RETURN(arrays::make_const_matrix_proxy(data, i));
#endif

 };

 //---------------------------- 1d array ----------------------------------

 template <typename T> struct data_proxy_array<T, 1> : data_proxy_array_common<T, 1> {
  template <typename S> AUTO_DECL operator()(S& data, long i) const RETURN(data(i));
 };

 //---------------------------- multi variable ----------------------------------

 template <typename T, int TotalDim> struct data_proxy_array_multivar : data_proxy_array_common<T, TotalDim> {
  // using the standard technique from tuple::apply with a sequence
  template <typename S, typename Tu, size_t... Is>
  AUTO_DECL _impl(S& data, Tu const& tu, std14::index_sequence<Is...>) const RETURN(data(std::get<Is>(tu)..., arrays::ellipsis()));
  template <typename S, typename Tu>
  AUTO_DECL operator()(S& data, Tu const& tu) const RETURN(_impl(data, tu, triqs::tuple::_get_seq<Tu>()));
 };

 //---------------------------- multi variable ----------------------------------

 template <typename T, int TotalDim> struct data_proxy_array_multivar_matrix_valued : data_proxy_array_common<T, TotalDim> {
  // using the standard technique from tuple::apply with a sequence
  template <typename S, typename Tu, size_t... Is>
  AUTO_DECL _impl(S& data, Tu const& tu, std14::index_sequence<Is...>) const RETURN(make_matrix_view(data(std::get<Is>(tu)..., arrays::range(), arrays::range())));
  template <typename S, typename Tu>
  AUTO_DECL operator()(S& data, Tu const& tu) const RETURN(_impl(data, tu, triqs::tuple::_get_seq<Tu>()));
 };

 //---------------------------- multi variable with index mixer----------------------------------
 template <typename T, int Nvar, int TargetDim, typename IndexMixer>
 struct data_proxy_array_index_mixer : data_proxy_array_common<T, Nvar + TargetDim> {

  template <typename S1, typename S2> static utility::mini_vector<int, Nvar + TargetDim> join_shape(S1 const& s1, S2 const& s2) {
   return tuple::apply_construct_parenthesis<utility::mini_vector<int, Nvar + TargetDim>>(IndexMixer::invoke(s1, s2));
  }

  template <typename S, typename Tu> auto operator()(S& data, Tu const& tu) const {
   return tuple::apply(data, IndexMixer::invoke(tu, tuple::make_tuple_repeat<TargetDim>(arrays::range())));
  }
 };

 //---------------------------- vector ----------------------------------

 template<typename V> struct view_proxy : public V { 
  view_proxy() : V(typename V::regular_type()) {}
  view_proxy(V const &v) : V(v){};
  view_proxy(view_proxy const & p) : V(p) {};
  template<typename ... Args> explicit view_proxy(Args && ... args) : V (std::forward<Args>(args)...){}
  view_proxy & operator = ( view_proxy const & cp ) { this->rebind(cp); return *this;}
  view_proxy & operator = ( V          const & v  ) { this->rebind(v);  return *this;}
  using V::operator=;
  //template<typename X> view_proxy & operator = (X && x) { V::operator=( std::forward<X>(x) ); return *this;} 
 };

 template <typename T> struct data_proxy_vector {
  using Tv = typename T::view_type;
  using Tcv = typename T::const_view_type;

  /// The storage
  using storage_t = std::vector<T>;
  using storage_view_t = std::vector<view_proxy<Tv>>;
  using storage_const_view_t = std::vector<view_proxy<Tcv>>;

  /// The data access
  template <typename S> AUTO_DECL operator()(S& data, size_t i) const  RETURN(data[i]);

  /*
  T        &  operator()(storage_t  &           data, size_t i) { return data[i];}
  T  const &  operator()(storage_t      const & data, size_t i) const { return data[i];}
  Tv       &  operator()(storage_view_t &       data, size_t i)       { return data[i];}
  Tv const &  operator()(storage_view_t const & data, size_t i) const { return data[i];}
  Tcv       &  operator()(storage_const_view_t &       data, size_t i)       { return data[i];}
  Tcv const &  operator()(storage_const_view_t const & data, size_t i) const { return data[i];}
*/

  template<typename S, typename RHS> static void assign_to_scalar   (S & data, RHS && rhs) {for (size_t i =0; i<data.size(); ++i) data[i] = rhs;}
  template <typename ST, typename RHS> static void rebind(ST& data, RHS&& rhs) { data.clear(); for (auto & x : rhs.data()) data.push_back(x);}
 };

 //---------------------------- lambda ----------------------------------

 template <typename F> struct data_proxy_lambda {

  /// The storage
  using storage_t = F;
  using storage_view_t = F;
  using storage_const_view_t = F;

  /// The data access
  template <typename S, typename ... I> AUTO_DECL operator()(S& data, I const& ...i) const RETURN(data(i...));

  template<typename S, typename RHS> static void assign_to_scalar   (S & data, RHS && rhs) = delete;
  template <typename ST, typename RHS> static void rebind(ST& data, RHS&& rhs) = delete;
 };


}}
#endif

