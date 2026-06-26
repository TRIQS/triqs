// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
// Copyright (c) 2016 Igor Krivenko
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
 * @brief Provides a fast imperative representation of a many-body operator acting on states.
 */

#pragma once
#include "./fundamental_operator_set.hpp"
#include "../operators/many_body_operator.hpp"
#include "./hilbert_space.hpp"

#include <vector>
#include <utility>
#include <algorithm>

namespace triqs::hilbert_space {

  /**
   * @addtogroup triqs-hilbert
   * @{
   */

  /**
   * @brief Representation of a many-body operator acting on many-body states.
   *
   * @details Constructed from a triqs::operators::many_body_operator_generic, an imperative_operator stores a
   * precomputed bitmask representation of every monomial \f$ \hat{m}_i \f$ that appears in the operator. Following the
   * conventions of triqs::operators::many_body_operator_generic and triqs::hilbert_space::fundamental_operator_set, the
   * source operator is written as
   * \f[
   *   \hat{O} = \sum_i a_i \, \hat{m}_i,
   *   \qquad
   *   \hat{m}_i = \hat{c}^{\dagger}_{\alpha_{i_1}} \cdots \hat{c}^{\dagger}_{\alpha_{i_{n_i}}}\,
   *               \hat{c}_{\alpha_{j_1}} \cdots \hat{c}_{\alpha_{j_{m_i}}},
   * \f]
   * with real or complex coefficients \f$ a_i \f$ and single particle state indices \f$ \alpha_k \f$ drawn from the
   * supplied fundamental operator set \f$ A = \{ \alpha_k \}_{k=0}^{N-1} \f$ (\f$ N \leq 64 \f$). Each \f$ \alpha_k \f$
   * is identified with its position \f$ k \f$ in \f$ A \f$, which is used as a bit position; a Fock basis state is
   * encoded as a 64-bit integer (see triqs::hilbert_space::fock_state_t) via \f$ \lvert n_0 n_1 \dots n_{N-1} \rangle
   * \leftrightarrow \sum_k n_k\, 2^k \f$, with the convention \f$ \lvert n_0 \dots n_{N-1} \rangle =
   * (\hat{c}^{\dagger}_{\alpha_0})^{n_0} \cdots (\hat{c}^{\dagger}_{\alpha_{N-1}})^{n_{N-1}} \lvert 0 \rangle \f$.
   *
   * At construction, each monomial \f$ \hat{m}_i \f$ is reordered to the canonical form above (creation operators on
   * the left with \f$ \alpha_{i_1} < \dots < \alpha_{i_{n_i}} \f$, annihilation operators on the right with \f$
   * \alpha_{j_1} > \dots > \alpha_{j_{m_i}} \f$, matching the ordering of triqs::operators::canonical_ops_t), absorbing
   * the appropriate fermionic sign into the coefficient \f$ a_i \f$. It is then stored as four 64-bit masks:
   *
   * - `dag_mask` — bit \f$ i_a \f$ is set if \f$ \hat{m}_i \f$ contains \f$ \hat{c}^{\dagger}_{\alpha_{i_a}} \f$,
   * - `d_mask` — bit \f$ j_b \f$ is set if \f$ \hat{m}_i \f$ contains \f$ \hat{c}_{\alpha_{j_b}} \f$,
   * - `dag_count_mask`, `d_count_mask` — precomputed parity masks that, when AND'ed with a Fock state and combined
   * through XOR, yield the fermionic sign without rescanning the canonical order at apply time.
   *
   * If `UseMap` is `false`, the operator acts within a single Hilbert (Fock) space (see
   * triqs::hilbert_space::hilbert_space).
   *
   * If `UseMap` is `true`, the operator acts across a fixed set of invariant subspaces (see
   * triqs::hilbert_space::sub_hilbert_space) and the user must supply a connection map how the operator maps subspaces
   * to one another.
   *
   * @tparam HilbertType Hilbert space type.
   * @tparam ScalarType Coefficient type.
   * @tparam UseMap Whether a connection map between invariant subspaces is used.
   */
  template <typename HilbertType, typename ScalarType = double, bool UseMap = false> class imperative_operator {
    private:
    // Precomputed bitmask representation of a single monomial.
    struct one_term_t {
      ScalarType coeff;
      uint64_t d_mask, dag_mask, d_count_mask, dag_count_mask;
    };

    public:
    /// Coefficient type (might be a callable type).
    using coeff_t = ScalarType;

    /// Map between subspace indices (only used when `UseMap = true`).
    using hilbert_map_t = std::vector<int>;

    /// Default constructor creates a zero imperative operator with no terms.
    imperative_operator() {}

    /**
     * @brief Construct an imperative_operator from a triqs::operators::many_body_operator_generic and a
     * triqs::hilbert_space::fundamental_operator_set.
     *
     * @details The monomials of the given operator are normalized to the order induced by the fundamental operator set,
     * picking up the appropriate fermionic sign, and converted to a compact bitmask representation in which each
     * canonical operator is identified with a bit position from the fundamental operator set.
     *
     * When `UseMap = true`, the operator additionally acts across invariant Hilbert subspaces and a connection map
     * describing how the operator maps subspaces to one another must be supplied through `hmap` and `sub_spaces_set`.
     * When `UseMap = false`, `hmap` must be empty and `sub_spaces_set` must be `nullptr`; an internal error is raised
     * otherwise.
     *
     * @note Preconditions for the `UseMap = true` case (not checked at construction time):
     * - `sub_spaces_set` must be non-null and the pointed-to vector must outlive the constructed operator.
     * - Every `hmap[i]` is either `-1` (subspace annihilated by `op`) or a valid index into `*sub_spaces_set`.
     * - `hmap.size()` must cover every subspace index of any state subsequently passed to operator()().
     *
     * @param op Source many-body operator.
     * @param fops Fundamental operator set; must contain all single particle state indices that appear in the operator.
     * @param hmap Map of subspace-to-subspace connections generated by `op` (only used when `UseMap = true`).
     * `hmap[i] = j` means the i-th subspace is mapped to the j-th subspace; `hmap[i] = -1` means it is annihilated.
     * @param sub_spaces_set Pointer to the vector of all Hilbert subspaces referenced by `hmap` (only used when
     * `UseMap = true`).
     */
    imperative_operator(triqs::operators::many_body_operator_generic<coeff_t> const &op, fundamental_operator_set const &fops,
                        hilbert_map_t hmap = hilbert_map_t(), std::vector<sub_hilbert_space> const *sub_spaces_set = nullptr) {

      sub_spaces  = sub_spaces_set;
      hilbert_map = hmap;
      if ((hilbert_map.size() == 0) != !UseMap) TRIQS_RUNTIME_ERROR << "Internal error";

      // ordering matching the canonical form
      auto greater = [&fops](triqs::operators::canonical_ops_t const &op1, triqs::operators::canonical_ops_t const &op2) {
        if (op1.dagger != op2.dagger) return op2.dagger;
        return op1.dagger ? (fops[op1.indices] > fops[op2.indices]) : (fops[op1.indices] < fops[op2.indices]);
      };

      // The goal here is to have a transcription of the many_body_operator in terms
      // of simple vectors (maybe the code below could be more elegant)
      for (auto const &term : op) {
        // canonical-order working copy; each swap flips the fermionic sign of the coefficient
        auto monomial = term.monomial;
        auto coef     = term.coef;

        // Sort monomial according to the order established by fops
        int n = monomial.size();
        bool swapped;
        do {
          swapped = false;
          for (int i = 1; i < n; ++i) {
            if (greater(monomial[i - 1], monomial[i])) {
              using std::swap;
              swap(monomial[i - 1], monomial[i]);
              swapped = true;
              coef *= coeff_t(-1);
            }
          }
          --n;
        } while (swapped);

        // Given the environment variable CHECK_ISSUE819 was set by the user
        // throw an exception if the result of this model was effected by issue 819
        // https://github.com/TRIQS/triqs/issues/819
        static const bool check_issue819 = std::getenv("CHECK_ISSUE819");
        if (check_issue819 && term.coef != coef)
          TRIQS_RUNTIME_ERROR << "ERROR: The Atom-Diag result of this model is affected by issue 819 (https://github.com/TRIQS/triqs/issues/819).\n"
                                 "If you have solved the same model with release 2.2.0, 2.2.1 or 3.0.0 of TRIQS the result was incorrect.";

        // build bitmask representation of the canonical monomial
        std::vector<int> dag, ndag;
        uint64_t d_mask = 0, dag_mask = 0;
        for (auto const &canonical_op : monomial) {
          (canonical_op.dagger ? dag : ndag).push_back(fops[canonical_op.indices]);
          (canonical_op.dagger ? dag_mask : d_mask) |= (uint64_t(1) << fops[canonical_op.indices]);
        }
        // parity mask: bit i set iff i is not in d AND the number of d-bits > i is odd
        // used to read off the fermionic sign via parity_number_of_bits in operator()
        auto compute_count_mask = [](std::vector<int> const &d) {
          uint64_t mask = 0;
          bool is_on    = (d.size() % 2 == 1);
          for (int i = 0; i < 64; ++i) {
            if (std::find(begin(d), end(d), i) != end(d))
              is_on = !is_on;
            else if (is_on)
              mask |= (uint64_t(1) << i);
          }
          return mask;
        };
        uint64_t d_count_mask = compute_count_mask(ndag), dag_count_mask = compute_count_mask(dag);
        all_terms.push_back(one_term_t{coeff_t(coef), d_mask, dag_mask, d_count_mask, dag_count_mask});
      }
    }

    /**
     * @brief Apply a callable object to each coefficient of the operator.
     *
     * @details The callable is invoked once per stored monomial term and receives the coefficient by mutable reference,
     * allowing in-place modification.
     *
     * @tparam w_max Callable type.
     * @param L Callable object applied to each coefficient of the operator.
     */
    template <typename w_max> void update(w_max L) {
      for (auto &M : all_terms) L(M.coeff);
    }

    /**
     * @brief Check whether the imperative operator has no terms.
     * @return True if the operator contains no monomials, false otherwise.
     */
    bool is_empty() const { return (all_terms.size() == 0); }

    /**
     * @fn StateType operator()(StateType const &st, Args&&... args) const
     * @brief Apply the operator to a many-body state \f$ \lvert \psi \rangle \f$ and return the resulting state.
     *
     * @details The input state is expanded over the occupation number basis of its Hilbert (Fock) space \f$
     * \mathcal{F}^{(m)} \f$,
     * \f[
     *   \lvert \psi \rangle = \sum_{f \in \mathcal{F}^{(m)}} b_f \lvert f \rangle,
     * \f]
     * where \f$ \lvert f \rangle \f$ are the basis Fock states and \f$ b_f \f$ the corresponding amplitudes. Applying
     * \f$ \hat{O} = \sum_i a_i \hat{m}_i \f$ to \f$ \lvert \psi \rangle \f$ yields
     * \f[
     *   \hat{O} \lvert \psi \rangle
     *     = \sum_{f} \sum_{i} a_i \, b_f \, \hat{m}_i \lvert f \rangle
     *     = \sum_{f'} b'_{f'} \lvert f' \rangle = \lvert \psi' \rangle \; ,
     * \f]
     * so the task reduces to evaluating \f$ \hat{m}_i \lvert f \rangle \f$ for every monomial \f$ \hat{m}_i \f$ and
     * every basis state \f$ \lvert f \rangle \f$ with non-zero amplitude \f$ b_f \f$, and accumulating the resulting
     * contributions into the amplitudes \f$ b'_{f'} \f$ of the target state.
     *
     * For a given monomial and Fock state, the action \f$ \hat{m}_i \lvert f \rangle = s_{i,f} \lvert f' \rangle \f$
     * (with sign \f$ s_{i,f} \in \{ -1, 0, +1 \} \f$) reduces to a handful of bitwise operations on the precomputed
     * masks:
     *
     * - if any annihilation site is unoccupied in \f$ \lvert f \rangle \f$ the term vanishes (\f$ s_{i,f} = 0 \f$);
     *   otherwise clear those bits,
     * - if any creation site is already occupied in the intermediate state the term again vanishes;
     *   otherwise set those bits to obtain \f$ \lvert f' \rangle \f$,
     * - read off the fermionic sign \f$ s_{i,f} = \pm 1 \f$ from the parity of the relevant bits of \f$ \lvert f
     *   \rangle \f$ and \f$ \lvert f' \rangle \f$ combined with the precomputed `d_count_mask` and `dag_count_mask`.
     *
     * The contribution \f$ s_{i,f} \, a_i \, b_f \f$ is then accumulated into the amplitude \f$ b'_{f'} \f$ at the
     * basis index of \f$ \lvert f' \rangle \f$ in the target state, so that after looping over all monomials and all
     * non-zero amplitudes of \f$ \lvert \psi \rangle \f$ the target state holds
     * \f$ b'_{f'} = \sum_{i, f} s_{i,f} \, a_i \, b_f \, \delta_{f', \hat{m}_i f} \f$.
     *
     * The optional extra arguments are forwarded to each coefficient of the operator, making this overload useful when
     * `ScalarType` is itself a callable (e.g. for parametric or time-dependent operators). For ordinary scalar
     * coefficients the arguments are simply ignored.
     *
     * @tparam StateType Many-body state type.
     * @tparam Args Types of the optional extra arguments forwarded to the coefficients.
     * @param st Initial many-body state \f$ \lvert \psi \rangle \f$.
     * @param args Optional arguments forwarded to each coefficient of the operator (only meaningful when `ScalarType`
     * is a callable).
     * @return Resulting many-body state \f$ \hat{O} \lvert \psi \rangle = \lvert \psi' \rangle  \f$.
     */
    template <typename StateType, typename... Args> StateType operator()(StateType const &st, Args &&...args) const {

      StateType target_st = get_target_st(st);
      auto const &hs      = st.get_hilbert();

      using amplitude_t = typename StateType::value_type;

      for (int i = 0; i < all_terms.size(); ++i) { // loop over monomials
        auto M = all_terms[i];
        foreach (st, [M, &target_st, hs, args...](int j, typename StateType::value_type amplitude) {
          fock_state_t f2 = hs.get_fock_state(j);
          if ((f2 & M.d_mask) != M.d_mask) return;
          f2 &= ~M.d_mask;
          if (((f2 ^ M.dag_mask) & M.dag_mask) != M.dag_mask) return;
          fock_state_t f3    = ~(~f2 & ~M.dag_mask);
          auto sign_is_minus = parity_number_of_bits((f2 & M.d_count_mask) ^ (f3 & M.dag_count_mask));
          // update state vector in target Hilbert space
          auto ind = target_st.get_hilbert().get_state_index(f3);
          target_st(ind) += amplitude * apply_if_possible(M.coeff, args...) * (sign_is_minus ? -amplitude_t(1) : amplitude_t(1));
        }); // foreach
      }
      return target_st;
    }

    private:
    // Return a zero state in the target Hilbert space of |psi> under this operator, or a default-constructed (empty)
    // state when UseMap=true and the connection map sends |psi>'s subspace to -1.
    template <typename StateType> StateType get_target_st(StateType const &st) const {
      if constexpr (UseMap) {
        auto n = hilbert_map[st.get_hilbert().get_index()];
        if (n == -1) return StateType{};
        return StateType{(*sub_spaces)[n]};
      } else {
        return StateType(st.get_hilbert());
      }
    }

    // Return true if the number of set bits in v is odd, false if it is even.
    static bool parity_number_of_bits(uint64_t v) {
      // http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetNaive
      // v ^= v >> 16;
      // only ok until 16 orbitals ! assert this or put the >> 16
      v ^= v >> 8;
      v ^= v >> 4;
      v ^= v >> 2;
      v ^= v >> 1;
      return v & 0x01;
    }

    // Return x(args...) when args... is non-empty (coeff_t must be callable with them), x otherwise.
    // Supports parameterized operators whose coefficients are themselves callable objects.
    template <typename... Args> static auto apply_if_possible(coeff_t const &x, Args &&...args) -> std::invoke_result_t<coeff_t, Args...> {
      return x(std::forward<Args>(args)...);
    }
    static auto apply_if_possible(coeff_t const &x) -> coeff_t { return x; }

    private:
    std::vector<one_term_t> all_terms;
    std::vector<sub_hilbert_space> const *sub_spaces;
    hilbert_map_t hilbert_map;
  };

  /** @} */

} // namespace triqs::hilbert_space
