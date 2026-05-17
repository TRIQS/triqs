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

#pragma once
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <boost/operators.hpp>
#include <triqs/utility/numeric_ops.hpp>
#include <triqs/arrays.hpp>
#include "hilbert_space.hpp"

namespace triqs::hilbert_space {

  /// Many-body state as a list of amplitudes in a *basis of Fock states*
  /**
  The amplitudes can either be stored using a map (only non-vanishing elements are stored)
  or in a `nda::vector`. In order to make this class model [[statevector_concept]]
  free function [[make_zero_state]] is defined. Specializations on `BasedOnMap` provide
  different extensions of the concept, and are documented separately:

  - :ref:`state, map-based storage <state\<HilbertSpace,ScalarType,true\>>`
  - :ref:`state, vector-based storage <state\<HilbertSpace,ScalarType,false\>>`

  Function [[project]] can be used to project a state from one Hilbert space to another Hilbert space/subspace.

  @tparam HilbertSpace Hilbert space type, one of [[hilbert_space]] and [[sub_hilbert_space]]
  @tparam ScalarType Amplitude type, normally `double` or `std::complex<double>`
  @tparam BasedOnMap Use the implementation based on a map
  @include triqs/hilbert_space/state.hpp
 */
  template <typename HilbertSpace, typename ScalarType, bool BasedOnMap> class state {};

  /// Make a copy of a given state with all amplitudes set to 0
  /**
  @param st State object to copy
  @tparam HilbertSpace Hilbert space type, one of [[hilbert_space]] and [[sub_hilbert_space]]
  @tparam ScalarType Amplitude type, normally `double` or `std::complex<double>`
  @tparam BasedOnMap Use the implementation based on a map
  @include triqs/hilbert_space/state.hpp
 */
  template <typename HilbertSpace, typename ScalarType, bool BasedOnMap>
  state<HilbertSpace, ScalarType, BasedOnMap> make_zero_state(state<HilbertSpace, ScalarType, BasedOnMap> const &st) {
    return {st.get_hilbert()};
  }

