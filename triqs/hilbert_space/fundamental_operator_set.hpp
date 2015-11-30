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
#include <triqs/utility/variant_int_string.hpp>
#include <triqs/utility/dressed_iterator.hpp>
#include <triqs/utility/exceptions.hpp>
#include <triqs/h5/base_public.hpp>
#include <vector>
#include <set>
#include <map>

namespace std {
inline std::ostream & operator<<(std::ostream & os, std::vector<triqs::utility::variant_int_string> const& fs) {
 int u = 0;
 for(auto const& i : fs) { if (u++) os << ","; os << i; }
 return os;
}
}

namespace triqs {
namespace hilbert_space {

// This class contains an ordered list the **indices** of the canonical operators used to build the Fock state.
// It guarantees that the order in the list is the same as given by < operator on the indice tuple of the canonical operators.
class fundamental_operator_set {
 public:
 /// The indices of the C, C^+ operators are a vector of int/string
 using indices_t = std::vector<utility::variant_int_string>;
 using reduction_t = std::vector<indices_t>;

 private:
 using map_t = std::map<indices_t, int>; // the table index <-> n
 map_t map_index_n;

 // internal only
 fundamental_operator_set(std::vector<std::vector<std::string>> const&);

 public:
 fundamental_operator_set() {}

 // constructor on a vector which gives the number of alpha's for every a
 fundamental_operator_set(std::vector<int> const& v) {
  for (int i = 0; i < v.size(); ++i)
   for (int j = 0; j < v[i]; ++j) insert(i, j);
 }

 // construct on a set of indices
 template <typename IndexType> fundamental_operator_set(std::set<IndexType> const& s) {
  for (auto const& i : s) insert(i);
 }

 /// Construct on a vector<indices_t>
 explicit fundamental_operator_set(reduction_t const& indices_list) {
  for (auto const& i : indices_list) insert_from_indices_t(i);
 }

 /// reduce to a vector<indices_t>
 explicit operator reduction_t() const { return reverse_map(); }

 // Insert an operator with indices_t (internal for many_body_operator)
 void insert_from_indices_t(indices_t const& ind) {
  map_index_n.insert({ind, size()});
  // reorder the indices which are always given in the order of the indices tuple
  map_t m;
  int i = 0;
  for (auto const& p : map_index_n) m.insert({p.first, i++});
  std::swap(m, map_index_n);
 }

 /// Insert an operator with indices ind
 template <typename... IndexType> void insert(IndexType const&... ind) { insert_from_indices_t(indices_t{ind...}); }

 // return the number of operators
 int size() const { return map_index_n.size(); }

 // Is an operator with indices t in this set?
 bool has_indices(indices_t const& t) const { return map_index_n.count(t) == 1; }

 // flatten (a,alpha) --> n
 int operator[](indices_t const& t) const {
  try {
   return map_index_n.at(t);
  } catch(std::out_of_range &) {
   TRIQS_RUNTIME_ERROR << "Operator with indices (" << t << ") does not belong to this fundamental set!";
  }
 }

 /// Build and return the reverse map : int -> indices
 std::vector<indices_t> reverse_map() const {
  std::vector<indices_t> r(size());
  for (auto const& x : map_index_n) r[x.second] = x.first;
  return r;
 }

// iterator on the tuples
 // for (auto & x : fops) { x.linear_index is linear_index, while x.index is the C multi-index.
 struct _cdress {
  indices_t const& index;
  int linear_index;
  _cdress(typename map_t::const_iterator _it) : index(_it->first), linear_index(_it->second) {}
 };
 using const_iterator = triqs::utility::dressed_iterator<typename map_t::const_iterator, _cdress>;

 const_iterator begin() const noexcept { return map_index_n.begin(); }
 const_iterator end() const noexcept { return map_index_n.end(); }
 const_iterator cbegin() const noexcept { return map_index_n.cbegin(); }
 const_iterator cend() const noexcept { return map_index_n.cend(); }

 friend void h5_write_attribute(hid_t id, std::string const& name, fundamental_operator_set const& f);
 friend void h5_read_attribute(hid_t id, std::string const& name, fundamental_operator_set& f);

};
}}
