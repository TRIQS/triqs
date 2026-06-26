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
// Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides a type for many-body states in a Hilbert (Fock) space.
 */

#pragma once

#include "./hilbert_space.hpp"
#include "../arrays.hpp"
#include "../utility/numeric_ops.hpp"

#include <boost/operators.hpp>

#include <cstddef>
#include <iostream>
#include <unordered_map>

namespace triqs::hilbert_space {

  /**
   * @addtogroup triqs-hilbert
   * @{
   */

  // Forward declaration.
  template <typename HilbertSpace, typename ScalarType, bool BasedOnMap> class state {};

  /**
   * @brief Create a zero state in the same Hilbert (Fock) space as the given state.
   * 
   * @details It simply returns the many-body state \f$ \lvert \psi \rangle \in \mathcal{F}^{(m)} = 0 \f$, i.e. with all 
   * amplitudes \f$ a_{f_i} = 0 \f$.
   * 
   * See triqs::hilbert_space::state<HilbertSpace, ScalarType, true> and 
   * triqs::hilbert_space::state<HilbertSpace, ScalarType, false> for more details.
   *
   * @tparam HilbertSpace Hilbert space type.
   * @tparam ScalarType Amplitude type.
   * @tparam BasedOnMap Whether the amplitudes are stored in a map or a vector.
   * @param phi Reference state belonging to \f$ \mathcal{F}^{(m)} \f$.
   * @return \f$ \lvert \psi \rangle \in \mathcal{F}^{(m)} \f$ with all amplitudes set to zero.
   */
  template <typename HilbertSpace, typename ScalarType, bool BasedOnMap>
  auto make_zero_state(state<HilbertSpace, ScalarType, BasedOnMap> const &phi) {
    return state<HilbertSpace, ScalarType, BasedOnMap>{phi.get_hilbert()};
  }

