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

#include "./state.hpp"
#include "../utility/numeric_ops.hpp"

#include <boost/pending/disjoint_sets.hpp>

#include <cstdint>
#include <map>
#include <set>
#include <utility>

namespace triqs::hilbert_space {

  /**
   * @addtogroup triqs-hilbert
   * @{
   */

  /**
   * @brief Automatic partitioning of a Hilbert (Fock) space into invariant subspaces of a Hermitian operator.
   *
   * @details Given a Hamiltonian \f$ \hat{H} \f$ acting on a Hilbert (Fock) space \f$ \mathcal{F} \f$ (see
   * triqs::hilbert_space::hilbert_space) with basis \f$ B_{\mathcal{F}} = \{ \lvert f \rangle \}_{f = 0}^{\dim(
   * \mathcal{F}) - 1} \f$, the class builds the finest partition of \f$ \mathcal{F} \f$ into invariant subspaces \f$
   * \{ \mathcal{F}^{(k)} \} \f$ such that
   * \f[
   *   \hat{H} \, \mathcal{F}^{(k)} \subseteq \mathcal{F}^{(k)} \quad \forall k, \qquad
   *   \mathcal{F} = \bigoplus_k \mathcal{F}^{(k)}.
   * \f]
   * Each invariant subspace \f$ \mathcal{F}^{(k)} \f$ is represented as the set of Fock state indices \f$ f \f$ whose
   * basis states \f$ \lvert f \rangle \f$ belong to it, and the partition is encoded internally as a disjoint-set
   * (union-find) data structure over \f$ \{ 0, \dots, \dim(\mathcal{F}) - 1 \} \f$.
   *
   * The partition is built in two phases:
   *
   * - **Phase I** (the constructor) discovers the invariant subspaces of \f$ \hat{H} \f$ by acting with \f$ \hat{H} \f$
   * on every basis Fock state \f$ \lvert f \rangle \in B_{\mathcal{F}} \f$ and merging the subspace containing \f$
   * \lvert f \rangle \f$ with the subspace containing every \f$ \lvert f' \rangle \f$ that appears with non-vanishing
   * amplitude in \f$ \hat{H} \lvert f \rangle \f$. An optional second operator (typically a hybridization) may be
   * supplied and is applied in the same loop so that the resulting partition is invariant under both operators.
   * - **Phase II** (merge_subspaces()) optionally merges subspaces further so that a given operator \f$ \hat{c}^\dagger
   * \f$ and its Hermitian conjugate \f$ \hat{c} \f$ generate at most one-to-one connections between the resulting
   * subspaces. This is implemented as a breadth-first traversal of the alternating \f$ \hat{c}^\dagger \hat{c}
   * \hat{c}^\dagger \cdots \f$ connections between subspaces.
   *
   * The algorithm is described in detail in <a href="http://dx.doi.org/10.1016/j.cpc.2015.10.023">Computer Physics
   * Communications 200, March 2016, 274-284</a> (section 4.2).
   *
   * @tparam S Many-body state type.
   * @tparam OP Imperative operator type.
   */
  template <typename S, typename OP> class space_partition {
    public:
    /// Index type for basis Fock states \f$ \lvert f \rangle \f$ and for invariant subspaces \f$ \mathcal{F}^{(k)} \f$.
    using idx_t = uint32_t;

    /// Many-body state type.
    using state_t = S;

    /// Imperative operator type.
    using operator_t = OP;

    /// Amplitude type of the many-body states.
    using amplitude_t = typename state_t::value_type;

    /// Set of subspace-to-subspace connections, stored as `(from-subspace, to-subspace)` index pairs.
    using block_mapping_t = std::set<std::pair<idx_t, idx_t>>;

    /**
     * @brief Non-vanishing matrix elements of an operator \f$ \hat{O} \f$ in the Fock basis.
     *
     * @details Stored as a map \f$ (i, j) \mapsto \langle f_j \lvert \hat{O} \rvert f_i \rangle \f$, where \f$ i \f$
     * and \f$ j \f$ are the indices of the initial and final basis Fock states \f$ \lvert f_i \rangle \f$ and \f$
     * \lvert f_j \rangle \f$.
     */
    using matrix_element_map_t = std::map<std::pair<idx_t, idx_t>, amplitude_t>;

    /**
     * @brief Construct a space partition by running **Phase I** of the automatic partition algorithm.
     *
     * @details Partitions the Hilbert (Fock) space \f$ \mathcal{F} \f$ associated with \f$ | \psi \rangle \f$ into 
     * invariant subspaces \f$ \{ \mathcal{F}^{(k)} \} \f$ of the Hamiltonian \f$ \hat{H} \f$. 
     * 
     * The constructor iterates over every basis Fock state \f$ \lvert f \rangle \in B_{\mathcal{F}} \f$, evaluates \f$ 
     * \hat{H} \lvert f \rangle \f$, and for every \f$ \lvert f' \rangle \f$ that appears with non-vanishing amplitude 
     * \f$ \langle f' \lvert \hat{H} \rvert f \rangle \neq 0 \f$ merges the subspace currently containing \f$ \lvert f 
     * \rangle \f$ with the one containing \f$ \lvert f' \rangle \f$ in a disjoint-set data structure.
     *
     * If \f$ \hat{\Delta} \f$ is non-empty, it is applied in the same loop so that the resulting partition is also 
     * invariant under \f$ \hat{\Delta} \f$. The typical use case is a hybridization operator that must respect the same 
     * block structure as \f$ \hat{H} \f$.
     *
     * @note \f$ | \psi \rangle \f$ is used only as a reference state to deduce the dimension and the Hilbert (Fock) 
     * space; its amplitudes are irrelevant.
     *
     * @param psi Sample many-body state \f$ | \psi \rangle \f$ defining the Hilbert (Fock) space \f$ \mathcal{F} \f$ to 
     * partition.
     * @param H Hamiltonian \f$ \hat{H} \f$ as a triqs::hilbert_space::imperative_operator.
     * @param store_matrix_elements If `true`, store all non-vanishing matrix elements \f$ \langle f' \lvert \hat{H} 
     * \rvert f \rangle \f$ encountered during the traversal (can be retrieved via get_matrix_elements()).
     * @param delta Optional additional operator \f$ \hat{\Delta} \f$ whose action must be respected by the partition.
     */
    space_partition(state_t const &psi, operator_t const &H, bool store_matrix_elements = true, operator_t const &delta = operator_t())
       : tmp_state(make_zero_state(psi)), subspaces(psi.size()) {
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
        if (not delta.is_empty()) {
          final_state = delta(tmp_state);
          foreach (final_state, mapping);
        }

        tmp_state(i) = amplitude_t(0.);
      }

      _update_index();
    }

