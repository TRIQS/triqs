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
//#include "./matrix_view_proxy.hpp"
#include "../arrays/matrix_tensor_proxy.hpp"

namespace triqs { namespace gfs {

 //---------------------------- generic case array of dim R----------------------------------
 
 template<typename T, int R> struct data_proxy_array { 
  /// The storage
  typedef arrays::array<T, R> storage_t;
  typedef typename storage_t::view_type storage_view_t;
  typedef typename storage_t::const_view_type storage_const_view_t;

  /// The data access 
  auto operator()(storage_t& data, long i) const DECL_AND_RETURN(arrays::make_tensor_proxy(data, i));
  auto operator()(storage_t const& data, long i) const DECL_AND_RETURN(arrays::make_const_tensor_proxy(data, i));
  auto operator()(storage_view_t& data, long i) const DECL_AND_RETURN(arrays::make_tensor_proxy(data, i));
  auto operator()(storage_view_t const& data, long i) const DECL_AND_RETURN(arrays::make_const_tensor_proxy(data, i));
  auto operator()(storage_const_view_t& data, long i) const DECL_AND_RETURN(arrays::make_const_tensor_proxy(data, i));
  auto operator()(storage_const_view_t const& data, long i) const DECL_AND_RETURN(arrays::make_const_tensor_proxy(data, i));

#ifdef TRIQS_GF_DATA_PROXIES_WITH_SIMPLE_VIEWS
  auto operator()(storage_t            & data, size_t i) const DECL_AND_RETURN(data(i,arrays::ellipsis()));
  auto operator()(storage_t const      & data, size_t i) const DECL_AND_RETURN(data(i,arrays::ellipsis()));
  auto operator()(storage_view_t       & data, size_t i) const DECL_AND_RETURN(data(i,arrays::ellipsis()));
  auto operator()(storage_view_t const & data, size_t i) const DECL_AND_RETURN(data(i,arrays::ellipsis()));
  auto operator()(storage_const_view_t       & data, size_t i) const DECL_AND_RETURN(data(i,arrays::ellipsis()));
  auto operator()(storage_const_view_t const & data, size_t i) const DECL_AND_RETURN(data(i,arrays::ellipsis()));
#endif

  template<typename S, typename RHS> static void assign_to_scalar (S & data, RHS && rhs)           { data() = std::forward<RHS>(rhs);}
  template <typename ST, typename RHS> static void rebind(ST& data, RHS&& rhs) { data.rebind(rhs.data()); }
 };

 //---------------------------- 3d array : returns matrices in this case ! ----------------------------------
 
 template<typename T> struct data_proxy_array<T,3> { 
  /// The storage
  typedef arrays::array<T,3>             storage_t;
  typedef typename storage_t::view_type  storage_view_t;
  typedef typename storage_t::const_view_type storage_const_view_t;

  /// The data access
  auto operator()(storage_t& data, long i) const DECL_AND_RETURN(arrays::make_matrix_proxy(data, i));
  auto operator()(storage_t const& data, long i) const DECL_AND_RETURN(arrays::make_const_matrix_proxy(data, i));
  auto operator()(storage_view_t& data, long i) const DECL_AND_RETURN(arrays::make_matrix_proxy(data, i));
  auto operator()(storage_view_t const& data, long i) const DECL_AND_RETURN(arrays::make_const_matrix_proxy(data, i));
  auto operator()(storage_const_view_t& data, long i) const DECL_AND_RETURN(arrays::make_const_matrix_proxy(data, i));
  auto operator()(storage_const_view_t const& data, long i) const DECL_AND_RETURN(arrays::make_const_matrix_proxy(data, i));

#ifdef TRIQS_DATA_PROXIES_OLD_MATRIX_VIEW_PROXY
  arrays::matrix_view_proxy<storage_t,0>            operator()(storage_t       & data, size_t i) const { return arrays::matrix_view_proxy<storage_t,0>(data,i); } 
  arrays::const_matrix_view_proxy<storage_t,0>      operator()(storage_t const & data, size_t i) const { return arrays::const_matrix_view_proxy<storage_t,0>(data,i); } 
  arrays::matrix_view_proxy<storage_view_t,0>       operator()(storage_view_t       & data, size_t i) const { return arrays::matrix_view_proxy<storage_view_t,0>(data,i); } 
  arrays::const_matrix_view_proxy<storage_view_t,0> operator()(storage_view_t const & data, size_t i) const { return arrays::const_matrix_view_proxy<storage_view_t,0>(data,i); } 
#endif

#ifdef TRIQS_GF_DATA_PROXIES_WITH_SIMPLE_VIEWS
  auto operator()(storage_t            & data, size_t i) const DECL_AND_RETURN(data(i,arrays::ellipsis()));
  auto operator()(storage_t const      & data, size_t i) const DECL_AND_RETURN(data(i,arrays::ellipsis()));

  auto operator()(storage_view_t       & data, size_t i) const DECL_AND_RETURN(data(i,arrays::ellipsis()));
  auto operator()(storage_view_t const & data, size_t i) const DECL_AND_RETURN(data(i,arrays::ellipsis()));
  
  auto operator()(storage_const_view_t       & data, size_t i) const DECL_AND_RETURN(data(i,arrays::ellipsis()));
  auto operator()(storage_const_view_t const & data, size_t i) const DECL_AND_RETURN(data(i,arrays::ellipsis()));
#endif

  template <typename S, typename RHS> static void assign_to_scalar(S& data, RHS&& rhs) { data() = std::forward<RHS>(rhs); }
  template <typename ST, typename RHS> static void rebind(ST& data, RHS&& rhs) { data.rebind(rhs.data()); }
 };

