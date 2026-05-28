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

namespace triqs {
  namespace hilbert_space {

    /**
     * @addtogroup triqs-ops
     * @{
     */

    /**
     * @brief Imperative representation of a many-body operator acting on many-body states.
     *
     * @details Constructed from a triqs::operators::many_body_operator_generic, an imperative_operator stores a
     * precomputed bitmask representation of every monomial \f$ \hat{m}_i \f$ that appears in the operator. Applying
     * the operator to a many-body state \f$ \lvert \psi \rangle \f$ is then reduced to a sequence of bitwise checks
     * and XORs on the integer-encoded Fock states (see triqs::hilbert_space::fock_state_t), augmented by the
     * fermionic sign computed from the parity of bit positions of the affected operators.
     *
     * If `UseMap` is `false`, the operator acts within a single Hilbert (Fock) space (see
     * triqs::hilbert_space::hilbert_space).
     *
     * If `UseMap` is `true`, the operator acts across a fixed set of invariant subspaces (see
     * triqs::hilbert_space::sub_hilbert_space) and the user must supply a connection map `hmap` such that `hmap[i]`
     * is the index of the subspace into which the operator maps the i-th subspace, or `-1` if the operator
     * annihilates that subspace.
     *
     * @warning When `HilbertType = triqs::hilbert_space::sub_hilbert_space`, the operator must generate only
     * one-to-one connections between the subspaces in the supplied map. If this is not the case, use
     * `HilbertType = triqs::hilbert_space::hilbert_space` instead.
     *
     * @tparam HilbertType Hilbert space type — either triqs::hilbert_space::hilbert_space or
     * triqs::hilbert_space::sub_hilbert_space.
     * @tparam ScalarType Coefficient type of the underlying many-body operator (typically `double` or
     * `std::complex<double>`); may also be a callable producing a scalar when the operator is parameterized.
     * @tparam UseMap If `true`, the constructor expects a user-supplied subspace connection map; if `false`, the
     * operator acts inside a single Hilbert (Fock) space.
     */
    template <typename HilbertType, typename ScalarType = double, bool UseMap = false> class imperative_operator {

      // Fock state convention:
      // |0,...,k> = C^+_0 ... C^+_k |0>
      // Operator monomial convention:
      // C^+_0 ... C^+_i ... C_j  ... C_0

      using scalar_t = ScalarType;

      // Precomputed bitmask representation of a single monomial: coeff is the coefficient,
      // d_mask / dag_mask mark the annihilation / creation operator bit positions, and
      // d_count_mask / dag_count_mask are the precomputed parity masks used for the fermionic sign.
      struct one_term_t {
        scalar_t coeff;
        uint64_t d_mask, dag_mask, d_count_mask, dag_count_mask;
      };
      std::vector<one_term_t> all_terms;

      std::vector<sub_hilbert_space> const *sub_spaces;
      using hilbert_map_t = std::vector<int>;
      hilbert_map_t hilbert_map;

      public:
      /// Default constructor creates a zero imperative operator with no terms.
      imperative_operator() {}

      /**
       * @brief Check whether the imperative operator has no terms.
       * @return True if the operator contains no monomials, false otherwise.
       */
      bool is_empty() const { return (all_terms.size() == 0); }

      /**
       * @brief Construct an imperative_operator from a triqs::operators::many_body_operator_generic and a
       * triqs::hilbert_space::fundamental_operator_set.
       *
       * @details The monomials of `op` are normalized to the order induced by `fops`, picking up the appropriate
       * fermionic sign, and then converted to a compact bitmask representation in which each canonical operator is
       * identified with a bit position from `fops`.
       *
       * If the template parameter `UseMap` is `true`, the additional arguments `hmap` and `sub_spaces_set` must be
       * supplied to describe how `op` maps Hilbert subspaces (see triqs::hilbert_space::sub_hilbert_space) to one
       * another. If `UseMap` is `false`, `hmap` must be empty and `sub_spaces_set` must be `nullptr`; an internal
       * error is raised otherwise.
       *
       * @param op Source many-body operator.
       * @param fops Fundamental operator set; must contain all single particle state indices that appear in `op`.
       * @param hmap Map of subspace-to-subspace connections generated by `op` (only used when `UseMap = true`).
       * `hmap[i] = j` means the i-th subspace is mapped to the j-th subspace; `hmap[i] = -1` means it is annihilated.
       * @param sub_spaces_set Pointer to the vector of all Hilbert subspaces referenced by `hmap` (only used when
       * `UseMap = true`).
       */
      imperative_operator(triqs::operators::many_body_operator_generic<scalar_t> const &op, fundamental_operator_set const &fops,
                          hilbert_map_t hmap = hilbert_map_t(), std::vector<sub_hilbert_space> const *sub_spaces_set = nullptr) {

        sub_spaces  = sub_spaces_set;
        hilbert_map = hmap;
        if ((hilbert_map.size() == 0) != !UseMap) TRIQS_RUNTIME_ERROR << "Internal error";

        auto greater = [&fops](triqs::operators::canonical_ops_t const &op1, triqs::operators::canonical_ops_t const &op2) {
          if (op1.dagger != op2.dagger) return op2.dagger;
          return op1.dagger ? (fops[op1.indices] > fops[op2.indices]) : (fops[op1.indices] < fops[op2.indices]);
        };

        // The goal here is to have a transcription of the many_body_operator in terms
        // of simple vectors (maybe the code below could be more elegant)
        for (auto const &term : op) {
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
                coef *= scalar_t(-1);
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

          std::vector<int> dag, ndag;
          uint64_t d_mask = 0, dag_mask = 0;
          for (auto const &canonical_op : monomial) {
            (canonical_op.dagger ? dag : ndag).push_back(fops[canonical_op.indices]);
            (canonical_op.dagger ? dag_mask : d_mask) |= (uint64_t(1) << fops[canonical_op.indices]);
          }
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
          all_terms.push_back(one_term_t{scalar_t(coef), d_mask, dag_mask, d_count_mask, dag_count_mask});
        }
      }

      /**
       * @brief Apply a callable object to each coefficient of the operator by reference.
       *
       * @details The callable is invoked once per stored monomial term and receives the coefficient by mutable
       * reference, allowing in-place modification.
       *
       * @tparam w_max Type of the callable object; must be invocable with `ScalarType &`.
       * @param L Callable object.
       */
      template <typename w_max> void update(w_max L) {
        for (auto &M : all_terms) L(M.coeff);
      }

      private:
      // Return a zero state in the target Hilbert space of `st` under this operator.
      // When UseMap is true and the connection map sends `st`'s subspace to `-1`, returns a default-constructed
      // (empty) sub_hilbert_space state.
      template <typename StateType> StateType get_target_st(StateType const &st) const {
        if constexpr (UseMap) {
          auto n = hilbert_map[st.get_hilbert().get_index()];
          if (n == -1) return StateType{};
          return StateType{(*sub_spaces)[n]};
        } else {
          return StateType(st.get_hilbert());
        }
      }

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

      // Return x(args...) when scalar_t is callable, x otherwise. Supports parameterized operators
      // whose coefficients are themselves callable objects.
      template <typename... Args> static auto apply_if_possible(scalar_t const &x, Args &&...args) -> std::invoke_result_t<scalar_t, Args...> {
        return x(std::forward<Args>(args)...);
      }
      static auto apply_if_possible(scalar_t const &x) -> scalar_t { return x; }

      public:
      /**
       * @brief Apply the operator to a many-body state \f$ \lvert \psi \rangle \f$ and return the resulting state.
       *
       * @details The result is computed term by term. For each monomial \f$ \hat{m}_i \f$ stored in the operator,
       * every non-zero amplitude in \f$ \lvert \psi \rangle \f$ is checked against the precomputed
       * annihilation/creation bitmasks. Whenever the monomial can act, the fermionic sign is recovered from the
       * parity of bit positions of the operators and the corresponding amplitude is added to the result.
       *
       * The optional extra arguments `args...` are forwarded to each coefficient of the operator, making this
       * overload useful when `ScalarType` is itself a callable (e.g. for parametric or time-dependent operators).
       * For ordinary scalar coefficients the arguments are simply ignored.
       *
       * @tparam StateType Many-body state type (see triqs::hilbert_space::state).
       * @tparam Args Types of the optional extra arguments forwarded to the coefficients.
       * @param st Initial many-body state \f$ \lvert \psi \rangle \f$.
       * @param args Optional argument pack forwarded to each coefficient of the operator.
       * @return Resulting many-body state \f$ \hat{O} \lvert \psi \rangle \f$.
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
          })
            ; // foreach
        }
        return target_st;
      }
    };

    /** @} */

  } // namespace hilbert_space
} // namespace triqs
