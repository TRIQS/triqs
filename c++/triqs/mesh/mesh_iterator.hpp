// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Thomas Ayral, Philipp Dumitrescu, Michel Ferrero, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides a generic random access iterator for 1D meshes.
 */

#pragma once

#include <compare>
#include <cstddef>
#include <iterator>

namespace triqs::mesh {

  /**
   * @brief A generic random access iterator for 1D meshes.
   * 
   * @details The iterator simply store a pointer to the underlying mesh and the data index of the current mesh point.
   * It uses the mesh's `operator[]` to access the mesh points via the data index.
   * 
   * @tparam M Mesh type.
   */
  template <typename M> struct mesh_iterator {
    /// Value type.
    using value_type = typename M::mesh_point_t;

    /// Iterator category.
    using iterator_category = std::random_access_iterator_tag;

    /// Pointer type.
    using pointer = value_type *;

    /// Difference type.
    using difference_type = std::ptrdiff_t;

    /// Reference type.
    using reference = value_type const &;

    /// Pointer to the underlying mesh.
    M const *mesh_ptr{nullptr};

    /// Current data index.
    long data_index{-1};

    /**
     * @brief Pre-increment operator increments the current data index by \f$ 1 \f$.
     * @return Reference to `this` iterator.
     */
    mesh_iterator &operator++() noexcept {
      ++data_index;
      return *this;
    }

    /**
     * @brief Post-increment operator increments the current data index by \f$ 1 \f$.
     * @return Copy of `this` iterator before incrementing.
     */
    mesh_iterator operator++(int) noexcept {
      mesh_iterator tmp = *this;
      ++data_index;
      return tmp;
    }

    /**
     * @brief Pre-decrement operator decrements the current data index by \f$ 1 \f$.
     * @return Reference to `this` iterator.
     */
    mesh_iterator &operator--() noexcept {
      --data_index;
      return *this;
    }

    /**
     * @brief Post-decrement operator decrements the current data index by \f$ 1 \f$.
     * @return Copy of `this` iterator before decrementing.
     */
    mesh_iterator operator--(int) noexcept {
      mesh_iterator tmp = *this;
      --data_index;
      return tmp;
    }

    /**
     * @brief Three-way comparison operator for two iterators.
     *
     * @param rhs Right hand side iterator to compare with.
     * @return It returns the result of a three-way comparison of their current values.
     */
    [[nodiscard]] std::strong_ordering operator<=>(mesh_iterator const &rhs) const noexcept { return data_index <=> rhs.data_index; }

    /**
     * @brief Equal-to operator for two iterators.
     *
     * @param other Iterator to compare with.
     * @return True, if they belong to the same mesh and if their current data indices are equal.
     */
    [[nodiscard]] bool operator==(mesh_iterator const &other) const noexcept {
      return mesh_ptr == other.mesh_ptr and data_index == other.data_index;
    }

    /**
     * @brief Dereference operator.
     * @return Mesh point at the current data index.
     */
    [[nodiscard]] value_type operator*() const noexcept { return mesh_ptr->operator[](data_index); }

    /**
     * @brief Member access operator.
     * @return Mesh point at the current data index.
     */
    [[nodiscard]] value_type operator->() const noexcept { return operator*(); }

    /**
     * @brief Addition assignment operator.
     * @param n Number of steps to add to the current data index.
     * @return Reference to `this` iterator with its current data index increased by \f$ n \f$.
     */
    mesh_iterator &operator+=(difference_type n) noexcept {
      data_index += n;
      return *this;
    }

    /**
     * @brief Addition operator for an iterator and an integer.
     * @param n Number of steps to add to the current data index.
     * @return Copy of `this` object with its current data index increased by \f$ n \f$.
     */
    [[nodiscard]] mesh_iterator operator+(difference_type n) const noexcept { return {.mesh_ptr = mesh_ptr, .data_index = data_index + n}; }

    /**
     * @brief Addition operator for an integer and an iterator.
     * @param n Number of steps to add to the current data index of the iterator.
     * @param it Iterator.
     * @return Copy of the given iterator with its current data index increased by \f$ n \f$.
     */
    [[nodiscard]] friend mesh_iterator operator+(difference_type n, mesh_iterator it) noexcept { return it + n; }

    /**
     * @brief Subtraction assignment operator.
     * @param n Number of steps to subtract from the current data index.
     * @return Reference to `this` iterator with its current data index decreased by \f$ n \f$.
     */
    mesh_iterator &operator-=(difference_type n) noexcept {
      data_index -= n;
      return *this;
    }

    /**
     * @brief Subtraction operator for an iterator and an integer.
     * @param n Number of steps to subtract from the current data index.
     * @return Copy of `this` iterator with its current data index decreased by \f$ n \f$.
     */
    [[nodiscard]] mesh_iterator operator-(difference_type n) const noexcept { return {.mesh_ptr = mesh_ptr, .data_index = data_index - n}; }

    /**
     * @brief Get the distance between two iterators.
     * @param rhs Right-hand side iterator.
     * @return Number of steps between the two iterators.
     */
    [[nodiscard]] difference_type operator-(mesh_iterator const &rhs) const noexcept { return data_index - rhs.data_index; }

    /**
     * @brief Subscript operator.
     * @param n Number of steps to add to the current value.
     * @return Mesh point at the current data index increased by \f$ n \f$.
     */
    [[nodiscard]] value_type operator[](difference_type n) const noexcept { return mesh_ptr->operator[](data_index + n); }
  };

} // namespace triqs::mesh
