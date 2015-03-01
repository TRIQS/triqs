/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015 by I. Krivenko, M. Ferrero
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

#include <set>
#include <map>
#include <utility>
#include <triqs/utility/draft/numeric_ops.hpp>
#include <boost/pending/disjoint_sets.hpp>
#include <boost/timer/timer.hpp>

namespace triqs {
namespace hilbert_space {

// Requirements
// ------------
// * State must model StateVector
// * StateType OperatorType::operator()(StateType const &st) must apply the operator to 'st' and return the result

template <typename StateType, typename OperatorType> class space_partition {

 public:
 using index_t = uint32_t;
 using state_t = StateType;
 using operator_t = OperatorType;
 using amplitude_t = typename state_t::value_type;
 using block_mapping_t = std::set<std::pair<index_t,index_t>>;
 using matrix_element_map_t =
     std::map<std::pair<index_t,index_t>, typename state_t::value_type>;

 space_partition(state_t const& st, operator_t const& H, bool store_matrix_elements = true)
    : subspaces(st.size()), tmp_state(make_zero_state(st)) {
  boost::timer::auto_cpu_timer timer("space_partition::space_partition(): %ws wall, %us user + %ss system = %ts CPU (%p%)\n");
  auto size = tmp_state.size();

  // Iteration over all initial basis states
  for (index_t i = 0; i < size; ++i) {
   tmp_state(i) = amplitude_t(1.0);
   state_t final_state = H(tmp_state);

   // Iterate over non-zero final amplitudes
   foreach(final_state, [&](index_t f, amplitude_t amplitude) {
    if (triqs::utility::is_zero(amplitude)) return;
    auto i_subspace = subspaces.find_set(i);
    auto f_subspace = subspaces.find_set(f);
    if (i_subspace != f_subspace) subspaces.link(i_subspace, f_subspace);

    if (store_matrix_elements) matrix_elements[std::make_pair(i, f)] = amplitude;
   });
   tmp_state(i) = amplitude_t(0.);
  }

  _update_index();
 }

 space_partition(space_partition const&) = default;

 std::pair<matrix_element_map_t, matrix_element_map_t> merge_subspaces(operator_t const& Cd, operator_t const& C,
                                                                       bool store_matrix_elements = true) {

  boost::timer::auto_cpu_timer timer("space_partition::merge_subspaces(): %ws wall, %us user + %ss system = %ts CPU (%p%)\n");

  matrix_element_map_t Cd_elements, C_elements;
  std::multimap<index_t,index_t> Cd_connections, C_connections;

  auto size = tmp_state.size();

  // Fill connection multimaps
  for (index_t i = 0; i < size; ++i) {
   tmp_state(i) = amplitude_t(1.0);
   auto i_subspace = subspaces.find_set(i);

   auto fill_conn = [this,i,i_subspace,store_matrix_elements]
                    (operator_t const& op, std::multimap<index_t,index_t> & conn, matrix_element_map_t & elem) {
    state_t final_state = op(tmp_state);
    // Iterate over non-zero final amplitudes
    foreach(final_state, [&](index_t f, amplitude_t amplitude) {
     if (triqs::utility::is_zero(amplitude)) return;
     auto f_subspace = subspaces.find_set(f);
     conn.insert({i_subspace,f_subspace});
     if (store_matrix_elements) elem[{i, f}] = amplitude;
    });
   };

   fill_conn(Cd,Cd_connections,Cd_elements);
   fill_conn(C,C_connections,C_elements);

   tmp_state(i) = amplitude_t(0.);
  }

  // 'Zigzag' traversal algorithm
  while(!Cd_connections.empty()) {

   // Take one C^+ - connection
   // C^+|lower_subspace> = |upper_subspace>
   index_t lower_subspace, upper_subspace;
   std::tie(lower_subspace,upper_subspace) = *std::begin(Cd_connections);

   // - Reveals all subspaces reachable from lower_subspace by application of
   //   a 'zigzag' product C^+ C C^+ C C^+ ... of any length.
   // - Removes all visited connections from Cd_connections/C_connections.
   // - Merges lower_subspace with all subspaces generated from lower_subspace by application of (C C^+)^(2*n).
   // - Merges upper_subspace with all subspaces generated from upper_subspace by application of (C^+ C)^(2*n).
   std::function<void(index_t,index_t,bool)> zigzag_traversal =
    [this,lower_subspace,upper_subspace,&Cd_connections,&C_connections,&zigzag_traversal]
    (index_t i_subspace,        // find all connections starting from i_subspace
     index_t prev_i_subspace,   // this was 'i_subspace' of the parent zigzag_traversal call
     bool upwards               // if true, C^+ connection, otherwise C connection
    ){
    std::multimap<index_t,index_t>::iterator it;
    while((it = (upwards ? Cd_connections : C_connections).find(i_subspace)) !=
                (upwards ? Cd_connections : C_connections).end()) {

     auto f_subspace = it->second;
     (upwards ? Cd_connections : C_connections).erase(it);
     // Have we returned to the same subspace in two successive calls to zigzag_traversal?
     // If so, stop traversing this branch.
     if(f_subspace == prev_i_subspace) continue;

     if(upwards) subspaces.link(f_subspace, upper_subspace);
     else        subspaces.link(f_subspace, lower_subspace);

     // Recursively apply to all found f_subspace's with a 'flipped' direction
     zigzag_traversal(f_subspace,i_subspace,!upwards);
    }
   };

   // Apply to all C^+ connections starting from lower_subspace
   zigzag_traversal(lower_subspace,lower_subspace,true);
  }

  _update_index();

  return std::make_pair(Cd_elements, C_elements);
 }

 // Access information about subspaces
 index_t n_subspaces() const { return representative_to_index.size(); }

 template <typename Lambda> friend void foreach(space_partition& SP, Lambda L) {
  for (index_t n = 0; n < SP.tmp_state.size(); ++n) L(n, SP.lookup_basis_state(n));
 };

 index_t lookup_basis_state(index_t basis_state) { return representative_to_index[subspaces.find_set(basis_state)]; }

 // Access to matrix elements of H
 matrix_element_map_t const& get_matrix_elements() const { return matrix_elements; }

  block_mapping_t find_mappings(operator_t const& op, bool diagonal_only = false) {

  block_mapping_t mapping;

  // Iteration over all initial basis states
  for (index_t i = 0; i < tmp_state.size(); ++i) {
   state_t initial_state = tmp_state;
   initial_state(i) = amplitude_t(1.0);
   auto i_subspace = subspaces.find_set(i);

   state_t final_state = op(initial_state);

   // Iterate over non-zero final amplitudes
   foreach(final_state, [&](index_t f, amplitude_t amplitude) {
    if (triqs::utility::is_zero(amplitude)) return;
    auto f_subspace = subspaces.find_set(f);
    if((!diagonal_only) || i_subspace==f_subspace)
      mapping.insert(std::make_pair(representative_to_index[i_subspace],
                                    representative_to_index[f_subspace]));
   });
  }

  return mapping;
 }

 private:
 void _update_index() {
  auto p = subspaces.parents();
  subspaces.compress_sets(p.begin(), p.end());  // parents are representatives
  subspaces.normalize_sets(p.begin(), p.end()); // the representative has the smallest index in the set

  // Update representative_to_index
  representative_to_index.clear();
  for (index_t n = 0; n < tmp_state.size(); ++n) {
   representative_to_index.insert(std::make_pair(subspaces.find_set(n), representative_to_index.size()));
  }
 }

 // Temporary zero state
 mutable state_t tmp_state;
 // Subspaces
 boost::disjoint_sets_with_storage<> subspaces;
 // Matrix elements of the Hamiltonian
 matrix_element_map_t matrix_elements;
 // Map representative basis state to subspace index
 std::map<index_t, index_t> representative_to_index;
};
}}
