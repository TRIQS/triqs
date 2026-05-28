// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
// Copyright (c) 2015-2016 Igor Krivenko
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
// Authors: Maxime Charlebois, Michel Ferrero, Igor Krivenko, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides the automatic partitioning algorithm of a Hilbert (Fock) space into invariant subspaces.
 */

#pragma once

#include <set>
#include <map>
#include <utility>
#include <triqs/utility/numeric_ops.hpp>
#include <boost/pending/disjoint_sets.hpp>

namespace triqs {
  namespace hilbert_space {

    /**
     * @addtogroup triqs-ops
     * @{
     */

    /**
     * @brief Automatic partitioning of a Hilbert (Fock) space into invariant subspaces of a Hermitian operator.
     *
     * @details Given a Hamiltonian \f$ \hat{H} \f$, the class partitions the Hilbert (Fock) space \f$ \mathcal{F} \f$
     * (see triqs::hilbert_space::hilbert_space) into a set of invariant subspaces \f$ \{ \mathcal{F}_k \} \f$ such
     * that \f$ \hat{H} \mathcal{F}_k \subseteq \mathcal{F}_k \f$ for every \f$ k \f$. The partition is built in two
     * phases:
     * - Phase I (the constructor) discovers the invariant subspaces of \f$ \hat{H} \f$ by acting with \f$ \hat{H}
     *   \f$ on every basis Fock state \f$ \lvert f \rangle \f$ and merging subspaces that are connected by non-zero
     *   matrix elements. An optional second operator (typically a hybridization) may be supplied to also be
     *   respected by the partition.
     * - Phase II (merge_subspaces()) optionally merges subspaces further so that a given operator \f$ \hat{C}^\dagger
     *   \f$ and its Hermitian conjugate \f$ \hat{C} \f$ generate only one-to-one connections between the resulting
     *   subspaces.
     *
     * The algorithm is described in detail in
     * <a href="http://dx.doi.org/10.1016/j.cpc.2015.10.023">Computer Physics Communications 200, March 2016,
     * 274-284</a> (section 4.2).
     *
     * @tparam StateType Many-body state type; must model `statevector_concept`.
     * @tparam OperatorType Imperative operator type; must provide `StateType operator()(StateType const&) const`
     * (see triqs::hilbert_space::imperative_operator).
     */
    template <typename StateType, typename OperatorType> class space_partition {

      public:
      /// Index type used for basis Fock states and for invariant subspaces.
      using idx_t = uint32_t;

      /// Many-body state type (template parameter `StateType`).
      using state_t = StateType;

      /// Imperative operator type (template parameter `OperatorType`).
      using operator_t = OperatorType;

      /// Amplitude type of the many-body states.
      using amplitude_t = typename state_t::value_type;

      /// Subspace-to-subspace connections, stored as a set of `(from-index, to-index)` pairs.
      using block_mapping_t = std::set<std::pair<idx_t, idx_t>>;

      /// Non-zero matrix elements of an operator, stored as a map `(from-state, to-state) -> amplitude`.
      using matrix_element_map_t = std::map<std::pair<idx_t, idx_t>, typename state_t::value_type>;

      /**
       * @brief Run Phase I of the automatic partition algorithm.
       *
       * @details Partitions the Hilbert (Fock) space spanned by `st` into invariant subspaces of the Hamiltonian
       * `H`. If `Hyb` is non-empty, the resulting partition is additionally constrained so that `Hyb` maps each
       * subspace into a union of subspaces of the partition (typical use case: `Hyb` is a hybridization operator
       * that must respect the symmetries used to block-diagonalize the Hamiltonian).
       *
       * @note `st` is used only as a "template" zero state to deduce the dimension and Hilbert (Fock) space; its
       * amplitudes are irrelevant. It is internally cleared to zero.
       *
       * @param st Sample many-body state defining the Hilbert (Fock) space to partition.
       * @param H Hamiltonian \f$ \hat{H} \f$ as a triqs::hilbert_space::imperative_operator.
       * @param store_matrix_elements If `true`, store all non-vanishing matrix elements of `H` (see
       * get_matrix_elements()).
       * @param Hyb Optional additional operator whose action must be respected by the partition.
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
          foreach (final_state, mapping)
            ;

          // redo for additionnal Hyb
          if (not Hyb.is_empty()) {
            final_state = Hyb(tmp_state);
            foreach (final_state, mapping)
              ;
          }

          tmp_state(i) = amplitude_t(0.);
        }

        _update_index();
      }

      /// Defaulted copy constructor.
      space_partition(space_partition const &) = default;

      /**
       * @brief Run Phase II of the automatic partition algorithm.
       *
       * @details Merges invariant subspaces from Phase I until a given operator `Cd` and its Hermitian conjugate
       * `C` generate only one-to-one connections between the resulting subspaces. The underlying "zigzag" traversal
       * of \f$ \hat{C}^\dagger \hat{C} \hat{C}^\dagger \dots \f$ products is described in section 4.2 of the
       * algorithm paper referenced in the class description.
       *
       * @param Cd Operator \f$ \hat{C}^\dagger \f$ (typically a creation operator).
       * @param C Hermitian conjugate \f$ \hat{C} \f$ of `Cd` (typically an annihilation operator).
       * @param store_matrix_elements If `true`, the non-vanishing matrix elements of `Cd` and `C` are collected
       * during the traversal and returned.
       * @return Pair of maps containing the non-vanishing matrix elements of `Cd` and `C` respectively; both maps
       * are empty if `store_matrix_elements = false`.
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

      /**
       * @brief Get the number of invariant subspaces in the current partition.
       * @return Number of invariant subspaces \f$ \{ \mathcal{F}_k \} \f$.
       */
      idx_t n_subspaces() const { return representative_to_index.size(); }

      /**
       * @brief Apply a callable object to every basis Fock state in the partitioned Hilbert (Fock) space.
       *
       * @details For each basis Fock state index \f$ f \in \{ 0, \dots, \dim(\mathcal{F}) - 1 \} \f$ the callable
       * is invoked with two arguments: \f$ f \f$ itself, and the index of the invariant subspace this basis state
       * belongs to.
       *
       * @tparam w_max Type of the callable object; must be invocable with `(idx_t, idx_t)`.
       * @param SP Space partition to iterate over.
       * @param L Callable object.
       */
      template <typename w_max> friend void foreach (space_partition &SP, w_max L) {
        for (idx_t n = 0; n < SP.tmp_state.size(); ++n) L(n, SP.lookup_basis_state(n));
      }

      /**
       * @brief Look up the invariant subspace containing a given basis Fock state.
       *
       * @param basis_state Index \f$ f \f$ of the basis Fock state.
       * @return Index of the invariant subspace \f$ \mathcal{F}_k \f$ that contains \f$ \lvert f \rangle \f$.
       */
      idx_t lookup_basis_state(idx_t basis_state) { return representative_to_index[subspaces.find_set(basis_state)]; }

      /**
       * @brief Get the stored non-vanishing matrix elements of the Hamiltonian.
       *
       * @details Only populated if Phase I was constructed with `store_matrix_elements = true`.
       *
       * @return Map of \f$ (i, j) \mapsto \langle j | \hat{H} | i \rangle \f$ for every non-vanishing matrix element.
       */
      matrix_element_map_t const &get_matrix_elements() const { return matrix_elements; }

      /**
       * @brief Find all subspace-to-subspace connections generated by a given operator.
       *
       * @details The connections are returned in terms of the indices of the invariant subspaces
       * \f$ \{ \mathcal{F}_k \} \f$ of the current partition.
       *
       * @param op Imperative operator to analyze.
       * @param diagonal_only If `true`, only retain self-connections \f$ \mathcal{F}_k \to \mathcal{F}_k \f$.
       * @return Set of `(from-subspace, to-subspace)` index pairs.
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

    /** @} */

  } // namespace hilbert_space
} // namespace triqs
