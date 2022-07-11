// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2015-2016 Igor Krivenko
// Copyright (c) 2018 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Nils Wentzell

#pragma once

#include <set>
#include <map>
#include <utility>
#include <triqs/utility/numeric_ops.hpp>
#include <boost/pending/disjoint_sets.hpp>

namespace triqs {
  namespace hilbert_space {

    /// Implementation of the automatic partitioning algorithm
    /**
  Partitions a Hilbert space into a set of subspaces invariant under action of a given Hermitian operator (Hamiltonian).
  Optionally modifies the partition so that a given operator generates only one-to-one connections between the subspaces.
  For a detailed description of the algorithm see
  `Computer Physics Communications 200, March 2016, 274-284 <http://dx.doi.org/10.1016/j.cpc.2015.10.023>`_ (section 4.2).

  @tparam StateType Many-body state type, must model [[statevector_concept]]
  @tparam OperatorType Imperative operator type, must provide `StateType operator()(StateType const&)`
 */
    template <typename StateType, typename OperatorType> class space_partition {

      public:
      /// Index of a basis Fock state/subspace
      using idx_t = uint32_t;
      /// Accessor to `StateType` template parameter
      using state_t = StateType;
      /// Accessor to `OperatorType` template parameter
      using operator_t = OperatorType;
      /// Amplitude type of the state
      using amplitude_t = typename state_t::value_type;
      /// Connections between subspaces represented as a set of (from-index,to-index) pair
      using block_mapping_t = std::set<std::pair<idx_t, idx_t>>;
      /// Non-zero matrix elements of an operator represented as a mapping (from-state,to-state) -> value
      using matrix_element_map_t = std::map<std::pair<idx_t, idx_t>, typename state_t::value_type>;

      /// Perform Phase I of the automatic partition algorithm
      /**
   Partitions a Hilbert space into invariant subspaces of the Hamiltonian.

   @param st Sample many-body state to be used internally by the algorithm
   @param H Hamiltonian as an imperative operator
   @param store_matrix_elements Should we store the non-vanishing matrix elements of the Hamiltonian?
   @param H Hyb as an additionnal optional imperative operator
  */
      space_partition(state_t const &st, operator_t const &H, bool store_matrix_elements = true, operator_t const &Hyb = operator_t())
         : tmp_state(make_zero_state(st)), subspaces(st.size()) {
        auto size = tmp_state.size();

        // Iteration over all initial basis states
        for (idx_t i = 0; i < size; ++i) {
          tmp_state(i)        = amplitude_t(1);
          state_t final_state = H(tmp_state);

          auto mapping = [&](idx_t f, amplitude_t amplitude) {
            using triqs::utility::is_zero;
            if (is_zero(amplitude)) return;
            auto i_subspace = subspaces.find_set(i);
            auto f_subspace = subspaces.find_set(f);
            if (i_subspace != f_subspace) subspaces.link(i_subspace, f_subspace);

            if (store_matrix_elements) matrix_elements[std::make_pair(i, f)] = amplitude;
          };

          // Iterate over non-zero final amplitudes
          foreach (final_state, mapping);
          
          // redo for additionnal Hyb
          if(not Hyb.is_empty()){
            final_state = Hyb(tmp_state);
            foreach (final_state, mapping);
          }
          
          tmp_state(i) = amplitude_t(0.);
        }

        _update_index();
      }

      /// Copy-constructor
      space_partition(space_partition const &) = default;

      /// Perform Phase II of the automatic partition algorithm
      /**
   Merge some of the invariant subspaces together, to ensure that a given operator `Cd`
   and its Hermitian conjugate `C` generate only one-to-one connections between the subspaces.

   @param Cd Subject operator `Cd`, normally a creation operator
   @param C Conjugate of `Cd`, normally an annihilation operator
   @param store_matrix_elements Should we store the non-vanishing matrix elements of `Cd`?
   @return Non-vanishing matrix elements of `Cd` and `C`, if `store_matrix_elements = true`
  */
      std::pair<matrix_element_map_t, matrix_element_map_t> merge_subspaces(operator_t const &Cd, operator_t const &C,
                                                                            bool store_matrix_elements = true) {

        matrix_element_map_t Cd_elements, C_elements;
        std::multimap<idx_t, idx_t> Cd_connections, C_connections;

        auto size = tmp_state.size();

        // Fill connection multimaps
        for (idx_t i = 0; i < size; ++i) {
          tmp_state(i)    = amplitude_t(1);
          auto i_subspace = subspaces.find_set(i);

          auto fill_conn = [this, i, i_subspace, store_matrix_elements](operator_t const &op, std::multimap<idx_t, idx_t> &conn,
                                                                        matrix_element_map_t &elem) {
            state_t final_state = op(tmp_state);
            // Iterate over non-zero final amplitudes
            foreach (final_state, [&](idx_t f, amplitude_t amplitude) {
              using triqs::utility::is_zero;
              if (is_zero(amplitude)) return;
              auto f_subspace = subspaces.find_set(f);
              conn.insert({i_subspace, f_subspace});
              if (store_matrix_elements) elem[{i, f}] = amplitude;
            })
              ;
          };

          fill_conn(Cd, Cd_connections, Cd_elements);
          fill_conn(C, C_connections, C_elements);

          tmp_state(i) = amplitude_t(0.);
        }

        // 'Zigzag' traversal algorithm
        while (!Cd_connections.empty()) {

          // Take one C^+ - connection
          // C^+|lower_subspace> = |upper_subspace>
          idx_t lower_subspace, upper_subspace;
          std::tie(lower_subspace, upper_subspace) = *std::begin(Cd_connections);

          // - Reveals all subspaces reachable from lower_subspace by application of
          //   a 'zigzag' product C^+ C C^+ C C^+ ... of any length.
          // - Removes all visited connections from Cd_connections/C_connections.
          // - Merges lower_subspace with all subspaces generated from lower_subspace by application of (C C^+)^(2*n).
          // - Merges upper_subspace with all subspaces generated from upper_subspace by application of (C^+ C)^(2*n).
          std::function<void(idx_t, bool)> zigzag_traversal = [this, lower_subspace, upper_subspace, &Cd_connections, &C_connections,
                                                               &zigzag_traversal](idx_t i_subspace, // find all connections starting from i_subspace
                                                                                  bool upwards      // if true, C^+ connection, otherwise C connection
                                                              ) {
            std::multimap<idx_t, idx_t>::iterator it;
            while ((it = (upwards ? Cd_connections : C_connections).find(i_subspace)) != (upwards ? Cd_connections : C_connections).end()) {

              auto f_subspace = it->second;
              (upwards ? Cd_connections : C_connections).erase(it);

              if (upwards)
                subspaces.link(f_subspace, upper_subspace);
              else
                subspaces.link(f_subspace, lower_subspace);

              // Recursively apply to all found f_subspace's with a 'flipped' direction
              zigzag_traversal(f_subspace, !upwards);
            }
          };

          // Apply to all C^+ connections starting from lower_subspace
          zigzag_traversal(lower_subspace, true);
        }

        _update_index();

        return std::make_pair(Cd_elements, C_elements);
      }

      /// Return the number of subspaces in the partition
      /**
   @return Number of invariant subspaces
  */
      idx_t n_subspaces() const { return representative_to_index.size(); }

      /// Apply a callable object to all basis Fock states in a given space partition
      /**
  The callable must take two arguments, 1) index of the basis Fock state in the considered full Hilbert space,
  and 2) index of the subspace this basis state belongs to.

  @tparam Lambda Type of the callable object
  @param SP Subject space partition
  @param L Callable object
  */
      template <typename Lambda> friend void foreach (space_partition &SP, Lambda L) {
        for (idx_t n = 0; n < SP.tmp_state.size(); ++n) L(n, SP.lookup_basis_state(n));
      };

      /// Find what invariant subspace a given Fock state belongs to
      /**
   @param basis_state Index of a basis Fock state
   @return Index of the found invariant subspace
  */
      idx_t lookup_basis_state(idx_t basis_state) { return representative_to_index[subspaces.find_set(basis_state)]; }

      /// Access to matrix elements of the Hamiltonian
      /**
   @return Stored matrix elements of the Hamiltonian
  */
      matrix_element_map_t const &get_matrix_elements() const { return matrix_elements; }

      /// Find all subspace-to-subspace connections generated by a given operator
      /**
   @param op Subject imperative operator
   @param diagonal_only Find only the 'diagonal' connections (each subspace connected to itself)
   @return Connections between subspaces
  */
      block_mapping_t find_mappings(operator_t const &op, bool diagonal_only = false) {

        block_mapping_t mapping;

        // Iteration over all initial basis states
        for (idx_t i = 0; i < tmp_state.size(); ++i) {
          state_t initial_state = tmp_state;
          initial_state(i)      = amplitude_t(1);
          auto i_subspace       = subspaces.find_set(i);

          state_t final_state = op(initial_state);

          // Iterate over non-zero final amplitudes
          foreach (final_state, [&](idx_t f, amplitude_t amplitude) {
            using triqs::utility::is_zero;
            if (is_zero(amplitude)) return;
            auto f_subspace = subspaces.find_set(f);
            if ((!diagonal_only) || i_subspace == f_subspace)
              mapping.insert(std::make_pair(representative_to_index[i_subspace], representative_to_index[f_subspace]));
          })
            ;
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
        for (idx_t n = 0; n < tmp_state.size(); ++n) {
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
      std::map<idx_t, idx_t> representative_to_index;
    };
  } // namespace hilbert_space
} // namespace triqs
