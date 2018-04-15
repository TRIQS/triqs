/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013, P. Seth, I. Krivenko, M. Ferrero and O. Parcollet
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
#include <variant>
#include <string>
#include <triqs/utility/dressed_iterator.hpp>
#include <triqs/utility/exceptions.hpp>
#include <triqs/h5/base_public.hpp>
#include <vector>
#include <set>
#include <map>
#include <variant>

namespace std {

 // == ostream operator<< for variant and vector of variant

 template<typename T1, typename ...T>
 std::ostream &operator<<(std::ostream &os, std::variant<T1, T...> const &v) {
   visit([&os](auto const &x) { os << x; }, v);
   return os;
 }

 inline std::ostream & operator<<(std::ostream & os, std::vector<std::variant<int, std::string>> const& fs) {
  int u = 0;
  for(auto const& i : fs) { if (u++) os << ","; os << i; }
  return os;
 }

 // == Make std::to_string available for both string and variant

 inline string to_string(string const &str) { return str; }
 template<typename T1, typename T2>
 inline string to_string(variant<T1, T2> const &var) {
   stringstream ss;
   ss << var;
   return ss.str();
 }

} // namespace std

namespace triqs {
namespace hilbert_space {

/// Structure of the Green's function
using gf_struct_t = std::vector<std::pair<std::string,std::vector<std::variant<int, std::string>>>>;

/// This class represents an ordered set of **indices** of the canonical operators (see [[many_body_operator]]) used to build the Fock states.
/**
 Every element of the set is an arbitrarily long sequence of integers/strings (types can be mixed within one sequence).
 The elements are ordered according to the result of `std::lexicographical_compare`.
 @include triqs/hilbert_space/fundamental_operator_set.hpp
 */
class fundamental_operator_set {
 public:

 /// Sequence of indices (`std::vector` of int/string variant objects)
 using indices_t = std::vector<std::variant<int, std::string>>;

 /// The set represented as a plain `std::vector`
 using reduction_t = std::vector<indices_t>;

 private:
 using map_t = std::map<indices_t, int>; // the table index <-> n
 map_t map_index_n;

 // internal only
 fundamental_operator_set(std::vector<std::vector<std::string>> const&);

 public:

 /// Construct an empty set
 fundamental_operator_set() {}

 /// Construct a set with each stored index being a pair of integers `(i,j)`
 /**
   @param v `i` runs from 0 to `v.size()-1`; `j` runs from 0 to `v[i].size()-1` for each `i`
  */
 fundamental_operator_set(std::vector<int> const& v) {
  for (int i = 0; i < v.size(); ++i)
   for (int j = 0; j < v[i]; ++j) insert(i, j);
 }

 /// Construct from a set of generic index sequences
 /**
   @param s Set of indices
  */
 template <typename IndexType> fundamental_operator_set(std::set<IndexType> const& s) {
  for (auto const& i : s) insert(i);
 }

 /// Construct from a vector of index sequences
 /**
   @param v Vector of indices
  */
 explicit fundamental_operator_set(reduction_t const& v) {
  for (auto const& i : v) insert_from_indices_t(i);
 }

 /// Construct fundamental_operator_set on a GF structure
 /**
   @param gf_struct GF structure object
  */
 fundamental_operator_set (gf_struct_t const& gf_struct) {
  for(auto const& block : gf_struct)
   for(auto const& inner : block.second)
    insert(block.first, inner);
 }

 /// Reduce to a `std::vector<indices_t>`
 explicit operator reduction_t() const { return reverse_map(); }

 /// Insert a new index sequence given as `indices_t`
 /**
   @param ind `indices_t` object
  */
 void insert_from_indices_t(indices_t const& ind) {
  map_index_n.insert({ind, size()});
  // reorder the indices, which are always given in the order of the indices tuple
  map_t m;
  int i = 0;
  for (auto const& p : map_index_n) m.insert({p.first, i++});
  std::swap(m, map_index_n);
 }

 /// Insert a new index sequence given as multiple `int`/`std::string` arguments
 template <typename... IndexType> void insert(IndexType const&... ind) { insert_from_indices_t(indices_t{ind...}); }

 /// Number of elements in this set
 /**
   @return Size of the set
  */
 int size() const { return map_index_n.size(); }

 /// Check if a given index sequence is in this set
 /**
   @param t Index sequence to look up
   @return `true` if `t` is in this set
  */
 bool has_indices(indices_t const& t) const { return map_index_n.count(t) == 1; }

 /// Request position of a given index sequence
 /**
   @param t Index sequence to look up
   @return Position of the requested index sequence
  */
 int operator[](indices_t const& t) const {
  try {
   return map_index_n.at(t);
  } catch(std::out_of_range &) {
   TRIQS_RUNTIME_ERROR << "Operator with indices (" << t << ") does not belong to this fundamental set!";
  }
 }

 /// Comparison with another fundamental operator set
 bool operator==(fundamental_operator_set const& fops) const { return map_index_n == fops.map_index_n; }

 /// Build and return the reverse map: `int` -> `indices_t`
 /**
   @return The reverse map
  */
 std::vector<indices_t> reverse_map() const {
  std::vector<indices_t> r(size());
  for (auto const& x : map_index_n) r[x.second] = x.first;
  return r;
 }

 // Dereference type for const_iterator
 struct _cdress {
  indices_t const& index;
  int linear_index;
  _cdress(typename map_t::const_iterator _it) : index(_it->first), linear_index(_it->second) {}
 };

 /// Constant bidirectional iterator over all stored index sequences. For an iterator `it`, `it->index` gives the `indices_t` object pointed by this iterator, and `it->linear_index` is its position in the set.
 using const_iterator = triqs::utility::dressed_iterator<typename map_t::const_iterator, _cdress>;

 /// Return `const_iterator` to the first element of this set
 /**
   @return Iterator to the first index sequence
  */
 const_iterator begin() const noexcept { return map_index_n.begin(); }
 /// Return `const_iterator` to the past-the-end element of this set
 /**
   @return Iterator to the past-the-end element
  */
 const_iterator end() const noexcept { return map_index_n.end(); }
 /// Equivalent to [[fundamental_operator_set_begin]]
 /**
   @return Iterator to the first index sequence
  */
 const_iterator cbegin() const noexcept { return map_index_n.cbegin(); }
 /// Equivalent to [[fundamental_operator_set_end]]
 /**
   @return Iterator to the past-the-end element
  */
 const_iterator cend() const noexcept { return map_index_n.cend(); }

 /// Write this set as an HDF5 attribute
 /**
   @param id ID of an HDF5 object to attach the attribute to
   @param name Name of the attribute
   @param f Fundamental set to write
  */

 friend void h5_write_attribute(hid_t id, std::string const& name, fundamental_operator_set const& f);
 /// Read a set from an HDF5 attribute
 /**
   @param id ID of an HDF5 object the attribute is attached to
   @param name Name of the attribute
   @param f Reference to a fundamental set to be read
  */
 friend void h5_read_attribute(hid_t id, std::string const& name, fundamental_operator_set& f);

};
}}

