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
#ifndef TRIQS_UTILITY_VALUE_VIEW_H
#define TRIQS_UTILITY_VALUE_VIEW_H
#include <memory>
#include <triqs/arrays/array.hpp>
#include <triqs/arrays/h5.hpp>

namespace triqs { 

 /// a class to keep a value and the view on it in a manner similar to arrays, gf, etc...
 template<typename T, bool IsView > class value_view;

 template<typename T> class value_view<T,true> {

  std::shared_ptr<T> p;
  friend class value_view<T,false>;

  public:
  typedef void has_view_type_tag; // Idiom : ValueView 
  typedef value_view<T,true>  view_type;
  typedef value_view<T,false> regular_type;
  typedef typename std::remove_cv<typename std::remove_reference <T>::type >::type value_type;
  value_view( regular_type const & a) { p = a.p;}
  value_view( view_type const & a) { p = a.p;}
  void rebind( view_type const &X) { p = X.p; }

  T & operator()() { return *p;}
  T const & operator()() const { return *p;}
  operator T() { return  *p;}
  operator T const() const { return *p;} 
  value_view & operator=(view_type     const & x) { if (!p) rebind(x); else { *p = *x.p;} return *this;}
  value_view & operator=(regular_type const & x) { if (!p) rebind(x); else { *p = *x.p;} return *this;}
  value_view & operator=(T const & x) { *p = x; return *this;}

  friend void h5_write (arrays::h5::group_or_file fg, std::string p, value_view const & v) { h5_write(fg,p,v()); }
  friend void h5_read  (arrays::h5::group_or_file fg, std::string p, value_view       & v) { h5_read (fg,p,v()); }
  // Boost serialization. Can I use std::shared_ptr directly ? Or use boost::shared_ptr
  friend class boost::serialization::access;
  template<class Archive>
   void save(Archive & ar, const unsigned int version) const { ar << boost::serialization::make_nvp("data",*p); }
  template<class Archive>
   void load(Archive & ar, const unsigned int version) { T x; ar >> x; p = std::make_shared<T>(x); }
  BOOST_SERIALIZATION_SPLIT_MEMBER();
 };

 template<typename T> class value_view<T,false> {
  std::shared_ptr<T> p;
  friend class value_view<T,true>;
  public:
  typedef void has_view_type_tag; // Idiom : ValueView 
  typedef value_view<T,true>  view_type;
  typedef value_view<T,false> regular_type;
  typedef typename std::remove_cv<typename std::remove_reference <T>::type >::type value_type;
  value_view( regular_type const & a) { p = std::make_shared<T>(*a.p);}
  value_view( view_type const & a) { p = std::make_shared<T>(*a.p);}
  template<typename... Args> explicit value_view(Args&&... args) {p = std::make_shared<T>(std::forward<Args>(args)...);}
  T & operator()() { return *p;}
  T const & operator()() const { return *p;}
  operator T() { return  *p;}
  operator T const() const { return *p;} 
  value_view& operator=(view_type     const & x) { *p = *x.p;return *this;}
  value_view& operator=(regular_type const & x) { *p = *x.p;return *this;}
  value_view& operator=(T const & x)             { *p = x;   return *this;}

  friend void h5_write (arrays::h5::group_or_file fg, std::string p, value_view const & v) { h5_write(fg,p,v()); }
  friend void h5_read (arrays::h5::group_or_file fg, std::string p, value_view & v) { h5_read(fg,p,v()); }

  // Boost serialization. Can I use std::shared_ptr directly ? Or use boost::shared_ptr
  friend class boost::serialization::access;
  template<class Archive>
   void save(Archive & ar, const unsigned int version) const { ar << boost::serialization::make_nvp("data",*p); }
  template<class Archive>
   void load(Archive & ar, const unsigned int version) { T x; ar >> x; p = std::make_shared<T>(x); }
  BOOST_SERIALIZATION_SPLIT_MEMBER();
 };

}//namespace triqs
#endif