  /**
   * @brief Many-body state based on `std::unordered_map`.
   *
   * @details A general many-body state \f$ \lvert \psi \rangle \f$ in a Hilbert (Fock) space \f$ \mathcal{F}^{(m)} \f$ 
   * can be written as a linear combination over all occupation number basis states \f$ \lvert f_i \rangle \in 
   * \mathcal{F}^{(m)} \f$, i.e.
   * \f[
   *   \lvert \psi \rangle = \sum_{f_i \in \mathcal{F}^{(m)}} a_{f_i} \lvert f_i \rangle \; .
   * \f]
   * Here, \f$ a_{f_i} \f$ are the expansion amplitudes/coefficients corresponding to each basis state \f$ \lvert f_i 
   * \rangle \f$.
   *
   * Non-zero amplitudes \f$ a_{f_i} \f$ are stored in a `std::unordered_map`, where the keys are the Fock states \f$ 
   * \lvert f_i \rangle \f$. The implementation based on a map is efficient as long as the number of non-vanishing 
   * amplitudes is small.
   * 
   * It inherits from `boost::additive` and `boost::multiplicative` to provide basic vector space operations.
   *
   * @tparam HilbertSpace Hilbert space type.
   * @tparam ScalarType Amplitude type.
   */
  template <typename HilbertSpace, typename ScalarType>
  class state<HilbertSpace, ScalarType, true> : boost::additive<state<HilbertSpace, ScalarType, true>>,
                                                boost::multiplicative<state<HilbertSpace, ScalarType, true>, ScalarType> {
    public:
    /// Value type of the amplitudes (either real or complex).
    using value_type = ScalarType;

    /// Type of the Hilbert (Fock) space this state belongs to.
    using hilbert_space_t = HilbertSpace;

    /// Container type for amplitudes.
    using amplitude_t = std::unordered_map<fock_state_t, ScalarType>;

    /// Default constructor for a dummy state that does not belong to any Hilbert (Fock) space.
    state() : hs_ptr_(nullptr) {}

    /**
     * @brief Construct a new many-body state \f$ \lvert \psi \rangle \in \mathcal{F}^{(m)} = 0 \f$.
     * @details All amplitues are set to zero, i.e. \f$ a_{f_i} = 0 \; \forall f_i \f$.
     * @param hs Hilbert (Fock) space \f$ \mathcal{F}^{(m)} \f$.
     */
    state(HilbertSpace const &hs) : hs_ptr_(&hs) {}

    /**
     * @brief Construct a new many-body state \f$ \lvert \psi \rangle \in \mathcal{F}^{(m)} = \lvert f_i \rangle \f$.
     * 
     * @details All amplitues are set to zero, except \f$ a_{f_i} = 1 \f$ for the given Fock state \f$ \lvert f_i 
     * \rangle \f$.
     *
     * @param hs Hilbert (Fock) space \f$ \mathcal{F}^{(m)} \f$.
     * @param f Fock state \f$ \lvert f_i \rangle \f$ for which the amplitude \f$ a_{f_i} = 1 \f$.
     */
    state(HilbertSpace const &hs, fock_state_t f) : hs_ptr_(&hs) { map_[f] = value_type(1.0); }

    /// Get the dimension of the associated Hilbert (Fock) space, i.e. \f$ \dim(\mathcal{F}^{(m)}) \f$.
    [[nodiscard]] int size() const { return hs_ptr_->size(); }

    /// Get the number of non-vanishing amplitudes in the state.
    [[nodiscard]] int nterms() const { return map_.size(); }

    /**
     * @brief Get the amplitude \f$ a_{f_i} \f$ for a given Fock state \f$ \lvert f_i \rangle \f$.
     * 
     * @note We do not check whether the given Fock state is \f$ \in \mathcal{F}^{(m)} \f$.
     *
     * @param f Fock state \f$ \lvert f_i \rangle \f$.
     * @return Reference to the amplitude \f$ a_{f_i} \f$.
     */
    value_type &operator()(fock_state_t f) { return map_[f]; }

    /**
     * @brief Get the amplitude \f$ a_{f_i} \f$ for a given Fock state \f$ \lvert f_i \rangle \f$.
     * 
     * @note We do not check whether the given Fock state is \f$ \in \mathcal{F}^{(m)} \f$.
     *
     * @param f Fock state \f$ \lvert f_i \rangle \f$.
     * @return Const reference to the amplitude \f$ a_{f_i} \f$.
     */
    value_type const &operator()(fock_state_t f) const { return map_[f]; }

    /**
     * @brief Addition assignment operator to add another state \f$ \lvert \phi \rangle \f$.
     *
     * @details It simply adds the amplitudes \f$ b_{f_i} \f$ of the state \f$ \lvert \phi \rangle \f$ to the current 
     * amplitudes \f$ a_{f_i} \f$ and removes any vanishing amplitudes.
     *
     * @param phi Other many-body state \f$ \lvert \phi \rangle \f$.
     * @return Reference to `this`.
     */
    state &operator+=(state const &phi) {
      for (auto const &[f, b_f] : phi.map_) {
        auto [it, inserted] = map_.insert({f, b_f});
        if (!inserted) it->second += b_f;
      }
      prune();
      return *this;
    }

    /**
     * @brief Subtraction assignment operator to subtract another state \f$ \lvert \phi \rangle \f$.
     *
     * @details It simply subtracts the amplitudes \f$ b_{f_i} \f$ of the state \f$ \lvert \phi \rangle \f$ from the 
     * current amplitudes \f$ a_{f_i} \f$ and removes any vanishing amplitudes.
     *
     * @param phi Other many-body state \f$ \lvert \phi \rangle \f$.
     * @return Reference to `this`.
     */
    state &operator-=(state const &phi) {
      for (auto const &[f, b_f] : phi.map_) {
        auto [it, inserted] = map_.insert({f, -b_f});
        if (!inserted) it->second -= b_f;
      }
      prune();
      return *this;
    }

    /**
     * @brief Multiplication assignment operator to multiply a scalar \f$ x \f$.
     *
     * @details It simply multiplies the current amplitudes \f$ a_{f_i} \f$ by \f$ x \f$ and removes any vanishing 
     * amplitudes.
     *
     * @param x Scalar mulitplication factor \f$ x \f$.
     * @return Reference to `this`.
     */
    state &operator*=(value_type x) {
      for (auto &[f, a_f] : map_) { a_f *= x; }
      prune();
      return *this;
    }

    /**
     * @brief Division assignment operator to divide by a scalar \f$ x \f$.
     *
     * @details It simply divides the current amplitudes \f$ a_{f_i} \f$ by \f$ x \f$ and removes any vanishing 
     * amplitudes.
     *
     * @param x Scalar divisor \f$ x \f$.
     * @return Reference to `this`.
     */
    state &operator/=(value_type x) { return operator*=(value_type(1.0) / x); }

    /**
     * @brief Calculate the dot product of two states \f$ \langle \phi | \psi \rangle \f$.
     * 
     * @details It computes the dot product as
     * \f[
     *   \langle \phi | \psi \rangle = \sum_{f_i \in \mathcal{F}^{(m)}} b_{f_i}^* a_{f_i} \; ,
     * \f]
     * where \f$ b_{f_i} \f$ and \f$ a_{f_i} \f$ are the amplitudes of the states \f$ \lvert \phi \rangle \f$ and \f$ 
     * \lvert \psi \rangle \f$, respectively.
     *
     * @param phi Many-body state \f$ \lvert \phi \rangle \f$.
     * @param psi Many-body state \f$ \lvert \psi \rangle \f$.
     * @return Dot product \f$ \langle \phi | \psi \rangle \f$.
     */
    friend value_type dot_product(state const &phi, state const &psi) {
      value_type res(0.0);
      for (auto const &[f, b_f] : phi.map_) {
        using triqs::utility::conj;
        if (psi.map_.count(f) == 1) res += conj(b_f) * psi.map_.at(f);
      }
      return res;
    }

    /**
     * @brief Apply a callable object to all non-vanishing amplitudes of a state \f$ \lvert \psi \rangle \f$.
     *
     * @details It first removes all vanishing amplitudes from the state and then calls the given callable object for 
     * each pair of Fock state \f$ \lvert f_i \rangle \f$ and corresponding amplitude \f$ a_{f_i} \f$.
     *
     * @tparam F Callable type.
     * @param psi Many-body state \f$ \lvert \psi \rangle \f$.
     * @param lambda Callable object.
     */
    template <typename F> friend void foreach (state const &psi, F lambda) {
      const_cast<state &>(psi).prune(); // NOLINT (why do we const cast here?)
      for (auto const &[f, a_f] : psi.map_) lambda(f, a_f);
    }

    /// Get the Hilbert (Fock) space \f$ \mathcal{F}^{(m)} \f$ the current state belongs to.
    HilbertSpace const &get_hilbert() const { return *hs_ptr_; }

    /**
     * @brief Set the Hilbert (Fock) space \f$ \mathcal{F}^{(m)} \f$ for this state.
     * @param new_hs New Hilbert (Fock) space \f$ \mathcal{F}^{(m)} \f$.
     */
    void set_hilbert(HilbertSpace const &new_hs) { hs_ptr_ = &new_hs; }

    private:
    // Discard all amplitudes smaller than a specific threshold.
    void prune() {
      for (auto it = map_.begin(); it != map_.end();) {
        using triqs::utility::is_zero;
        if (is_zero(it->second)) {
          it = map_.erase(it);
        } else {
          ++it;
        }
      }
    }

    private:
    const HilbertSpace *hs_ptr_;
    amplitude_t map_;
  };

