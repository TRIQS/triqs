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
#include <triqs/operators/many_body_operator.hpp>
#include <triqs/utility/exceptions.hpp>
#include <vector>
#include <set>
#include <map>

namespace triqs {
namespace hilbert_space {

// This class contains an ordered list the **indices** of the canonical operators used to build the Fock state.
// It guarantees that the order in the list is the same as given by < operator on the indice tuple of the canonical operators.
class fundamental_operator_set {
 public:
 using indices_t = triqs::utility::many_body_operator<double>::indices_t;

 private:
 using map_t = std::map<indices_t, int>; // the table index <-> n
 map_t map_index_n;

 public:
 fundamental_operator_set() {}

 // constructor on a vector which gives the number of alpha's for every a
 fundamental_operator_set(std::vector<int> const& v) {
  for (int i = 0; i < v.size(); ++i)
   for (int j = 0; j < v[i]; ++j) insert(i, j);
 }

 // construct on a set of indices
 template <typename IndexType> fundamental_operator_set(std::set<IndexType> const& s) {
  for(auto const& i : s) insert(i);
 }

 template <typename... IndexType> void insert(IndexType const&... ind) {
  map_index_n.insert({{ind...}, size()});
  // reorder the indices which are always given in the order of the indices tuple
  map_t m;
  int i = 0;
  for (auto const& p : map_index_n) m.insert({p.first, i++});
  std::swap(m, map_index_n);
 }

 // return the number of operators
 int size() const { return map_index_n.size(); }

 // flatten (a,alpha) --> n
 int operator[](indices_t const& t) const {
  try {
   return map_index_n.at(t);
  } catch(std::out_of_range &) {
   std::stringstream msg;
   msg << "Operator with indices (";
   int u = 0;
   for(auto const& i : t) { if (u++) msg << ","; msg << i; }
   msg << ") does not belong to this fundamental set!";
   TRIQS_RUNTIME_ERROR << msg.str();
  }
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
};
}}