 //---------------------------- 1d array ----------------------------------

 template<typename T> struct data_proxy_array<T,1>{ 
  /// The storage
  typedef arrays::array<T,1>             storage_t;
  typedef typename storage_t::view_type  storage_view_t;
  typedef typename storage_t::const_view_type storage_const_view_t;

  /// The data access 
  auto operator()(storage_t       & data,size_t i) const -> decltype(data(i)) { return data(i);}
  auto operator()(storage_t const & data,size_t i) const -> decltype(data(i)) { return data(i);}
  auto operator()(storage_view_t       & data,size_t i) const -> decltype(data(i)) { return data(i);}
  auto operator()(storage_view_t const & data,size_t i) const -> decltype(data(i)) { return data(i);}
  auto operator()(storage_const_view_t       & data,size_t i) const -> decltype(data(i)) { return data(i);}
  auto operator()(storage_const_view_t const & data,size_t i) const -> decltype(data(i)) { return data(i);}

  template<typename S, typename RHS> static void assign_to_scalar (S & data, RHS && rhs)           { data() = std::forward<RHS>(rhs);}
  template <typename ST, typename RHS> static void rebind(ST& data, RHS&& rhs) { data.rebind(rhs.data()); }
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

 template<typename T> struct data_proxy_vector { 
  typedef typename T::view_type Tv;
  typedef typename T::const_view_type Tcv;

  /// The storage
  typedef std::vector<T> storage_t;
  typedef std::vector<view_proxy<Tv>> storage_view_t;
  typedef std::vector<view_proxy<Tcv>> storage_const_view_t;

  /// The data access 
  T        &  operator()(storage_t  &           data, size_t i) { return data[i];}
  T  const &  operator()(storage_t      const & data, size_t i) const { return data[i];}
  Tv       &  operator()(storage_view_t &       data, size_t i)       { return data[i];}
  Tv const &  operator()(storage_view_t const & data, size_t i) const { return data[i];}
  Tcv       &  operator()(storage_const_view_t &       data, size_t i)       { return data[i];}
  Tcv const &  operator()(storage_const_view_t const & data, size_t i) const { return data[i];}
/*Tv operator()(storage_view_t &       data, size_t i) const      { return data[i];}
  Tv operator()(storage_view_t const & data, size_t i) const { return data[i];}
  Tcv operator()(storage_const_view_t &       data, size_t i) const      { return data[i];}
  Tcv operator()(storage_const_view_t const & data, size_t i) const { return data[i];}
*/

  template<typename S, typename RHS> static void assign_to_scalar   (S & data, RHS && rhs) {for (size_t i =0; i<data.size(); ++i) data[i] = rhs;}
  template <typename ST, typename RHS> static void rebind(ST& data, RHS&& rhs) { data.clear(); for (auto & x : rhs.data()) data.push_back(x);}
 };

 //---------------------------- lambda ----------------------------------

 template<typename F> struct data_proxy_lambda { 

  /// The storage
  typedef F storage_t;
  typedef F storage_view_t;
  typedef F storage_const_view_t;

  /// The data access 
  auto operator()(storage_t  &           data, size_t i)       DECL_AND_RETURN( data(i));
  auto operator()(storage_t      const & data, size_t i) const DECL_AND_RETURN( data(i));

  template<typename S, typename RHS> static void assign_to_scalar   (S & data, RHS && rhs) = delete;
  template <typename ST, typename RHS> static void rebind(ST& data, RHS&& rhs) = delete;
 };


}}
#endif

