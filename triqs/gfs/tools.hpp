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
#ifndef TRIQS_GF_TOOLS_H
#define TRIQS_GF_TOOLS_H
#include <triqs/utility/first_include.hpp>
#include <utility>
#include <boost/iterator/iterator_facade.hpp>
#include <triqs/clef.hpp>
#include <triqs/arrays/array.hpp>
#include <triqs/arrays/matrix.hpp>
#include <triqs/arrays/h5/simple_read_write.hpp>
#include "triqs/utility/complex_ops.hpp"
#include <triqs/utility/view_tools.hpp>
#include <triqs/utility/expression_template_tools.hpp>
#include <triqs/h5.hpp>

namespace triqs { namespace gfs {
 namespace tqa= triqs::arrays;
 namespace mpl=boost::mpl;

 namespace tag  { struct composite{}; struct mesh_point{};} 
 
 struct scalar_valued {};
 template<int R> struct tensor_valued {static_assert( R>0, "tensor_valued only for rank >0");};
 struct matrix_valued{};

 //------------------------------------------------------

 typedef std::complex<double> dcomplex; 

 enum statistic_enum {Boson,Fermion};

 struct freq_infty{}; // the point at infinity

 inline  std::vector<std::string> split(const std::string &s, char delim){
  std::vector<std::string> elems;
  std::stringstream ss(s);
  std::string item;
  while(std::getline(ss, item, delim)) { elems.push_back(item); }
  return elems;
 }
 //------------------------------------------------------

 template<typename ... T> struct closest_pt_wrap;
 
 template<typename T> struct closest_pt_wrap<T> : tag::mesh_point { T value; template<typename U> explicit closest_pt_wrap(U&&x): value(std::forward<U>(x)){}  };

 template<typename T1, typename T2, typename ...Ts > struct closest_pt_wrap<T1,T2,Ts...> : tag::mesh_point { 
  std::tuple<T1,T2,Ts...> value_tuple; 
  template<typename ... U> explicit closest_pt_wrap(U&&  ... x): value_tuple (std::forward<U>(x) ... ){}  
 };
 
 template<typename ... T> closest_pt_wrap<T...> closest_mesh_pt(T && ... x) { return closest_pt_wrap<T...> (std::forward<T>(x)...);}

 //------------------------------------------------------

 struct nothing {
  template<typename... Args> explicit nothing(Args&&...) {} // takes anything, do nothing..
  nothing() {}
  typedef nothing view_type;
  typedef nothing regular_type;
  void rebind (nothing){}
  template< typename RHS> void operator=(RHS && ) {}
  friend void h5_write (h5::group, std::string subgroup_name, nothing ) {}
  friend void h5_read  (h5::group, std::string subgroup_name, nothing ) {}
  //  BOOST Serialization
  friend class boost::serialization::access;
  template<class Archive>
   void serialize(Archive & ar, const unsigned int version) {
   }
  friend nothing operator +( nothing, nothing) { return nothing();}
  template<typename RHS> friend void assign_from_expression(nothing & ,RHS) {}
}; 

 template<typename T> nothing operator+(nothing, T const &) { return nothing();}
 template<typename T> nothing operator-(nothing, T const &) { return nothing();}
 template<typename T> nothing operator*(nothing, T const &) { return nothing();}
 template<typename T> nothing operator/(nothing, T const &) { return nothing();}
 template<typename T> TYPE_DISABLE_IF(nothing, std::is_same<T,nothing>) operator+(T const &, nothing) { return nothing();}
 template<typename T> TYPE_DISABLE_IF(nothing, std::is_same<T,nothing>) operator-(T const &, nothing) { return nothing();}
 template<typename T> TYPE_DISABLE_IF(nothing, std::is_same<T,nothing>) operator*(T const &, nothing) { return nothing();}
 template<typename T> TYPE_DISABLE_IF(nothing, std::is_same<T,nothing>) operator/(T const &, nothing) { return nothing();}
  
 //------------------------------------------------------

}}
#endif
