/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by H. Hafermann, O. Parcollet
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
#pragma once
#include "./_field.hpp"

namespace triqs {
namespace params {

 template<typename T> struct no_default {};
 /**
  * Class for storing program parameters.
  * Parameters can be added to and extracted from the parameter object using the element access operator [].
  * Each element is stored by means of an object of type _field, which also stores the original type (all
  * integral types are collapsed to long and char* is collapsed to std::string).
  * When accessing elements, a typecheck is performed. Typecasts are allowed and are similar to the C++ rules
  * for casts. If the lvalue has type double, a cast from any integral type is allowed. If the lvalue has
  * arithmetic type, a boost::lexical_cast is attempted.
  * The class is boost-serializable and implements hdf5 I/O operations.
  */
 class parameters {
  struct _data_elem {
   std::string key;
   _field f;
   std::string doc;
   template <class Archive> void serialize(Archive& ar, const unsigned int version) {
    ar& TRIQS_MAKE_NVP("key", key) & TRIQS_MAKE_NVP("f", f) & TRIQS_MAKE_NVP("doc", doc);
   }
  };
  using _data_t = std::vector<_data_elem>;
  _data_t _data;
  void insert(std::string const& key, _field&& f, std::string const& doc);
  _data_t::iterator find(std::string const& key);
  _data_t::const_iterator find(std::string const& key) const;

  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive& ar, const unsigned int version) { ar& TRIQS_MAKE_NVP("_data", _data); }

  public:
  parameters();

  /// calls can be chained for multiple parameters
  template <typename T> parameters& add_field(std::string const& key, T&& x, std::string const& doc) {
   insert(key, _field{std::forward<T>(x),key, false}, doc);
   return *this;
  }

  // add a field without a default value
  template <typename T> parameters& add_field(std::string const& key, no_default<T>, std::string const& doc) {
   insert(key, _field{T{}, key, true}, doc);
   return *this;
  }

  parameters& add_group(std::string const& key, std::string const& doc) {
   insert(key, _field{parameters{}, key, false}, doc);
   return *this;
  }

  void sort_by_key();

  bool has_key(std::string const& k) const;

  /// Access the parameter key, which must be present (or it throws an exception).
  _field& operator[](const char * key);
  _field const& operator[](const char * key) const;

  /// generate help in form of a table of strings containing a list of required and optional parameters
  std::vector<std::vector<std::string>> generate_help() const;

  friend std::string get_triqs_hdf5_data_scheme(parameters) { return ""; }
  friend void h5_write(h5::group F, std::string const& subgroup_name, parameters const& p);
  friend void h5_read(h5::group F, std::string const& subgroup_name, parameters& p);
  friend std::ostream& operator<<(std::ostream& out, parameters const& p);

  /**
   * Update with another parameter set.
   * If a key is present in other and not in this, add parameter to this.
   * If a key is present in both, overwrite parameter in this without any check (Python-like behaviour).
   */
  void update(parameters const&);
 };

 inline parameters operator+(parameters p1, parameters const& p2) {
  p1.update(p2);
  return p1;
 }
 // can only be implemented after complete declaration of parameters
 template <typename... T> _field& _field::add_field(T&&... x) {
  auto* pp = dynamic_cast<_data_impl<parameters>*>(p.get());
  if (!pp) TRIQS_RUNTIME_ERROR << "Can only use add_field on a subgroup, which " << name() << " is not !";
  pp->x.add_field(std::forward<T>(x)...);
  return *this;
 }
}
}
#include <pytriqs/converters/parameters.hpp>