  /**
   * @brief Many-body state based on `nda::vector`.
   *
   * @details A general many-body state \f$ \lvert \psi \rangle \f$ in a Hilbert (Fock) space \f$ \mathcal{F}^{(m)} \f$ 
   * can be written as a linear combination over all occupation number basis states \f$ \lvert f_i \rangle \in 
   * \mathcal{F}^{(m)} \f$, i.e.
   * \f[
   *   \lvert \psi \rangle = \sum_{f_i \in \mathcal{F}^{(m)}} a_{f_i} \lvert f_i \rangle \; .
   * \f]
   * Here, \f$ a_{f_i} \f$ are the expansion amplitudes/coefficients corresponding to each basis state \f$ \lvert f_i 
   * \rangle \f$.
   *
   * The amplitudes \f$ a_{f_i} \f$ are stored in an `nda::vector` \f$ \mathbf{a} \in \mathbb{R}^M \f$ or \f$ \mathbf{a} 
   * \in \mathbb{C}^M \f$, with \f$ M = \dim(\mathcal{F}^{(m)}) \f$.
   * 
   * It inherits from `boost::additive` and `boost::multiplicative` to provide basic vector space operations.
   *
   * @tparam HilbertSpace Hilbert space type.
   * @tparam ScalarType Amplitude type.
   */
  template <typename HilbertSpace, typename ScalarType>
  class state<HilbertSpace, ScalarType, false> : boost::additive<state<HilbertSpace, ScalarType, false>>,
                                                 boost::multiplicative<state<HilbertSpace, ScalarType, false>, ScalarType> {
    public:
    /// Value type of the amplitudes (either real or complex).
    using value_type = ScalarType;

    /// Type of the Hilbert (Fock) space this state belongs to.
    using hilbert_space_t = HilbertSpace;

    /// Container type for amplitudes.
    using amplitude_t = nda::vector<ScalarType>;

    /// Default constructor for a dummy state that does not belong to any Hilbert (Fock) space.
    state() : hs_ptr_(nullptr) {}

    /**
     * @brief Construct a new many-body state \f$ \lvert \psi \rangle \in \mathcal{F}^{(m)} = 0 \f$.
     * @details All amplitues are set to zero, i.e. \f$ a_{f_i} = 0 \; \forall f \f$.
     * @param hs Hilbert (Fock) space \f$ \mathcal{F}^{(m)} \f$.
     */
    state(HilbertSpace const &hs) : hs_ptr_(&hs), vec_(amplitude_t::zeros(hs.size())) {}

    /**
     * @brief Construct a new many-body state \f$ \lvert \psi \rangle \in \mathcal{F}^{(m)} = \lvert f_i \rangle \f$.
     * 
     * @details All amplitues are set to zero, except \f$ a_{f_i} = 1 \f$ for the given Fock state \f$ \lvert f_i 
     * \rangle \f$.
     *
     * @param hs Hilbert (Fock) space \f$ \mathcal{F}^{(m)} \f$.
     * @param f Fock state \f$ \lvert f_i \rangle \f$ for which the amplitude \f$ a_{f_i} = 1 \f$.
     */
    state(HilbertSpace const &hs, fock_state_t f) : hs_ptr_(&hs), vec_(amplitude_t::zeros(hs.size())) { vec_[f] = value_type(1.0); }

    /// Get the dimension of the associated Hilbert (Fock) space, i.e. \f$ \dim(\mathcal{F}^{(m)}) \f$.
    [[nodiscard]] int size() const { return hs_ptr_->size(); }

    /**
     * @brief Get the amplitude \f$ a_{f_i} \f$ for a given index \f$ i \f$.
     * 
     * @details The integer index \f$ i \f$ indexes the Fock states \f$ \lvert f_i \rangle \f$ in the associated Hilbert
     * (Fock) space \f$ \mathcal{F}^{(m)} \f$.
     *
     * @param i Index \f$ i \f$.
     * @return Reference to the amplitude \f$ a_{f_i} \f$.
     */
    value_type &operator()(int i) { return vec_[i]; }

    /**
     * @brief Get the amplitude \f$ a_{f_i} \f$ for a given index \f$ i \f$.
     * 
     * @details The integer index \f$ i \f$ indexes the Fock states \f$ \lvert f_i \rangle \f$ in the associated Hilbert
     * (Fock) space \f$ \mathcal{F}^{(m)} \f$.
     *
     * @param i Index \f$ i \f$.
     * @return Const reference to the amplitude \f$ a_{f_i} \f$.
     */
    value_type const &operator()(int i) const { return vec_[i]; }

    /**
     * @brief Addition assignment operator to add another state \f$ \lvert \phi \rangle \f$.
     *
     * @details It simply adds the amplitudes \f$ b_{f_i} \f$ of the state \f$ \lvert \phi \rangle \f$ to the current 
     * amplitudes \f$ a_{f_i} \f$.
     *
     * @param phi Other many-body state \f$ \lvert \phi \rangle \f$.
     * @return Reference to `this`.
     */
    state &operator+=(state const &phi) {
      vec_ += phi.vec_;
      return *this;
    }

    /**
     * @brief Subtraction assignment operator to subtract another state \f$ \lvert \phi \rangle \f$.
     *
     * @details It simply subtracts the amplitudes \f$ b_{f_i} \f$ of the state \f$ \lvert \phi \rangle \f$ from the 
     * current amplitudes \f$ a_{f_i} \f$.
     *
     * @param phi Other many-body state \f$ \lvert \phi \rangle \f$.
     * @return Reference to `this`.
     */
    state &operator-=(state const &phi) {
      vec_ -= phi.vec_;
      return *this;
    }

    /**
     * @brief Multiplication assignment operator to multiply a scalar \f$ x \f$.
     *
     * @details It simply multiplies the current amplitudes \f$ a_{f_i} \f$ by \f$ x \f$.
     *
     * @param x Scalar mulitplication factor \f$ x \f$.
     * @return Reference to `this`.
     */
    state &operator*=(value_type x) {
      vec_ *= x;
      return *this;
    }

    /**
     * @brief Division assignment operator to divide by a scalar \f$ x \f$.
     *
     * @details It simply divides the current amplitudes \f$ a_{f_i} \f$ by \f$ x \f$.
     *
     * @param x Scalar divisor \f$ x \f$.
     * @return Reference to `this`.
     */
    state &operator/=(value_type x) {
      vec_ /= x;
      return *this;
    }

    /**
     * @brief Calculate the dot product of two states \f$ \langle \phi | \psi \rangle \f$.
     * 
     * @details It computes the dot product as
     * \f[
     *   \langle \phi | \psi \rangle = \sum_{f_i \in \mathcal{F}^{(m)}} b_{f_i}^* a_{f_i} \; ,
     * \f]
     * where \f$ b_{f_i} \f$ and \f$ a_{f_i} \f$ are the amplitudes of the states \f$ \lvert \phi \rangle \f$ and \f$ 
     * \lvert \psi \rangle \f$, respectively.
     *
     * @param phi Many-body state \f$ \lvert \phi \rangle \f$.
     * @param psi Many-body state \f$ \lvert \psi \rangle \f$.
     * @return Dot product \f$ \langle \phi | \psi \rangle \f$.
     */
    friend value_type dot_product(state const &phi, state const &psi) { return nda::blas::dotc(phi.vec_, psi.vec_); }

    /**
     * @brief Apply a callable object to all amplitudes of a state \f$ \lvert \psi \rangle \f$.
     *
     * @details It calls the given callable object for each pair of state index \f$ i \f$ and corresponding amplitude 
     * \f$ a_{f_i} \f$.
     *
     * @tparam F Callable type.
     * @param psi Many-body state \f$ \lvert \psi \rangle \f$.
     * @param lambda Callable object.
     */
    template <typename F> friend void foreach (state const &psi, F lambda) {
      const auto dim = psi.size();
      for (size_t i = 0; i < dim; ++i) lambda(i, psi(i));
    }

    /// Get the amplitudes \f$ a_{f_i} \f$ as an `nda::vector`.
    amplitude_t const &amplitudes() const { return vec_; }

    /// Get the amplitudes \f$ a_{f_i} \f$ as an `nda::vector`.
    amplitude_t &amplitudes() { return vec_; }

    /// Get the Hilbert (Fock) space \f$ \mathcal{F}^{(m)} \f$ the current state belongs to.
    HilbertSpace const &get_hilbert() const { return *hs_ptr_; }

    /**
     * @brief Set the Hilbert (Fock) space \f$ \mathcal{F}^{(m)} \f$ for this state.
     * @param new_hs New Hilbert (Fock) space \f$ \mathcal{F}^{(m)} \f$.
     */
    void set_hilbert(HilbertSpace const &new_hs) { hs_ptr_ = &new_hs; }

    private:
    const HilbertSpace *hs_ptr_;
    amplitude_t vec_;
  };