  /// State: implementation based on a map
  /**
  This specialization can work well on huge Hilbert spaces, as long as there are not
  too many non-vanishing amplitudes  in the state.

  @tparam HilbertSpace Hilbert space type, one of [[hilbert_space]] and [[sub_hilbert_space]]
  @tparam ScalarType Amplitude type, normally `double` or `std::complex<double>`
  @include triqs/hilbert_space/state.hpp
 */
  template <typename HilbertSpace, typename ScalarType>
  class state<HilbertSpace, ScalarType, true> : boost::additive<state<HilbertSpace, ScalarType, true>>,
                                                boost::multiplicative<state<HilbertSpace, ScalarType, true>, ScalarType> {
    // derivations implement the vector space operations over ScalarType from the compounds operators +=, *=, ....
    public:
    /// Accessor to `ScalarType` template parameter
    using value_type = ScalarType;
    /// Accessor to `HilbertSpace` template parameter
    using hilbert_space_t = HilbertSpace;
    /// Container type for storing amplitudes.
    using amplitude_t = std::unordered_map<fock_state_t, ScalarType>;

    /// Construct a new state object
    /**
   The constructed state is dummy state not belonging to any Hilbert space. **It should not be used in expressions!**
  */
    state() : hs_ptr_(nullptr) {}
    /// Construct a new state object
    /**
   @param hs Hilbert space the new state belongs to
  */
    state(HilbertSpace const &hs) : hs_ptr_(&hs) {}

    /// Construct a new state object and set the st-th amplitude to 1.
    /**
       * @param hs Hilbert space the new state belongs to
       * @param st Basis state with the unity amplitude
       */
    state(HilbertSpace const &hs, fock_state_t st) : hs_ptr_(&hs) { map_[st] = value_type(1.0); }

    /// Return the dimension of the associated Hilbert space
    /**
   @return Dimension of the associated Hilbert space
  */
    int size() const { return hs_ptr_->size(); }

    /// Number of non-vanishing amplitudes in the state
    /**
   @return Number of non-vanishing amplitudes
  */
    int nterms() const { return map_.size(); }

    /// Access to individual amplitudes
    /**
   @param i index of the requested amplitude
   @return Reference to the requested amplitude
  */
    value_type &operator()(fock_state_t i) { return map_[i]; }
    /// Access to individual amplitudes
    /**
   @param i index of the requested amplitude
   @return Constant reference to the requested amplitude
  */
    value_type const &operator()(fock_state_t i) const { return map_[i]; }

    /// In-place addition of another state
    /**
   @param s2 Another [[state]] object to add
   @return Reference to this state
  */
    state &operator+=(state const &s2) {
      for (auto const &aa : s2.map_) {
        auto r = map_.insert(aa);
        if (!r.second) r.first->second += aa.second;
      }
      prune();
      return *this;
    }

    /// In-place subtraction of another state
    /**
   @param s2 Another [[state]] object to add
   @return Reference to this state
  */
    state &operator-=(state const &s2) {
      for (auto const &aa : s2.map_) {
        auto r = map_.insert({aa.first, -aa.second});
        if (!r.second) r.first->second -= aa.second;
      }
      prune();
      return *this;
    }

    /// In-place multiplication by a scalar
    /**
   @param x Multiplier
   @return Reference to this state
  */
    state &operator*=(value_type x) {
      for (auto &a : map_) { a.second *= x; }
      prune();
      return *this;
    }

    /// In-place division by a scalar
    /**
   @param x Divisor
   @return Reference to this state
  */
    state &operator/=(value_type x) { return operator*=(value_type(1) / x); }

    /// Calculate scalar product of two states
    /**
   @param s1 First state to multiply
   @param s2 Second state to multiply
   @return Value of the scalar product
  */
    friend value_type dot_product(state const &s1, state const &s2) {
      value_type res(0);
      for (auto const &a : s1.map_) {
        using triqs::utility::conj;
        if (s2.map_.count(a.first) == 1) res += conj(a.second) * s2.map_.at(a.first);
      }
      return res;
    }

    /// Apply a callable object to **non-vanishing** amplitudes of a state
    /**
  The callable must take two arguments, 1) index of the basis Fock state in the associated Hilbert space, and 2) the corresponding amplitude.

  @tparam w_max Type of the callable object
  @param st State object
  @param l Callable object
  */
    template <typename w_max> friend void foreach (state const &st, w_max l) {
      const_cast<state &>(st).prune();
      for (auto const &p : st.map_) l(p.first, p.second);
    }

    //
    // Additions to StateVector concept
    //

    /// Return a constant reference to the associated Hilbert space
    /**
   @return Constant reference to the Hilbert space
  */
    HilbertSpace const &get_hilbert() const { return *hs_ptr_; }
    /// Reset the associated Hilbert space
    /**
   @param new_hs Constant reference to the new Hilbert space
  */
    void set_hilbert(HilbertSpace const &new_hs) { hs_ptr_ = &new_hs; }

    private:
    void prune() {
      for (auto it = map_.begin(); it != map_.end();) {
        using triqs::utility::is_zero;
        if (is_zero(it->second))
          it = map_.erase(it);
        else
          ++it;
      }
    }

    private:
    const HilbertSpace *hs_ptr_;
    amplitude_t map_;
  };

