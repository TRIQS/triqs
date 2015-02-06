#pragma once

#include <set>
#include <map>
#include <utility>
#include <triqs/utility/draft/numeric_ops.hpp>
#include <boost/pending/disjoint_sets.hpp>

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

 // Algorithm by Michel Ferrero
 // FIXME WARNING: Redundant implementation, must be revisited later
 std::pair<matrix_element_map_t, matrix_element_map_t> merge_subspaces(operator_t const& Cd, operator_t const& C,
                                                                       bool store_matrix_elements = true) {

  matrix_element_map_t Cd_elements, C_elements;

  std::set<index_t> initial_basis_states;
  for (index_t i = 0; i < tmp_state.size(); ++i) initial_basis_states.insert(i);

  do {
   bool merge_occured;
   index_t i = *initial_basis_states.cbegin();

   auto apply_and_merge = [&](operator_t const& op, matrix_element_map_t& me, bool exclude_from_ibs) {
    index_t final_subspace = -1;

    // Iteration over all initial basis states
    for (index_t n = 0; n < tmp_state.size(); ++n) {
     // Is n in initial_subspace?
     if (subspaces.find_set(i) != subspaces.find_set(n)) continue;

     if (exclude_from_ibs && initial_basis_states.count(n) != 0) initial_basis_states.erase(n);

     tmp_state(n) = amplitude_t(1.);
     state_t final_state = op(tmp_state);

     foreach(final_state, [&](index_t f, amplitude_t amplitude) {
      if (triqs::utility::is_zero(amplitude)) return;

      auto f_subspace = subspaces.find_set(f);
      if (final_subspace == -1)
       final_subspace = f_subspace;
      else {
       if (final_subspace != f_subspace) {
        subspaces.link(final_subspace, f_subspace);
        merge_occured = true;
       }
      }

      if (store_matrix_elements) me.insert(std::make_pair(std::make_pair(n, f), amplitude));
     });

     tmp_state(n) = amplitude_t(0.);
    }
    i = final_subspace;
   };

   do {
    merge_occured = false;

    // Apply op1 to i subspace and merge final subspaces
    // All used initial states will be excluded from initial_basis_states
    apply_and_merge(Cd, Cd_elements, true);
    if (i == -1) break; // No final subspace, just 0
    // Apply op2 to i subspace and merge final subspaces
    apply_and_merge(C, C_elements, false);
    if (i == -1) break; // No final subspace, just 0

   } while (merge_occured); // If there were merges, repeat application once more

  } while (initial_basis_states.size() != 0);

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
