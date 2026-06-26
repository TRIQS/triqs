// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief STL-compatible iterator wrapper that dresses an underlying iterator with a user-defined view type.
 */

#pragma once

#include "./first_include.hpp"

#include <boost/iterator/iterator_facade.hpp>
#include <itertools/itertools.hpp>

#include <iterator>
#include <type_traits>

namespace triqs::utility {

  // Forward declaration.
  template <typename IteratorType, typename Dressing, typename DressingAuxiliaryArgumentPtrType = void> struct dressed_iterator;

  namespace detail {

    template <typename IteratorType>
    constexpr bool is_bidir = std::is_same_v<typename IteratorType::iterator_category, std::bidirectional_iterator_tag>;

  } // namespace detail

  /**
   * @addtogroup triqs-utility-other
   * @{
   */

  /**
   * @brief STL-compatible iterator that wraps an underlying iterator and dereferences to a user-supplied dressing type.
   *
   * @details Given an iterator type `IteratorType` and a small dressing struct `Dressing` whose constructor takes the 
   * iterator (and optionally an auxiliary pointer) and exposes the data via references, `dressed_iterator` produces a 
   * bidirectional or forward iterator (depending on the iterator category of the given iterator type) generated via
   * `boost::iterator_facade`. Dereferencing the iterator returns a fresh `Dressing` constructed from the current
   * underlying iterator state.
   *
   * Optionally, an auxiliary pointer (typically to the parent container) can be carried along — useful e.g. for
   * cyclic iterators that need to consult the container size.
   *
   * @tparam IteratorType Underlying iterator type.
   * @tparam Dressing Lightweight view struct constructed from a single iterator (or iterator + aux pointer).
   * @tparam DressingAuxiliaryArgumentPtrType Optional pointer type stored alongside the iterator (`void` to disable).
   */
  template <typename IteratorType, typename Dressing, typename DressingAuxiliaryArgumentPtrType>
  struct dressed_iterator
     : public boost::iterator_facade<
          dressed_iterator<IteratorType, Dressing, DressingAuxiliaryArgumentPtrType>, Dressing,
          std::conditional_t<detail::is_bidir<IteratorType>, boost::bidirectional_traversal_tag, boost::forward_traversal_tag>, Dressing> {
    public:
    /// Default constructor leaves the underlying iterator and auxiliary pointer uninitialized.
    dressed_iterator() = default;

    /**
     * @brief Construct a dressed iterator from an underlying iterator and an auxiliary pointer.
     * 
     * @param it Underlying iterator.
     * @param aux Auxiliary pointer (typically to the parent container).
     */
    template <typename T> dressed_iterator(T const &it, DressingAuxiliaryArgumentPtrType *aux) : _it(it), _aux(aux) {}

    // Special member functions (should be removed probably).
    dressed_iterator(dressed_iterator const &it)            = default;
    dressed_iterator(dressed_iterator &&it)                 = default;
    dressed_iterator &operator=(dressed_iterator const &it) = default;
    dressed_iterator &operator=(dressed_iterator &&it) noexcept {
      using std::swap;
      swap(it._it, this->_it);
      swap(this->_aux, it._aux);
      return *this;
    }

    /**
     * @brief Access the underlying iterator.
     * @return Const reference to the underlying iterator.
     */
    IteratorType const &get() const { return _it; }

    /**
     * @brief Access the underlying iterator.
     * @return Reference to the underlying iterator.
     */
    IteratorType &get() { return _it; }

    /// Implicit conversion back to the underlying iterator type.
    operator IteratorType() const { return _it; }

    /**
     * @brief Access the auxiliary pointer.
     * @return Pointer to the auxiliary argument.
     */
    auto *get_aux() { return _aux; }

    /**
     * @brief Access the auxiliary pointer.
     * @return Const pointer to the auxiliary argument.
     */
    const auto *get_aux() const { return _aux; }

    /**
     * @brief Equal-to operator to compare a `itertools::sentinel_t` type with a dressed iterator.
     * 
     * @tparam OtherSentinel Type of the other sentinel.
     * @param other Sentinel object to compare with.
     * @return True if the underlying iterator is equal to the sentinel's iterator.
     */
    template <typename OtherSentinel> bool operator==(itertools::sentinel_t<OtherSentinel> other) { return _it == other.it; }

    private:
    // Core operations required by boost::iterator_facade; iterator_core_access lets the facade call them while private.
    // increment/equal/dereference implement forward traversal, decrement adds bidirectional traversal.
    friend class boost::iterator_core_access;
    void increment() { ++_it; }
    void decrement()
      requires(detail::is_bidir<IteratorType>)
    {
      --_it;
    }
    bool equal(dressed_iterator const &other) const { return (other._it == _it); }
    Dressing dereference() const { return Dressing(_it, _aux); }

    private:
    IteratorType _it;
    DressingAuxiliaryArgumentPtrType *_aux;
  };

  /**
   * @brief Specialization of triqs::utility::dressed_iterator for the case where no auxiliary pointer is needed.
   * 
   * @tparam IteratorType Underlying iterator type.
   * @tparam Dressing Lightweight view struct constructed from a single iterator (or iterator + aux pointer).
   */
  template <typename IteratorType, typename Dressing>
  struct dressed_iterator<IteratorType, Dressing, void>
     : public boost::iterator_facade<
          dressed_iterator<IteratorType, Dressing>, Dressing,
          std::conditional_t<detail::is_bidir<IteratorType>, boost::bidirectional_traversal_tag, boost::forward_traversal_tag>, Dressing> {
    public:
    /// Default constructor leaves the underlying iterator uninitialized.
    dressed_iterator() = default;

    /**
     * @brief Construct a dressed iterator from an underlying iterator.
     * @param it Underlying iterator.
     */
    template <typename T> dressed_iterator(T const &it) : _it(it) {}

    // Special member functions (should be removed probably).
    dressed_iterator(dressed_iterator const &it)            = default;
    dressed_iterator(dressed_iterator &&it)                 = default;
    dressed_iterator &operator=(dressed_iterator const &it) = default;
    dressed_iterator &operator=(dressed_iterator &&it) noexcept {
      using std::swap;
      swap(it._it, this->_it);
      return *this;
    }

    /**
     * @brief Access the underlying iterator.
     * @return Const reference to the underlying iterator.
     */
    IteratorType const &get() const { return _it; }

    /**
     * @brief Access the underlying iterator.
     * @return Reference to the underlying iterator.
     */
    IteratorType &get() { return _it; }

    /// Implicit conversion back to the underlying iterator type.
    operator IteratorType() const { return _it; }

    /**
     * @brief Equal-to operator to compare a `itertools::sentinel_t` type with a dressed iterator.
     * 
     * @tparam OtherSentinel Type of the other sentinel.
     * @param other Sentinel object to compare with.
     * @return True if the underlying iterator is equal to the sentinel's iterator.
     */
    template <typename OtherSentinel> bool operator==(itertools::sentinel_t<OtherSentinel> other) { return _it == other.it; }

    private:
    // Core operations required by boost::iterator_facade; iterator_core_access lets the facade call them while private.
    // increment/equal/dereference implement forward traversal, decrement adds bidirectional traversal.
    friend class boost::iterator_core_access;
    void increment() { ++_it; }
    void decrement()
      requires(detail::is_bidir<IteratorType>)
    {
      --_it;
    }
    bool equal(dressed_iterator const &other) const { return (other._it == _it); }
    Dressing dereference() const { return Dressing(_it); }

    private:
    IteratorType _it;
  };

  /** @} */

} // namespace triqs::utility