  /// State: implementation based on `nda::vector`
  template <typename HilbertSpace, typename ScalarType>
  class state<HilbertSpace, ScalarType, false> : boost::additive<state<HilbertSpace, ScalarType, false>>,
                                                 boost::multiplicative<state<HilbertSpace, ScalarType, false>, ScalarType> {
    public:
    /// Accessor to `ScalarType` template parameter
    using value_type = ScalarType;
    /// Accessor to `HilbertSpace` template parameter
    using hilbert_space_t = HilbertSpace;
    /// Container type for storing amplitudes.
    using amplitude_t = nda::vector<ScalarType>;

    /// Construct a new state object
    /**
   The constructed state is dummy state not belonging to any Hilbert space. **It should not be used in expressions!**
  */
    state() : hs_ptr_(nullptr) {}
    /// Construct a new state object
    /**
   @param hs Hilbert space the new state belongs to
  */
    state(HilbertSpace const &hs) : hs_ptr_(&hs), vec_(nda::zeros<ScalarType>(hs.size())) {}

    /// Construct a new state object and set the st-th amplitude to 1.
    /**
       * @param hs Hilbert space the new state belongs to
       * @param st Basis state with the unity amplitude
       */
    state(HilbertSpace const &hs, fock_state_t st) : hs_ptr_(&hs), vec_(nda::zeros<ScalarType>(hs.size())) { vec_[st] = value_type(1.0); }

    /// Return the dimension of the associated Hilbert space
    /**
   @return Dimension of the associated Hilbert space
  */
    int size() const { return hs_ptr_->size(); }

    /// Access to individual amplitudes
    /**
   @param i index of the requested amplitude
   @return Reference to the requested amplitude
  */
    value_type &operator()(int i) { return vec_[i]; }
    /// Access to individual amplitudes
    /**
   @param i index of the requested amplitude
   @return Constant reference to the requested amplitude
  */
    value_type const &operator()(int i) const { return vec_[i]; }

    /// In-place addition of another state
    /**
   @param s2 Another [[state]] object to add
   @return Reference to this state
  */
    state &operator+=(state const &s2) {
      vec_ += s2.vec_;
      return *this;
    }

    /// In-place subtraction of another state
    /**
   @param s2 Another [[state]] object to add
   @return Reference to this state
  */
    state &operator-=(state const &s2) {
      vec_ -= s2.vec_;
      return *this;
    }

    /// In-place multiplication by a scalar
    /**
   @param x Multiplier
   @return Reference to this state
  */
    state &operator*=(value_type x) {
      vec_ *= x;
      return *this;
    }

    /// In-place division by a scalar
    /**
   @param x Divisor
   @return Reference to this state
  */
    state &operator/=(value_type x) {
      vec_ /= x;
      return *this;
    }

    /// Calculate scalar product of two states
    /**
   @param s1 First state to multiply
   @param s2 Second state to multiply
   @return Value of the scalar product
  */
    friend value_type dot_product(state const &s1, state const &s2) { return nda::blas::dotc(s1.vec_, s2.vec_); }

    /// Apply a callable object to all amplitudes of a state
    /**
  The callable must take two arguments, 1) index of the basis Fock state in the associated Hilbert space, and 2) the corresponding amplitude.

  @tparam w_max Type of the callable object
  @param st State object
  @param l Callable object
  */
    template <typename w_max> friend void foreach (state const &st, w_max l) {
      const auto L = st.size();
      for (size_t i = 0; i < L; ++i) l(i, st(i));
    }

    //
    // Additions to StateVector concept
    //

    /// Direct access to the storage container (`nda::vector`)
    /**
   @return Constant reference to the storage container
  */
    amplitude_t const &amplitudes() const { return vec_; }
    /// Direct access to the storage container (`nda::vector`)
    /**
   @return Reference to the storage container
  */
    amplitude_t &amplitudes() { return vec_; }

    /// Return a constant reference to the associated Hilbert space
    /**
   @return Constant reference to the Hilbert space
  */
    HilbertSpace const &get_hilbert() const { return *hs_ptr_; }
    /// Reset the associated Hilbert space
    /**
   @param new_hs Constant reference to the new Hilbert space
  */
    void set_hilbert(HilbertSpace const &new_hs) { hs_ptr_ = &new_hs; }

    private:
    const HilbertSpace *hs_ptr_;
    amplitude_t vec_;
  };

  // Print state
  template <typename HilbertSpace, typename ScalarType, bool BasedOnMap>
  std::ostream &operator<<(std::ostream &os, state<HilbertSpace, ScalarType, BasedOnMap> const &s) {
    bool something_written = false;
    auto const &hs         = s.get_hilbert();

    using value_type = typename state<HilbertSpace, ScalarType, BasedOnMap>::value_type;
    foreach (s, [&os, hs, &something_written](int i, value_type ampl) {
      using triqs::utility::is_zero;
      if (!is_zero(ampl)) {
        os << " +(" << ampl << ")"
           << "|" << hs.get_fock_state(i) << ">";
        something_written = true;
      }
    });

    if (!something_written) os << 0;
    return os;
  }

  template <typename TargetState, typename OriginalState> TargetState project(OriginalState const &psi, hilbert_space const &proj_hs) {
    TargetState proj_psi(proj_hs);
    auto const &hs = psi.get_hilbert();
    foreach (psi, [&](int i, typename OriginalState::value_type v) { proj_psi(hs.get_fock_state(i)) = v; });
    return proj_psi;
  }

  template <typename TargetState, typename OriginalState> TargetState project(OriginalState const &psi, sub_hilbert_space const &proj_hs) {
    TargetState proj_psi(proj_hs);
    auto const &hs = psi.get_hilbert();
    foreach (psi, [&](int i, typename OriginalState::value_type v) {
      auto f = hs.get_fock_state(i);
      if (proj_hs.has_state(f)) proj_psi(proj_hs.get_state_index(f)) = v;
    });
    return proj_psi;
  }

} // namespace triqs::hilbert_space
