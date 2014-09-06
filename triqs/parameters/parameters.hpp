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

 /// For add_field method of parameters.
 template<typename T> struct no_default {};

 /**
  * DOC to be written.
  *
  * Provide a form for program parameters.
  *
  * Fields of the form can be added with add_field method (only in C++), with a name, a type, a doc, and optionally a default value.
  *
  * They can be retrieved from and put into the form, using the [] operator, in C++ and python.
  *
  * The data is stored with a strict type checking at runtime, with some type collapse (all integers into a long,
  *  all string, const char * into a std::string, etc).
  *
  * Basic type cast to basic types are provided.
  *
  * The class is boost-serializable and implements hdf5 I/O operations.
  */
 class parameters {
  struct _data_elem {
   std::string key;
   _field f;
   std::string doc;
#ifdef TRIQS_SERIALIZATION_WITH_HDF5_IMPOSSIBLE
   template <class Archive> void serialize(Archive& ar, const unsigned int version) {
    ar& TRIQS_MAKE_NVP("key", key) & TRIQS_MAKE_NVP("f", f) & TRIQS_MAKE_NVP("doc", doc);
   }
#endif
  };
  using _data_t = std::vector<_data_elem>;
  _data_t _data;
  void insert(std::string const& key, _field&& f, std::string const& doc);
  _data_t::iterator find(std::string const& key);
  _data_t::const_iterator find(std::string const& key) const;

  friend class boost::serialization::access;
#ifdef TRIQS_SERIALIZATION_WITH_HDF5_IMPOSSIBLE
  template <class Archive> void serialize(Archive& ar, const unsigned int version) {
   ar& TRIQS_MAKE_NVP("_data", _data);
  }
#else
  // do it at once, with one buffer only.
  template <class Archive> void save(Archive &ar, const unsigned int version) const {
   std::string s = h5::serialize(*this);
   ar << TRIQS_MAKE_NVP("seria_str", s);
  }
  template <class Archive> void load(Archive &ar, const unsigned int version) {
   std::string s;
   ar >> TRIQS_MAKE_NVP("seria_str", s);
   *this = h5::deserialize<parameters>(s);
  }
  BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

  public:
  parameters();

  /// Add a field with a default value
  template <typename T> parameters& add_field(std::string const& key, T&& x, std::string const& doc) {
   insert(key, _field{std::forward<T>(x),key, false}, doc);
   return *this;
  }

  // Add a field without a default value
  template <typename T> parameters& add_field(std::string const& key, no_default<T>, std::string const& doc) {
   insert(key, _field{T{}, key, true}, doc);
   return *this;
  }

  /// Add a subgroup (a field which it itself a parameter).
  parameters& add_group(std::string const& key, std::string const& doc) {
   insert(key, _field{parameters{}, key, false}, doc);
   return *this;
  }

  /// Sort with the key. By default the list of fields is ordered as in the order of their addition to the parameter.
  void sort_by_key();

  /// Does the form have the key ?
  bool has_key(std::string const& key) const;

  /**
   * Access the parameter key.
   * Key must be a valid key or a TRIQS_RUNTIME_ERROR is thrown.
   */
  _field& operator[](const char * key);

  /**
   * Access the parameter key.
   * Key must be a valid key or a TRIQS_RUNTIME_ERROR is thrown.
   */
  _field const& operator[](const char * key) const;

  /// generate help in form of a table of strings containing a list of required and optional parameters
  std::vector<std::vector<std::string>> generate_help(bool with_header = true) const;

  /// help as a string
  std::string help() const;

  /// hdf5
  friend std::string get_triqs_hdf5_data_scheme(parameters const &) { return ""; }
  friend void h5_write(h5::group F, std::string const& subgroup_name, parameters const& p);
  friend void h5_read(h5::group F, std::string const& subgroup_name, parameters& p);

  /// Ostream
  friend std::ostream& operator<<(std::ostream& out, parameters const& p);

  /**
   * Update with another parameter set.
   * If a key is present in other and not in this, add parameter to this.
   * If a key is present in both, overwrite parameter with a strict type checking.
   */
  void update(parameters const&);
 };

 /// Another form of update, more pythonic
 inline parameters operator+(parameters p1, parameters const& p2) {
  p1.update(p2);
  return p1;
 }

 inline parameters & operator+=(parameters & p1, parameters const& p2) {
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

#ifdef TRIQS_WITH_PYTHON_SUPPORT
// include the generated python converter
#include <pytriqs/converters/parameters.hpp>
#endif