  /**
   * @brief Write a triqs::hilbert_space::state<HilbertSpace, ScalarType, true> or a 
   * triqs::hilbert_space::state<HilbertSpace, ScalarType, false> to a `std::ostream`.
   *
   * @tparam HilbertSpace Hilbert space type.
   * @tparam ScalarType Amplitude type.
   * @tparam BasedOnMap Whether the amplitudes are stored in a map or a vector.
   * @param sout `std::ostream` object.
   * @param psi State to be written.
   * @return Reference to `std::ostream` object.
   */
  template <typename HilbertSpace, typename ScalarType, bool BasedOnMap>
  std::ostream &operator<<(std::ostream &sout, state<HilbertSpace, ScalarType, BasedOnMap> const &psi) {
    bool something_written = false;
    auto const &hs         = psi.get_hilbert();

    foreach (psi, [&sout, hs, &something_written](int i, auto a_f_i) {
      using triqs::utility::is_zero;
      if (!is_zero(a_f_i)) {
        sout << " +(" << a_f_i << ")"
             << "|" << hs.get_fock_state(i) << ">";
        something_written = true;
      }
    });

    if (!something_written) sout << 0;
    return sout;
  }

  /**
   * @brief Project a state \f$ \lvert \psi \rangle \in \mathcal{F}^{(m)} \f$ to a new Hilbert (Fock) space \f$ 
   * \mathcal{F}' \f$.
   *
   * @details The projection is done by creating the target state \f$ \lvert \psi' \rangle \in \mathcal{F}' = 0 \f$ and 
   * by copying amplitudes from the original to the target state, i.e. we set \f$ a'_{f_i} = a_{f_i} \f$ for all \f$ 
   * \lvert f_i \rangle \in B_{\mathcal{F}^{(m)}} \f$.
   * 
   * @note The original Hilbert (Fock) space can either be a triqs::hilbert_space::hilbert_space or a 
   * triqs::hilbert_space::sub_hilbert_space. However, it is assumed that all the basis states \f$ \lvert f_i \rangle 
   * \in \mathcal{F}^{(m)} \f$ are also contained in \f$ \mathcal{F}' \f$.
   *
   * @tparam TargetState Type of the target state.
   * @tparam OriginalState Type of the original state.
   * @param psi Original state \f$ \lvert \psi \rangle \in \mathcal{F}^{(m)} \f$.
   * @param proj_hs Hilbert (Fock) space \f$ \mathcal{F}' \f$ to project into.
   * @return Projected state \f$ \lvert \psi' \rangle \in \mathcal{F}' \f$.
   */
  template <typename TargetState, typename OriginalState> TargetState project(OriginalState const &psi, hilbert_space const &proj_hs) {
    TargetState proj_psi(proj_hs);
    auto const &hs = psi.get_hilbert();
    foreach (psi, [&](int i, auto a_f_i) { proj_psi(hs.get_fock_state(i)) = a_f_i; });
    return proj_psi;
  }

