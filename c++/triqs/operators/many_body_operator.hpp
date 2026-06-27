// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
// Copyright (c) 2014-2017 Igor Krivenko
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
// Authors: Michel Ferrero, Alexander Hampel, Igor Krivenko, Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell

/**
 * @file
 * @brief Provides generic many-body operators.
 */

#pragma once

#include "../hilbert_space/fundamental_operator_set.hpp"
#include "../utility/dressed_iterator.hpp"
#include "../utility/real_or_complex.hpp"
#include "../utility/numeric_ops.hpp"
#include "../utility/variant_extensions.hpp"

#include <h5/h5.hpp>

#include <algorithm>
#include <cmath>
#include <compare>
#include <complex>
#include <cstddef>
#include <functional>
#include <map>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace triqs::operators {

  /**
   * @addtogroup triqs-ops
   * @{
   */

  // Elevate triqs::utility::real_or_complex to the `triqs::operators` namespace.
  using utility::real_or_complex;

  /// Elevate triqs::hilbert_space::indices_t to the `triqs::operators` namespace.
  using indices_t = hilbert_space::fundamental_operator_set::indices_t;

  // Forward declarations.
  template <typename T> class many_body_operator_generic;
  template <typename T> many_body_operator_generic<T> real(many_body_operator_generic<T> const &op);
  template <typename T> many_body_operator_generic<T> imag(many_body_operator_generic<T> const &op);

  /// Many-body operator with real or complex coefficients (see triqs::operators::many_body_operator_generic).
  using many_body_operator = many_body_operator_generic<real_or_complex>;

  /// Many-body operator with real coefficients (see triqs::operators::many_body_operator_generic).
  using many_body_operator_real = many_body_operator_generic<double>;

  /// Many-body operator with complex coefficients (see triqs::operators::many_body_operator_generic).
  using many_body_operator_complex = many_body_operator_generic<std::complex<double>>;

  /**
   * @brief Second quantization creation/annihilation operator.
   *
   * @details A canonical second quantization operator, \f$ \hat{c}_{\alpha_i} \f$ or
   * \f$ \hat{c}_{\alpha_i}^\dagger \f$, is defined by
   *
   * - a single particle state index \f$ \alpha_i = (\beta^{(i)}_1, \dots, \beta^{(i)}_{k_i}) \f$, where each
   *   \f$ \beta^{(i)}_j \f$ is an integer, a string, a double or an array of integers, and
   * - a boolean flag `dagger` indicating whether it is a creation (true) or annihilation (false) operator.
   */
  struct canonical_ops_t {
    /**
     * @brief True for creation (\f$ \hat{c}_{\alpha_i}^{\dagger} \f$), false for annihilation
     * (\f$ \hat{c}_{\alpha_i} \f$) operators.
     */
    bool dagger{false};

    /// Single particle state index \f$ \alpha_i \f$.
    indices_t indices{};

    /**
     * @brief Three-way comparison operator for canonical operators.
     *
     * @details The ordering is defined as follows:
     * - All creation operators are less than all annihilation operators, i.e. \f$ \hat{c}_{\alpha_i}^\dagger <
     * \hat{c}_{\alpha_j} \f$ for any \f$ \alpha_i \f$ and \f$ \alpha_j \f$.
     * - Among canonical operators with the same `dagger` flag, indices are compared element-wise. For each element, the
     * active type of `std::variant` is compared first (`long` < `std::string` < `double` < `std::array<long, 3>`), and
     * only when the active types agree are the underlying values compared.
     * - For annihilation operators, the index comparison is reversed, so that with the same set of indices the ordering
     * becomes \f$ \hat{c}_{\alpha_1}^\dagger < \hat{c}_{\alpha_2}^\dagger < \dots < \hat{c}_{\alpha_2} <
     * \hat{c}_{\alpha_1} \f$. For example, with integer indices \f$ 1, 2, 3 \f$ the ascending order is \f$
     * \hat{c}_1^\dagger < \hat{c}_2^\dagger < \hat{c}_3^\dagger < \hat{c}_3 < \hat{c}_2 < \hat{c}_1 \f$.
     *
     * @note The order induced by this operator is in general different from the order in
     * triqs::hilbert_space::fundamental_operator_set.
     *
     * @param b Right hand side operator.
     * @return `std::strong_ordering` result of the comparison.
     */
    auto operator<=>(canonical_ops_t const &b) const {
      if (dagger != b.dagger) return !dagger <=> !b.dagger; // c+ < c

      // Compare indices element-wise, ordering by type index first (long < string < double < array)
      auto compare_indices = [](indices_t const &a, indices_t const &b) -> std::strong_ordering {
        for (size_t i = 0; i < std::min(a.size(), b.size()); ++i) {
          // Compare by type index first
          if (a[i].index() != b[i].index()) return a[i].index() <=> b[i].index();
          // Same type: compare values
          auto cmp = std::visit(
             [](auto const &x, auto const &y) -> std::strong_ordering {
               if constexpr (std::is_same_v<std::decay_t<decltype(x)>, std::decay_t<decltype(y)>>) {
                 if (x == y) return std::strong_ordering::equal;
                 return x < y ? std::strong_ordering::less : std::strong_ordering::greater;
               }
               return std::strong_ordering::equal; // unreachable due to index check
             },
             a[i], b[i]);
          if (cmp != std::strong_ordering::equal) return cmp;
        }
        return a.size() <=> b.size();
      };

      return dagger ? compare_indices(indices, b.indices) : compare_indices(b.indices, indices);
    }

    /**
     * @brief Equality operator for canonical operators.
     *
     * @param b Right hand side operator.
     * @return True if both operators are creation (annihilation) operators with the same indices, false otherwise.
     */
    bool operator==(canonical_ops_t const &b) const { return (*this <=> b) == 0; }

    /**
     * @brief Serialize the canonical operator to a generic archive.
     * @param ar Archive to serialize to.
     */
    void serialize(auto &ar) const { ar & dagger & indices; }

    /**
     * @brief Deserialize the canonical operator from a generic archive.
     * @param ar Archive to deserialize from.
     */
    void deserialize(auto &ar) { ar & dagger & indices; }
  };

  /**
   * @brief Write a triqs::operators::canonical_ops_t to a `std::ostream`.
   *
   * @param os `std::ostream` object.
   * @param op Operator to be written.
   * @return Reference to `std::ostream` object.
   */
  std::ostream &operator<<(std::ostream &os, canonical_ops_t const &op);

  /**
   * @brief Type used to represent a monomial of canonical second quantization operators.
   *
   * @details A monomial \f$ \hat{m} \f$ is a product of canonical second quantization operators, i.e.
   * \f[
   *   \hat{m} = \hat{d}_{\alpha_{i_1}} \hat{d}_{\alpha_{i_2}} \dots \hat{d}_{\alpha_{i_n}} \; ,
   * \f]
   * where \f$ \hat{d}_{\alpha_{i_j}} \f$ is either a fermionic creation \f$ \hat{c}_{\alpha_{i_j}}^\dagger \f$ or
   * annihilation operator \f$ \hat{c}_{\alpha_{i_j}} \f$ and \f$ n \f$ is the total number of operators in the
   * monomial. See also triqs::operators::canonical_ops_t.
   *
   * The order of the operators in the product matters, due to the fermionic anti-commutation relations.
   */
  using monomial_t = std::vector<canonical_ops_t>;

  /**
   * @brief Less-than comparison operator for triqs::operators::monomial_t.
   *
   * @details The ordering is defined as follows:
   * - If the two monomials \f$ \hat{m}_1 \f$ and \f$ \hat{m}_2 \f$ contain a different number of operators, the shorter
   * one is the smaller one.
   * - Otherwise, the monomials are compared lexicographically according to the ordering of
   * triqs::operators::canonical_ops_t::operator<=>().
   *
   * @param m1 Left hand side monomial \f$ \hat{m}_1 \f$.
   * @param m2 Right hand side monomial \f$ \hat{m}_2 \f$.
   * @return True if \f$ \hat{m}_1 < \hat{m}_2 \f$, false otherwise.
   */
  bool operator<(monomial_t const &m1, monomial_t const &m2);

  /**
   * @brief Write a triqs::operators::monomial_t to a `std::ostream`.
   *
   * @param os `std::ostream` object.
   * @param m Monomial to be written.
   * @return Reference to `std::ostream` object.
   */
  std::ostream &operator<<(std::ostream &os, monomial_t const &m);

  /**
   * @brief Generic many-body operator.
   *
   * @details A generic many-body operator \f$ \hat{O} \f$ is defined as a linear combination of monomials
   * \f$ \hat{m}_i \f$ such that
   * \f[
   *   \hat{O} = \sum_{i} a_i \hat{m}_i \; ,
   * \f]
   * where \f$ a_i \f$ are real or complex coefficients.
   *
   * Under the hood, we simply store all individual terms in a map/dictionary with the monomials as keys and the
   * coefficients as values.
   *
   * Operator-operator and operator-scalar arithmetic is supported such that many-body operators form an algebra over
   * the field of real/complex numbers with an extra addition operation between operators and scalars.
   *
   * @tparam T Value type of the coefficients.
   */
  template <typename T> class many_body_operator_generic {
    public:
    /// Container type for monomials and their coefficients.
    using monomials_map_t = std::map<monomial_t, T>;

    /// Value type of the coefficients (either real, complex or real_or_complex).
    using scalar_t = T;

    /// Default constructor creates a zero many-body operator, i.e. with no terms.
    many_body_operator_generic() = default;

    /**
     * @brief Construct a many-body operator from another many-body operator with a different coefficient type.
     *
     * @details Calls operator=() to assign all terms of the other operator to the new operator. The scalar type
     * `scalar_t` must be constructible from `S`.
     *
     * @tparam S Scalar type of the source operator.
     * @param p Source many-body operator.
     */
    template <typename S> many_body_operator_generic(many_body_operator_generic<S> const &p) {
      static_assert(std::is_constructible_v<scalar_t, S>);
      *this = p;
    }

    /**
     * @brief Construct a many-body operator \f$ \hat{O} = a \hat{I} \f$.
     *
     * @param x Coefficient \f$ a \f$ of the identity operator \f$ \hat{I} \f$.
     */
    explicit many_body_operator_generic(scalar_t const &x) {
      using triqs::utility::is_zero;
      if (!is_zero(x)) monomials_.insert({{}, x});
    }

    /**
     * @brief Construct a many-body operator \f$ \hat{O} = a \hat{m} \f$.
     *
     * @param x Coefficient \f$ a \f$ of the monomial.
     * @param monomial Monomial \f$ \hat{m} \f$.
     */
    many_body_operator_generic(scalar_t const &x, monomial_t monomial) {
      using triqs::utility::is_zero;
      if (!is_zero(x)) monomials_.emplace(std::move(monomial), x);
    }

    struct _cdress;
    C2PY_IGNORE many_body_operator_generic(_cdress const &term) { normalize_and_insert(term.monomial, term.coef, monomials_); }

    /**
     * @brief Assignment operator from a many-body operator with a different coefficient type.
     *
     * @details Clears the current operator and copies all monomials from the source, converting each coefficient to
     * `scalar_t`. The scalar type `scalar_t` must be constructible from `S`.
     *
     * @tparam S Scalar type of the source operator.
     * @param p Source many-body operator.
     * @return Reference to `this` containing the result.
     */
    template <typename S> many_body_operator_generic &operator=(many_body_operator_generic<S> const &p) {
      static_assert(std::is_constructible_v<scalar_t, S>);
      monomials_.clear();
      for (auto const &y : p.get_monomials()) monomials_.insert(std::make_pair(monomial_t{y.first}, scalar_t(y.second)));
      return *this;
    }

    /// Get the map/dictionary of monomials and their coefficients.
    [[nodiscard]] monomials_map_t const &get_monomials() const { return monomials_; }

    /**
     * @brief Create a minimal fundamental operator set with all single particle state indices \f$ \alpha_i \f$ that
     * appear in the monomials of this operator.
     *
     * @return Fundamental operator set \f$ A = \{ \alpha_i \}_{i=0}^{N-1} \f$.
     */
    [[nodiscard]] hilbert_space::fundamental_operator_set make_fundamental_operator_set() const {
      hilbert_space::fundamental_operator_set fops;
      for (auto const &m : monomials_)
        for (auto const &c_cdag_op : m.first) fops.insert_from_indices_t(c_cdag_op.indices);
      return fops;
    }

    /**
     * @brief Create a many-body operator that represents a single canonical operator \f$ \hat{c}_{\alpha} \f$ or
     * \f$ \hat{c}_{\alpha}^{\dagger} \f$.
     *
     * @param is_dag Boolean flag indicating whether to create a creation (true) or annihilation (false) operator.
     * @param indices Single particle state index \f$ \alpha \f$.
     * @return Many-body operator \f$ \hat{O} = \hat{c}_{\alpha}^{\dagger} \f$ or \f$ \hat{O} = \hat{c}_{\alpha} \f$.
     */
    static many_body_operator_generic make_canonical(bool is_dag, indices_t indices) {
      many_body_operator_generic res;
      auto m = monomial_t{canonical_ops_t{.dagger = is_dag, .indices = indices}};
      res.monomials_.insert({m, scalar_t(1)});
      return res;
    }

    // We use utility::dressed_iterator to dress iterators.
    // _cdress is a simple struct of refs that exposes (monomial, coef) to the iterator.
    struct C2PY_IGNORE _cdress {
      monomial_t const &monomial; // NOLINT
      scalar_t coef;
      _cdress(typename monomials_map_t::const_iterator _it) : monomial(_it->first), coef(_it->second) {}
      operator std::pair<std::vector<std::pair<bool, indices_t>>, scalar_t>() const {
        std::vector<std::pair<bool, indices_t>> tmp_monomial;
        tmp_monomial.reserve(monomial.size());
        for (auto cop : monomial) tmp_monomial.emplace_back(cop.dagger, cop.indices);
        return {tmp_monomial, coef};
      }
    };

    /// Constant iterator type yielding (monomial, coefficient) pairs.
    using const_iterator = utility::dressed_iterator<typename monomials_map_t::const_iterator, _cdress>;

    /// Get a const iterator to the beginning of the map that contains the monomials and their coefficients.
    [[nodiscard]] const_iterator begin() const noexcept { return monomials_.begin(); }

    /// Get a const iterator past the end of the map that contains the monomials and their coefficients.
    [[nodiscard]] const_iterator end() const noexcept { return monomials_.end(); }

    /// Get a const iterator to the beginning of the map that contains the monomials and their coefficients.
    [[nodiscard]] const_iterator cbegin() const noexcept { return monomials_.cbegin(); }

    /// Get a const iterator past the end of the map that contains the monomials and their coefficients.
    [[nodiscard]] const_iterator cend() const noexcept { return monomials_.cend(); }

    /**
     * @brief Check if the current operator \f$ \hat{O} \f$ is close to zero.
     *
     * @param precision Tolerance \f$ \epsilon \f$ for considering a coefficient to be zero.
     * @return True if \f$ |a_i| < \epsilon \f$ for all coefficients \f$ a_i \f$, false otherwise.
     */
    [[nodiscard]] bool is_almost_zero(double precision = 1e-10) const {
      auto term_is_zero = [precision](auto const &term) { return triqs::utility::is_zero(abs(term.coef), precision); };
      return std::all_of(this->begin(), this->end(), term_is_zero);
    }

    /**
     * @brief Check if the current operator \f$ \hat{O} \f$ is exactly zero.
     * @return True if the operator has no terms, false otherwise.
     */
    [[nodiscard]] bool is_zero() const { return monomials_.empty(); }

    /**
     * @brief Unary minus operator to negate the current many-body operator \f$ \hat{O} \f$.
     * @return Many-body operator \f$ -\hat{O} \f$ with the sign of all coefficients flipped.
     */
    many_body_operator_generic operator-() const {
      auto res = *this;
      for (auto &m : res.monomials_) m.second = -m.second;
      return res;
    }

    /**
     * @brief Addition assignment operator to add a scalar \f$ a \f$ to the current many-body operator \f$ \hat{O} \f$.
     *
     * @details If the term with the empty monomial \f$ \hat{m} = \hat{I} \f$ (the identity operator) is already
     * present, the given scalar \f$ a \f$ is added to its coefficient. Otherwise, a new term \f$ a \hat{I} \f$ is
     * created and added to \f$ \hat{O} \f$.
     *
     * @param a Scalar \f$ a \f$.
     * @return Reference to `this` containing the result \f$ \hat{O} + a \f$.
     */
    many_body_operator_generic &operator+=(scalar_t a) {
      using triqs::utility::is_zero;
      if (is_zero(a)) return *this;
      bool is_new_monomial{};
      typename monomials_map_t::iterator it;
      std::tie(it, is_new_monomial) = monomials_.insert(std::make_pair(monomial_t(0), a));
      if (!is_new_monomial) {
        it->second += a;
        erase_zero_monomial(monomials_, it);
      }
      return *this;
    }

    /**
     * @brief Subtraction assignment operator to subtract a scalar \f$ a \f$ from the current many-body operator
     * \f$ \hat{O} \f$.
     *
     * @details Equivalent to `operator+=(-a)`.
     *
     * @param a Scalar \f$ a \f$.
     * @return Reference to `this` containing the result \f$ \hat{O} - a \f$.
     */
    many_body_operator_generic &operator-=(scalar_t a) { return operator+=(-a); }

    /**
     * @brief Multiplication assignment operator to multiply the current many-body operator \f$ \hat{O} \f$ by a scalar 
     * \f$ a \f$.
     *
     * @details Multiplies all coefficients \f$ a_i \f$ of the operator \f$ \hat{O} \f$ by the given scalar \f$ a \f$.
     * If \f$ a = 0 \f$, the operator is set to zero (i.e. all terms are removed).
     *
     * @param a Scalar \f$ a \f$.
     * @return Reference to `this` containing the result \f$ \hat{O} \times a \f$.
     */
    many_body_operator_generic &operator*=(scalar_t a) {
      using triqs::utility::is_zero;
      if (is_zero(a)) {
        monomials_.clear();
      } else {
        for (auto &m : monomials_) m.second *= a;
      }
      return *this;
    }

    /**
     * @brief Division assignment operator to divide the current many-body operator \f$ \hat{O} \f$ by a scalar
     * \f$ a \f$.
     *
     * @details It calls operator*=() with \f$ 1 / a \f$.
     *
     * @param a Scalar \f$ a \f$.
     * @return Reference to `this` containing the result \f$ \hat{O} / a \f$.
     */
    many_body_operator_generic &operator/=(scalar_t a) { return operator*=(scalar_t(1) / a); }

    /**
     * @brief Add a scalar \f$ a \f$ to a many-body operator \f$ \hat{O} \f$.
     *
     * @param op Operator \f$ \hat{O} \f$.
     * @param a Scalar \f$ a \f$.
     * @return Many-body operator \f$ \hat{O} + a \f$.
     */
    friend many_body_operator_generic operator+(many_body_operator_generic op, scalar_t a) { return op += a; }

    /**
     * @brief Add a many-body operator \f$ \hat{O} \f$ to a scalar \f$ a \f$.
     *
     * @param a Scalar \f$ a \f$.
     * @param op Operator \f$ \hat{O} \f$.
     * @return Many-body operator \f$ a + \hat{O} \f$.
     */
    friend many_body_operator_generic operator+(scalar_t a, many_body_operator_generic op) { return op += a; }

    /**
     * @brief Subtract a scalar \f$ a \f$ from a many-body operator \f$ \hat{O} \f$.
     *
     * @param op Operator \f$ \hat{O} \f$.
     * @param a Scalar \f$ a \f$.
     * @return Many-body operator \f$ \hat{O} - a \f$.
     */
    friend many_body_operator_generic operator-(many_body_operator_generic op, scalar_t a) { return op -= a; }

    /**
     * @brief Subtract a many-body operator \f$ \hat{O} \f$ from a scalar \f$ a \f$.
     *
     * @details Equivalent to \f$ -\hat{O} + a \f$. See operator-() and operator+=(scalar_t).
     *
     * @param a Scalar \f$ a \f$.
     * @param op Many-body operator \f$ \hat{O} \f$.
     * @return Many-body operator \f$ a - \hat{O} \f$.
     */
    friend many_body_operator_generic operator-(scalar_t a, many_body_operator_generic const &op) { return -op + a; }

    /**
     * @brief Multiply a many-body operator \f$ \hat{O} \f$ by a scalar \f$ a \f$ on the right.
     *
     * @param op Operator \f$ \hat{O} \f$.
     * @param a Scalar \f$ a \f$.
     * @return Many-body operator \f$ \hat{O} \times a \f$.
     */
    friend many_body_operator_generic operator*(many_body_operator_generic op, scalar_t a) { return op *= a; }

    /**
     * @brief Multiply a many-body operator \f$ \hat{O} \f$ by a scalar \f$ a \f$ on the left.
     *
     * @param a Scalar \f$ a \f$.
     * @param op Operator \f$ \hat{O} \f$.
     * @return Many-body operator \f$ a \times \hat{O} \f$.
     */
    friend many_body_operator_generic operator*(scalar_t a, many_body_operator_generic op) { return op *= a; }

    /**
     * @brief Divide a many-body operator \f$ \hat{O} \f$ by a scalar \f$ a \f$.
     *
     * @param op Operator \f$ \hat{O} \f$.
     * @param a Scalar \f$ a \f$.
     * @return Many-body operator \f$ \hat{O} / a \f$.
     */
    friend many_body_operator_generic operator/(many_body_operator_generic op, scalar_t a) { return op /= a; }

    /**
     * @brief Addition assignment operator to add the many-body operator \f$ \hat{P} \f$ to the current many-body
     * operator \f$ \hat{O} \f$.
     *
     * @details Adds all terms of \f$ \hat{P} \f$ to the current operator \f$ \hat{O} \f$. If a monomial is already
     * present in \f$ \hat{O} \f$, their coefficients are added; otherwise a new term is created. All terms with
     * vanishing coefficients are removed.
     *
     * @param op Right hand side many-body operator \f$ \hat{P} \f$.
     * @return Reference to `this` containing the result \f$ \hat{O} + \hat{P} \f$.
     */
    many_body_operator_generic &operator+=(many_body_operator_generic const &op) {
      bool is_new_monomial{};
      typename monomials_map_t::iterator it;
      for (auto const &m : op.monomials_) {
        std::tie(it, is_new_monomial) = monomials_.insert(m);
        if (!is_new_monomial) {
          it->second += m.second;
          erase_zero_monomial(monomials_, it);
        }
      }
      return *this;
    }

    /**
     * @brief Subtraction assignment operator to subtract the many-body operator \f$ \hat{P} \f$ from the current
     * many-body operator \f$ \hat{O} \f$.
     *
     * @details Subtracts all terms of \f$ \hat{P} \f$ from the current operator \f$ \hat{O} \f$. If a monomial is
     * already present in \f$ \hat{O} \f$, their coefficients are subtracted. All terms with vanishing coefficients
     * are removed.
     *
     * @param op Right hand side many-body operator \f$ \hat{P} \f$.
     * @return Reference to `this` containing the result \f$ \hat{O} - \hat{P} \f$.
     */
    many_body_operator_generic &operator-=(many_body_operator_generic const &op) {
      bool is_new_monomial{};
      typename monomials_map_t::iterator it;
      for (auto const &m : op.monomials_) {
        std::tie(it, is_new_monomial) = monomials_.insert(std::make_pair(m.first, -m.second));
        if (!is_new_monomial) {
          it->second -= m.second;
          erase_zero_monomial(monomials_, it);
        }
      }
      return *this;
    }

    /**
     * @brief Multiplication assignment operator to multiply the current many-body operator \f$ \hat{O} \f$ by another
     * many-body operator \f$ \hat{P} \f$.
     *
     * @details Computes the product \f$ \hat{O} \times \hat{P} \f$ using the distributive property of operator
     * multiplication.
     *
     * All generated monomials are normalized, i.e. sorted according to the order defined by
     * canonical_ops_t::operator<=>(), using fermionic anti-commutation relations and added to the resulting operator.
     * If a monomial is already present in the resulting operator, their coefficients are added together.
     *
     * All terms with vanishing coefficients are removed.
     *
     * @param op Right hand side many-body operator \f$ \hat{P} \f$.
     * @return Reference to `this` containing the result \f$ \hat{O} \times \hat{P} \f$.
     */
    many_body_operator_generic &operator*=(many_body_operator_generic const &op) {
      monomials_map_t tmp_map; // product will be stored here
      for (auto const &m : monomials_)
        for (auto const &op_m : op.monomials_) {
          monomial_t product_m;
          product_m.reserve(m.first.size() + op_m.first.size());
          for (auto const &tmp_op : m.first) product_m.push_back(tmp_op);
          for (auto const &tmp_op : op_m.first) product_m.push_back(tmp_op);
          normalize_and_insert(product_m, m.second * op_m.second, tmp_map);
        }
      std::swap(monomials_, tmp_map);
      return *this;
    }

    /**
     * @brief Sum of two many-body operators \f$ \hat{O} \f$ and \f$ \hat{P} \f$.
     *
     * @param lhs Left hand side operator \f$ \hat{O} \f$.
     * @param rhs Right hand side operator \f$ \hat{P} \f$.
     * @return Many-body operator \f$ \hat{O} + \hat{P} \f$.
     */
    friend many_body_operator_generic operator+(many_body_operator_generic lhs, many_body_operator_generic const &rhs) { return lhs += rhs; }

    /**
     * @brief Difference of two many-body operators \f$ \hat{O} \f$ and \f$ \hat{P} \f$.
     *
     * @param lhs Left hand side operator \f$ \hat{O} \f$.
     * @param rhs Right hand side operator \f$ \hat{P} \f$.
     * @return Many-body operator \f$ \hat{O} - \hat{P} \f$.
     */
    friend many_body_operator_generic operator-(many_body_operator_generic lhs, many_body_operator_generic const &rhs) { return lhs -= rhs; }

    /**
     * @brief Product of two many-body operators \f$ \hat{O} \f$ and \f$ \hat{P} \f$ (Fock-space operator product).
     *
     * @param lhs Left hand side operator \f$ \hat{O} \f$.
     * @param rhs Right hand side operator \f$ \hat{P} \f$.
     * @return Many-body operator \f$ \hat{O} \times \hat{P} \f$.
     */
    friend many_body_operator_generic operator*(many_body_operator_generic lhs, many_body_operator_generic const &rhs) { return lhs *= rhs; }

    /**
     * @brief Equality operator to compare two many-body operators.
     *
     * @details Checks whether the difference \f$ \hat{O} - \hat{P} \f$ is exactly zero.
     *
     * @param op Right hand side many-body operator \f$ \hat{P} \f$.
     * @return True if \f$ \hat{O} = \hat{P} \f$, false otherwise.
     */
    bool operator==(many_body_operator_generic const &op) const { return (*this - op).is_zero(); }

    private:
    // Implementation details of dagger.
    static canonical_ops_t _dagger(canonical_ops_t const &cop) { return {!cop.dagger, cop.indices}; }

    static monomial_t _dagger(monomial_t const &m) {
      monomial_t res;
      for (auto it = m.rbegin(); it != m.rend(); ++it) res.push_back(_dagger(*it));
      return res;
    }

    public:
    /**
     * @brief Compute the Hermitian conjugate (dagger) of the many-body operator \f$ \hat{O} \f$.
     *
     * @details Computes the Hermitian conjugate by reversing the order of the canonical operators in each monomial
     * and flipping their `dagger` flag. The coefficients are complex conjugated.
     *
     * @param op Many-body operator \f$ \hat{O} \f$.
     * @return Many-body operator \f$ \hat{O}^{\dagger} \f$.
     */
    friend many_body_operator_generic dagger(many_body_operator_generic const &op) {
      many_body_operator_generic res;
      using triqs::utility::conj;
      for (auto const &x : op) res.monomials_.insert({_dagger(x.monomial), conj(x.coef)});
      return res;
    }

    /**
     * @brief Transform the coefficients of an operator \f$ \hat{O} \f$ using a callable object.
     *
     * @details The callable object must take two arguments, a monomial \f$ \hat{m}_i \f$ and the corresponding
     * coefficient \f$ a_i \f$, and return a new coefficient \f$ \tilde{a}_i \f$.
     *
     * The callable is applied to each term of the operator and the returned coefficient \f$ \tilde{a}_i \f$ together
     * with the monomial \f$ \hat{m}_i \f$ is inserted in the resulting operator. If the returned coefficient is zero,
     * the term is omitted.
     *
     * @tparam F Callable type.
     * @param op Operator \f$ \hat{O} \f$ to be transformed.
     * @param f Callable object.
     * @return Transformed many-body operator.
     */
    template <typename F> friend many_body_operator_generic transform(many_body_operator_generic const &op, F &&f) { // NOLINT
      many_body_operator_generic res;
      using triqs::utility::is_zero;
      for (auto const &x : op) {
        auto c = f(x.monomial, x.coef);
        if (!is_zero(c)) res.monomials_.insert({x.monomial, c});
      }
      return res;
    }

    /// Get a copy of the operator \f$ \hat{O} \f$ with the imaginary parts of all monomial coefficients set to zero.
    [[nodiscard]] C2PY_PROPERTY_GET(real) many_body_operator_generic real() const { return operators::real(*this); }

    /// Get a copy of the operator \f$ \hat{O} \f$ with the real parts of all monomial coefficients set to zero.
    [[nodiscard]] C2PY_PROPERTY_GET(imag) many_body_operator_generic imag() const { return operators::imag(*this); }

    /**
     * @brief Write a triqs::operators::many_body_operator_generic to a `std::ostream`.
     *
     * @param os `std::ostream` object.
     * @param op Operator \f$ \hat{O} \f$ to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &os, many_body_operator_generic const &op) {
      if (op.monomials_.size() != 0) {
        bool print_plus = false;
        for (auto const &m : op.monomials_) {
          os << (print_plus ? " + " : "") << m.second;
          os << m.first;
          print_plus = true;
        }
      } else
        os << "0";
      return os;
    }

    /**
     * @brief Serialize the many-body operator to a generic archive.
     * @param ar Archive to serialize to.
     */
    void serialize(auto &ar) const { ar & monomials_; }

    /**
     * @brief Deserialize the many-body operator from a generic archive.
     * @param ar Archive to deserialize from.
     */
    void deserialize(auto &ar) { ar & monomials_; }

    /// HDF5 format tag of the many-body operator.
    [[nodiscard]] static std::string hdf5_format() { return "Operator"; }

    /**
     * @brief Write a triqs::operators::many_body_operator together with a
     * triqs::hilbert_space::fundamental_operator_set to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param op Operator to be written.
     * @param fops Fundamental operator set associated with the operator.
     */
    friend void h5_write(h5::group g, std::string const &name, many_body_operator const &op, hilbert_space::fundamental_operator_set const &fops);

    /**
     * @brief Write a triqs::operators::many_body_operator_generic to HDF5.
     *
     * @details A minimal fundamental operator set is constructed from the operator and used to label the canonical
     * operators of each monomial.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param op Operator to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, many_body_operator_generic const &op) {
      h5_write(g, name, op, op.make_fundamental_operator_set());
    }

    /**
     * @brief Read a triqs::operators::many_body_operator together with a
     * triqs::hilbert_space::fundamental_operator_set from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param op Operator to be read into.
     * @param fops Fundamental operator set associated with the operator.
     */
    friend void h5_read(h5::group g, std::string const &name, many_body_operator &op, hilbert_space::fundamental_operator_set &fops);

    /**
     * @brief Read a triqs::operators::many_body_operator_generic from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param op Operator to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, many_body_operator_generic &op) {
      hilbert_space::fundamental_operator_set fops;
      many_body_operator op_real_cplx;
      h5_read(g, name, op_real_cplx, fops);
      op = std::move(op_real_cplx);
    }

    private:
    // Normalize a monomial and insert into a map
    static void normalize_and_insert(monomial_t m, scalar_t coeff, monomials_map_t &target) {
      // The normalization is done by employing a simple bubble sort algorithms.
      // Apart from sorting elements this function keeps track of the sign and
      // recursively calls itself if a permutation of two operators produces a new
      // monomial
      if (m.size() >= 2) {
        bool is_swapped{};
        do { // NOLINT
          is_swapped = false;
          for (int n = 1; n < m.size(); ++n) {
            canonical_ops_t &prev_index = m[n - 1];
            canonical_ops_t &cur_index  = m[n];
            if (prev_index == cur_index) return; // The monomial is effectively zero
            if (prev_index > cur_index) {
              // Are we swapping C and C^+ with the same indices?
              // A bit ugly ...
              canonical_ops_t cur_index_flipped_type(cur_index);
              cur_index_flipped_type.dagger = !cur_index_flipped_type.dagger;
              if (prev_index == cur_index_flipped_type) {
                monomial_t new_m;
                new_m.reserve(m.size() - 2);
                std::copy(m.begin(), m.begin() + n - 1, std::back_inserter(new_m));
                std::copy(m.begin() + n + 1, m.end(), std::back_inserter(new_m));
                normalize_and_insert(new_m, coeff, target);
              }
              coeff = -coeff;
              std::swap(prev_index, cur_index);
              is_swapped = true;
            }
          }
        } while (is_swapped);
      }

      // Insert the result
      bool is_new_monomial{};
      typename monomials_map_t::iterator it;
      std::tie(it, is_new_monomial) = target.insert(std::make_pair(std::move(m), coeff));
      if (!is_new_monomial) {
        it->second += coeff;
        erase_zero_monomial(target, it);
      }
    }

    // Erase a monomial with a close-to-zero coefficient.
    static void erase_zero_monomial(monomials_map_t &m, typename monomials_map_t::iterator &it) {
      using triqs::utility::is_zero;
      if (is_zero(it->second)) m.erase(it);
    }

    private:
    monomials_map_t monomials_;
  };

  /**
   * @brief Assert that two many-body operators are close to each other within a given precision.
   *
   * @details Checks whether the difference \f$ \hat{O}_1 - \hat{O}_2 \f$ is close to zero within the given tolerance
   * \f$ \epsilon \f$ using many_body_operator_generic::is_almost_zero(). Throws an exception otherwise.
   *
   * @tparam T1 Scalar type of the first many-body operator.
   * @tparam T2 Scalar type of the second many-body operator.
   * @param op1 Many-body operator \f$ \hat{O}_1 \f$.
   * @param op2 Many-body operator \f$ \hat{O}_2 \f$.
   * @param precision Tolerance \f$ \epsilon \f$.
   */
  template <typename T1, typename T2>
  void assert_operators_are_close(many_body_operator_generic<T1> const &op1, many_body_operator_generic<T2> const &op2, double precision) {
    if (!(op1 - op2).is_almost_zero(precision)) TRIQS_RUNTIME_ERROR << "Error in operators::assert_operators_are_close: Terms are different";
  }

  /**
   * @brief Get a copy of the given operator \f$ \hat{O} \f$ with the imaginary parts of all monomial coefficients set
   * to zero.
   *
   * @details Implemented in terms of transform().
   *
   * @tparam T Scalar type of the many-body operator.
   * @param op Operator \f$ \hat{O} \f$ to be transformed.
   * @return Real part of the operator.
   */
  template <typename T> many_body_operator_generic<T> real(many_body_operator_generic<T> const &op) {
    return transform(op, [](monomial_t const &, T a_i) {
      using triqs::utility::real;
      return real(a_i);
    });
  }

  /**
   * @brief Get a copy of the given operator \f$ \hat{O} \f$ with the real parts of all monomial coefficients set to
   * zero.
   *
   * @details Implemented in terms of transform().
   *
   * @tparam T Scalar type of the many-body operator.
   * @param op Operator \f$ \hat{O} \f$ to be transformed.
   * @return Imaginary part of the operator.
   */
  template <typename T> many_body_operator_generic<T> imag(many_body_operator_generic<T> const &op) {
    return transform(op, [](monomial_t const &, T a_i) {
      using triqs::utility::imag;
      return imag(a_i);
    });
  }

  /**
   * @brief Check if a many-body operator is Hermitian within a given precision.
   *
   * @details Checks whether the difference \f$ \hat{O}^{\dagger} - \hat{O} \f$ is close to zero within the given
   * tolerance \f$ \epsilon \f$.
   *
   * @tparam T Scalar type of the many-body operator.
   * @param op Many-body operator \f$ \hat{O} \f$.
   * @param tolerance Tolerance \f$ \epsilon \f$.
   * @return True if \f$ \hat{O} \f$ is Hermitian within the given precision, false otherwise.
   */
  template <typename T> bool is_op_hermitian(many_body_operator_generic<T> const &op, double tolerance = 0.0) {
    return (dagger(op) - op).is_almost_zero(tolerance);
  }

  /**
   * @brief Create an annihilation operator \f$ \hat{c}_{\alpha} \f$.
   *
   * @tparam T Scalar type of the many-body operator.
   * @tparam IndexTypes Integer, string or double types.
   * @param indices \f$ \beta_1, \dots, \beta_k \f$ that form the index \f$ \alpha \f$.
   * @return Many-body operator \f$ \hat{O} = \hat{c}_{\alpha} \f$.
   */
  template <typename T = real_or_complex, typename... IndexTypes> many_body_operator_generic<T> c(IndexTypes... indices) {
    return many_body_operator_generic<T>::make_canonical(false, indices_t{indices...});
  }

  /**
   * @brief Create a creation operator \f$ \hat{c}_{\alpha}^{\dagger} \f$.
   *
   * @tparam T Scalar type of the many-body operator.
   * @tparam IndexTypes Integer, string or double types.
   * @param indices \f$ \beta_1, \dots, \beta_k \f$ that form the index \f$ \alpha \f$.
   * @return Many-body operator \f$ \hat{O} = \hat{c}_{\alpha}^{\dagger} \f$.
   */
  template <typename T = real_or_complex, typename... IndexTypes> many_body_operator_generic<T> c_dag(IndexTypes... indices) {
    return many_body_operator_generic<T>::make_canonical(true, indices_t{indices...});
  }

  /**
   * @brief Create a number operator \f$ \hat{n}_{\alpha} = \hat{c}_{\alpha}^{\dagger} \hat{c}_{\alpha} \f$.
   *
   * @tparam T Scalar type of the many-body operator.
   * @tparam IndexTypes Integer, string or double types.
   * @param indices \f$ \beta_1, \dots, \beta_k \f$ that form the index \f$ \alpha \f$.
   * @return Many-body operator \f$ \hat{O} = \hat{n}_{\alpha} = \hat{c}_{\alpha}^{\dagger} \hat{c}_{\alpha} \f$.
   */
  template <typename T = real_or_complex, typename... IndexTypes> many_body_operator_generic<T> n(IndexTypes... indices) {
    return c_dag<T>(indices...) * c<T>(indices...);
  }

  /** @} */

} // namespace triqs::operators

// Specialization of `std::hash` for triqs::operators::canonical_ops_t combining the `dagger` flag with each element of `indices`.
template <> struct std::hash<triqs::operators::canonical_ops_t> {
  std::size_t operator()(triqs::operators::canonical_ops_t const &c) const noexcept {
    std::size_t h = std::hash<bool>{}(c.dagger);
    for (auto const &idx : c.indices) {
      std::visit(triqs::utility::overloaded{[&](std::array<long, 3> const &a) {
                                              for (long x : a) h += std::hash<long>{}(x);
                                            },
                                            [&](auto const &v) { h += std::hash<std::decay_t<decltype(v)>>{}(v); }},
                 idx);
    }
    return h;
  }
};