    /// Default copy constructor.
    space_partition(space_partition const &) = default;

    /**
     * @brief Run **Phase II** of the automatic partition algorithm.
     *
     * @details Merges invariant subspaces produced by **Phase I** until a given operator \f$ \hat{c}^\dagger \f$ and 
     * its Hermitian conjugate \f$ \hat{c} \f$ generate at most one-to-one connections between the resulting subspaces 
     * \f$ \{ \mathcal{F}^{(k)} \} \f$. 
     * 
     * The implementation builds, for every initial basis Fock state \f$ \lvert f \rangle \in B_{\mathcal{F}} \f$, the 
     * subspace-level connections induced by \f$ \hat{c}^\dagger \lvert f \rangle \f$ and \f$ \hat{c} \lvert f \rangle 
     * \f$, and then performs a breadth-first traversal of the resulting bipartite graph: starting from a pair of 
     * subspaces \f$ (\mathcal{F}^{(\mathrm{lower})}, \mathcal{F}^{(\mathrm{upper})}) \f$ such that \f$ \hat{c}^\dagger 
     * \mathcal{F}^{(\mathrm{lower})} \cap \mathcal{F}^{(\mathrm{upper})} \neq \{ 0 \} \f$, all subspaces reachable by 
     * an even-length alternating product \f$ (\hat{c} \hat{c}^\dagger)^n \f$ from \f$ \mathcal{F}^{(\mathrm{lower})} 
     * \f$ are merged into \f$ \mathcal{F}^{(\mathrm{lower})} \f$, and all subspaces reachable by \f$ (\hat{c}^\dagger 
     * \hat{c})^n \f$ from \f$ \mathcal{F}^{(\mathrm{upper})} \f$ are merged into \f$ \mathcal{F}^{(\mathrm{upper})} 
     * \f$. The procedure repeats until no \f$ \hat{c}^\dagger \f$-connection is left untreated.
     *
     * @note The algorithm assumes that the given \f$ \hat{c} \f$ is the Hermitian conjugate of \f$ \hat{c}^\dagger \f$; 
     * the bipartite traversal will not give meaningful results otherwise. See section 4.2 of the algorithm paper 
     * referenced in the class description for the mathematical justification.
     *
     * @param cd Operator \f$ \hat{c}^\dagger \f$ (typically a creation operator).
     * @param c Hermitian conjugate \f$ \hat{c} \f$ of \f$ \hat{c}^\dagger \f$ (typically an annihilation operator).
     * @param store_matrix_elements If `true`, the non-vanishing matrix elements of \f$ \hat{c}^\dagger \f$ and \f$ 
     * \hat{c} \f$ are collected during the traversal and returned.
     * @return Pair of maps containing the non-vanishing matrix elements \f$ \langle f' \lvert \hat{c}^\dagger \rvert f 
     * \rangle \f$ and \f$ \langle f' \lvert \hat{c} \rvert f \rangle \f$ respectively; both maps are empty if 
     * `store_matrix_elements = false`.
     */
    auto merge_subspaces(operator_t const &cd, operator_t const &c, bool store_matrix_elements = true) {

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
          });
        };

        fill_conn(cd, Cd_connections, Cd_elements);
        fill_conn(c, C_connections, C_elements);

        tmp_state(i) = amplitude_t(0.);
      }

      // 'Zigzag' traversal algorithm
      while (!Cd_connections.empty()) {

        // Take one C^+ - connection
        // C^+|lower_subspace> = |upper_subspace>
        auto const [lower_subspace, upper_subspace] = *std::begin(Cd_connections);

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
     * @return Number of invariant subspaces \f$ \{ \mathcal{F}^{(k)} \} \f$ in the partition.
     */
    idx_t n_subspaces() const { return representative_to_index.size(); }

    /**
     * @brief Apply a callable object to every basis Fock state in the partitioned Hilbert (Fock) space.
     *
     * @details For each basis Fock state index \f$ f \in \{ 0, \dots, \dim(\mathcal{F}) - 1 \} \f$ the callable
     * is invoked with two arguments: \f$ f \f$ itself, and the index \f$ k \f$ of the invariant subspace \f$ 
     * \mathcal{F}^{(k)} \f$ containing \f$ \lvert f \rangle \f$.
     *
     * @tparam F Callable type.
     * @param sp Space partition to iterate over.
     * @param f Callable object.
     */
    template <typename F> friend void foreach (space_partition &sp, F f) {
      for (idx_t n = 0; n < sp.tmp_state.size(); ++n) f(n, sp.lookup_basis_state(n));
    }

    /**
     * @brief Look up the invariant subspace containing a given basis Fock state.
     *
     * @param f Basis fock state \f$ \lvert f \rangle \f$.
     * @return Index \f$ k \f$ of the invariant subspace \f$ \mathcal{F}^{(k)} \f$ that contains \f$ \lvert f \rangle 
     * \f$.
     */
    idx_t lookup_basis_state(idx_t f) { return representative_to_index[subspaces.find_set(f)]; }

    /**
     * @brief Get the stored non-vanishing matrix elements of the Hamiltonian.
     *
     * @details Only populated if **Phase I** was constructed with `store_matrix_elements = true`. Otherwise the 
     * returned map is empty.
     *
     * @return Map of \f$ (i, j) \mapsto \langle f_j \lvert \hat{H} \rvert f_i \rangle \f$ for every non-vanishing
     * matrix element, where \f$ i \f$ and \f$ j \f$ are the indices of the initial and final basis Fock states.
     */
    matrix_element_map_t const &get_matrix_elements() const { return matrix_elements; }

    /**
     * @brief Find all subspace-to-subspace connections generated by a given operator.
     *
     * @details For every basis Fock state \f$ \lvert f \rangle \in B_{\mathcal{F}} \f$, computes
     * \f$ \hat{O} \lvert f \rangle \f$ and records the index pair \f$ (k, k') \f$ of the invariant subspaces
     * \f$ \mathcal{F}^{(k)} \ni \lvert f \rangle \f$ and \f$ \mathcal{F}^{(k')} \ni \lvert f' \rangle \f$ for
     * every \f$ \lvert f' \rangle \f$ that appears with non-vanishing amplitude.
     *
     * @param op Imperative operator \f$ \hat{O} \f$ to analyze.
     * @param diagonal_only If `true`, only retain self-connections
     * \f$ \mathcal{F}^{(k)} \to \mathcal{F}^{(k)} \f$.
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
      for (idx_t n = 0; n < tmp_state.size(); ++n) {
        representative_to_index.insert(std::make_pair(subspaces.find_set(n), representative_to_index.size()));
      }
    }

    private:
    mutable state_t tmp_state;
    boost::disjoint_sets_with_storage<> subspaces;
    matrix_element_map_t matrix_elements;
    std::map<idx_t, idx_t> representative_to_index;
  };

  /** @} */

} // namespace triqs::hilbert_space