  /**
   * @brief Project a state \f$ \lvert \psi \rangle \in \mathcal{F}^{(m)} \f$ to a new Hilbert (Fock) subspace \f$ 
   * \mathcal{F}'^{(m')} \f$.
   *
   * @details The projection is done by creating the target state \f$ \lvert \psi' \rangle \in \mathcal{F}'^{(m')} = 0 
   * \f$ and by copying amplitudes from the original to the target state for common basis states, i.e. we set \f$ 
   * a'_{f_i} = a_{f_i} \f$ for all \f$ \lvert f_i \rangle \in B_{\mathcal{F}^{(m)}} \cap B_{\mathcal{F}'^{(m')}} \f$.
   * 
   * @note The original Hilbert (Fock) space can either be a triqs::hilbert_space::hilbert_space or a 
   * triqs::hilbert_space::sub_hilbert_space.
   *
   * @tparam TargetState Type of the target state.
   * @tparam OriginalState Type of the original state.
   * @param psi Original state \f$ \lvert \psi \rangle \in \mathcal{F}^{(m)} \f$.
   * @param proj_hs Hilbert (Fock) space \f$ \mathcal{F}' \f$ to project into.
   * @return Projected state \f$ \lvert \psi' \rangle \in \mathcal{F}' \f$.
   */
  template <typename TargetState, typename OriginalState> TargetState project(OriginalState const &psi, sub_hilbert_space const &proj_hs) {
    TargetState proj_psi(proj_hs);
    auto const &hs = psi.get_hilbert();
    foreach (psi, [&](int i, auto a_f_i) {
      auto f = hs.get_fock_state(i);
      if (proj_hs.has_state(f)) proj_psi(proj_hs.get_state_index(f)) = a_f_i;
    });
    return proj_psi;
  }

  /** @} */

} // namespace triqs::hilbert_space
