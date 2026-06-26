// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
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
 * @brief Provides a fundamental operator set class.
 */

#pragma once

#include "../gfs/block/gf_struct.hpp"
#include "../utility/dressed_iterator.hpp"
#include "../utility/exceptions.hpp"

#include <h5/h5.hpp>
#include <itertools/itertools.hpp>

#include <algorithm>
#include <array>
#include <iterator>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace triqs::hilbert_space {

  /**
   * @addtogroup triqs-hilbert
   * @{
   */

  /// Index type for single particle state indices \f$ \alpha_i \f$.
  using indices_t = std::vector<std::variant<long, std::string, double, std::array<long, 3>>>;

  /// Elevate triqs::gfs::gf_struct_t to the `triqs::hilbert_space` namespace.
  using triqs::gfs::gf_struct_t;

  /**
   * @brief String representation of a single particle state index \f$ \alpha_i \f$.
   * 
   * @details The individual index elements of \f$ \alpha_i \f$ are first converted to strings (if necessary) and then 
   * concatenated using the given separator `sep`. String index elements are enclosed in single quotes.
   * 
   * Optional prefix and suffix strings can be added around the entire representation.
   * 
   * @param alpha Single particle state index \f$ \alpha_i \f$.
   * @param sep Separator string inserted between individual index elements.
   * @param prefix Optional prefix string.
   * @param suffix Optional suffix string.
   * @return Formatted string representation of \f$ \alpha_i \f$.
   */
  std::string format_indices(indices_t const &alpha, std::string_view sep = ",", std::string_view prefix = "", std::string_view suffix = "");

  /**
   * @brief Class representing a fundamental operator set.
   *
   * @details A fundamental operator set is an ordered set of single particle state indices, \f$ A = \{ \alpha_i 
   * \}_{i=0}^{N-1} \f$, where the corresponding states \f$ \{ \lvert \alpha_i \rangle \}_{i=0}^{N-1} \f$ span a 
   * 1-particle Hilbert space \f$ \mathcal{H}_1 \f$ of finite dimension \f$ N \f$. By ordered, we mean that there is a
   * strict total order imposed on the set \f$ A \f$ such that \f$ \alpha_i < \alpha_j \f$ if \f$ i < j \f$.
   * 
   * Each index \f$ \alpha_i \f$ can consist of an arbitrarily long sequence of integers, strings, doubles and arrays of
   * integers. We write \f$ \alpha_i = (\beta^{(i)}_1, \dots, \beta^{(i)}_{k_i}) \f$, where each \f$ \beta^{(i)}_j \f$ 
   * is either an integer, a string, a double or an array of integers.
   * 
   * For example, fermionic operators are often characterized by a spin index \f$ \sigma \f$ and an orbital index \f$ a
   * \f$ such that \f$ \alpha = (\sigma, a) \f$. Considering fermions with spins \f$ \sigma \in \{ \text{"up"},
   * \text{"down"} \} \f$ and 3 orbitals \f$ a \in \{ 0, 1, 2 \} \f$, the fundamental operator set is given by
   * \f[
   *   A = \{ (\text{"up"}, 0), (\text{"up"}, 1), (\text{"up"}, 2), (\text{"down"}, 0), (\text{"down"}, 1),
   *   (\text{"down"}, 2) \} \; .
   * \f]
   */
  class fundamental_operator_set {
    public:
    /// Index type to represent a single \f$ \alpha_i \f$.
    using indices_t = triqs::hilbert_space::indices_t;

    /// Container type to store \f$ A = \{ \alpha_i \}_{i=0}^{N-1} \f$.
    using data_t = std::vector<indices_t>;

    private:
    // Constructor for a vector of vector of strings (only used in h5_read_attribute).
    fundamental_operator_set(std::vector<std::vector<std::string>> const &);

    public:
    /// Default constructor leaves the set of indices empty, i.e. \f$ A = \emptyset \f$.
    fundamental_operator_set() = default;

    /**
     * @brief Construct a fundamental operator set from a vector of integers \f$ \mathbf{v} = (v_1, \dots, v_k) \f$.
     *
     * @details The set will contain indices \f$ \alpha_i = (\beta^{(i)}_1, \beta^{(i)}_2) \f$ with \f$ \beta^{(i)}_1
     * \in \{ 0, \dots, k-1 \} \f$ and \f$ \beta^{(i)}_2 \in \{ 0, \dots, v_{\beta^{(i)}_1} \} \f$.
     *
     * For example, given \f$ \mathbf{v} = (2, 1, 3) \f$, the generated fundamental operator set is
     * \f[
     *   A = \{ (0, 0), (0, 1), (1, 0), (2, 0), (2, 1), (2, 2) \} \; .
     * \f]
     *
     * @param v Vector of integers \f$ \mathbf{v} \f$.
     */
    fundamental_operator_set(std::vector<int> const &v) {
      for (int i = 0; i < v.size(); ++i)
        for (int j = 0; j < v[i]; ++j) insert(i, j);
    }

    /**
     * @brief Construct a fundamental operator set from a set of indices \f$ A = \{ \alpha_0, \dots, \alpha_{N-1} \} 
     * \f$.
     * 
     * @note The order of the indices in the set is preserved.
     * 
     * @tparam IndexType Index type.
     * @param A Set of indices \f$ A \f$.
     */
    template <typename IndexType> fundamental_operator_set(std::set<IndexType> const &A) {
      for (auto const &alpha : A) insert(alpha);
    }

    /**
     * @brief Construct a fundamental operator set from a vector of indices \f$ \mathbf{v} = (\alpha_0, \dots, 
     * \alpha_{N-1}) \f$.
     * 
     * @note The order of the indices in the vector is preserved.
     * 
     * @param v Vector of indices \f$ \mathbf{v} \f$.
     */
    explicit fundamental_operator_set(data_t v) : idxs_(std::move(v)) {}

    /**
     * @brief Construct a fundamental operator set from a triqs::gfs::gf_struct_t object.
     *
     * @details A triqs::gfs::gf_struct_t object determines the shape of a block Green's function. It is a vector of
     * pairs, where each pair consists of a block name \f$ s \f$ and block size \f$ b_s \f$. The size of the vector \f$
     * k \f$ corresponds to the number of blocks.
     *
     * The set will contain indices \f$ \alpha_i = (\beta^{(i)}_1, \beta^{(i)}_2) \f$ with \f$ \beta^{(i)}_1 \in \{ s_1,
     * \dots, s_k \} \f$ and \f$ \beta^{(i)}_2 \in \{ 0, \dots, b_{\beta^{(i)}_1} - 1 \} \f$.
     * 
     * For example, given the block structure \f$\left( (\text{"up"}, 2), (\text{"down"}, 3) \right) \f$, the generated 
     * fundamental operator set is
     * \f[
     *   A = \{ (\text{"up"}, 0), (\text{"up"}, 1), (\text{"down"}, 0), (\text{"down"}, 1), (\text{"down"}, 2) \} \; .
     * \f]
     *
     * @param gf_struct A triqs::gfs::gf_struct_t object representing the structure of a Green's function.
     */
    fundamental_operator_set(gf_struct_t const &gf_struct) {
      for (auto const &[blname, blsize] : gf_struct)
        for (auto i : itertools::range(blsize)) insert(blname, i);
    }

    /// Explicit conversion operator to a `std::vector` of indices_t.
    explicit operator data_t() const { return idxs_; }

    /**
     * @brief Insert a new index \f$ \alpha \f$ into the set.
     * 
     * @details The index is inserted at end of the set such that \f$ \alpha_i < \alpha \f$ for all existing indices \f$
     * \alpha_i \f$ in the set.
     * 
     * It does nothing if the index is already present.
     * 
     * @param alpha Index \f$ \alpha \f$ to insert.
     */
    void insert_from_indices_t(indices_t const &alpha) {
      if (!has_indices(alpha)) idxs_.push_back(alpha);
    }

    /**
     * @brief Insert a new index \f$ \alpha = (\beta_1, \dots, \beta_k) \f$ into the set.
     *
     * @details It calls insert_from_indices_t() with the index \f$ \alpha \f$ constructed from the given \f$ \beta_i 
     * \f$.
     *
     * @tparam Bs Types of \f$ \beta_i \f$.
     * @param betas \f$ \beta_1, \dots, \beta_k \f$ that form the index \f$ \alpha \f$.
     */
    template <typename... Bs> void insert(Bs const &...betas) { insert_from_indices_t(indices_t{betas...}); }

    /// Get the number \f$ N \f$ of single particle state indices in the set.
    [[nodiscard]] auto size() const { return static_cast<int>(idxs_.size()); }

    /**
     * @brief Check if a given \f$ \alpha \f$ is in the set.
     *
     * @param alpha Index \f$ \alpha \f$ to look up.
     * @return True if \f$ \alpha = \alpha_i \f$ for some \f$ \alpha_i \in A \f$, false otherwise.
     */
    [[nodiscard]] bool has_indices(indices_t const &alpha) const { return std::ranges::find(idxs_, alpha) != idxs_.end(); }

    /**
     * @brief Subscript operator to get the position of a given index \f$ \alpha \f$ in the set.
     *
     * @details It throws an exception if \f$ \alpha \notin A \f$.
     *
     * @param alpha Index \f$ \alpha \f$ to look up.
     * @return Position of the index \f$ \alpha \f$ in the set.
     */
    [[nodiscard]] auto operator[](indices_t const &alpha) const {
      auto it = std::ranges::find(idxs_, alpha);
      if (it == idxs_.end()) {
        TRIQS_RUNTIME_ERROR << "Error in fundamental_operator_set::operator[]: Operator with indices (" << format_indices(alpha)
                            << ") does not belong to this fundamental set!";
      }
      return std::distance(idxs_.begin(), it);
    }

    /// Equal-to operator compares the ordered sets of indices for equality.
    bool operator==(fundamental_operator_set const &fops) const { return idxs_ == fops.idxs_; }

    /// Get the ordered set \f$ A = \{ \alpha_i \}_{i=0}^{N-1} \f$.
    [[nodiscard]] data_t const &data() const { return idxs_; }

    private:
    // Helper class for the creation of the const iterator.
    using _enum_iterator = decltype(itertools::enumerate(std::declval<data_t>()).cbegin());
    struct _cdress {
      indices_t const &index;
      int linear_index;
      _cdress(_enum_iterator _it) : index(std::get<1>(*_it)), linear_index(std::get<0>(*_it)) {}
    };

    public:
    /// Constant iterator type.
    using const_iterator = triqs::utility::dressed_iterator<_enum_iterator, _cdress>;

    /// Get a const iterator to the beginning of the set.
    [[nodiscard]] const_iterator begin() const noexcept { return itertools::enumerate(idxs_).begin(); }

    /// Get a const iterator to the beginning of the set.
    [[nodiscard]] const_iterator cbegin() const noexcept { return itertools::enumerate(idxs_).cbegin(); }

    /// Get a const iterator to the end of the set.
    [[nodiscard]] auto end() const noexcept { return itertools::enumerate(idxs_).end(); }

    /// Get a const iterator to the end of the set.
    [[nodiscard]] auto cend() const noexcept { return itertools::enumerate(idxs_).cend(); }

    /**
     * @brief Write a triqs::hilbert_space::fundamental_operator_set to HDF5 as an attribute.
     *
     * @param obj `h5::object` the attribute belongs to.
     * @param name Name of the attribute.
     * @param fops Fundamental operator set to be written.
     */
    friend void h5_write_attribute(h5::object obj, std::string const &name, fundamental_operator_set const &fops);

    /**
     * @brief Read a triqs::hilbert_space::fundamental_operator_set from an HDF5 attribute.
     *
     * @param obj `h5::object` the attribute belongs to.
     * @param name Name of the attribute.
     * @param fops Fundamental operator set to be read into.
     */
    friend void h5_read_attribute(h5::object obj, std::string const &name, fundamental_operator_set &fops);

    private:
    data_t idxs_ = {};
  };

  /** @} */

} // namespace triqs::hilbert_space
