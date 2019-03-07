/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013, P. Seth, I. Krivenko, M. Ferrero and O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once
#include <triqs/utility/variant.hpp>
#include <string>
#include <triqs/utility/dressed_iterator.hpp>
#include <triqs/utility/exceptions.hpp>
#include <triqs/utility/variant_extensions.hpp>
#include <triqs/utility/itertools.hpp>
#include <triqs/h5/base_public.hpp>
#include <vector>
#include <set>
#include <map>

namespace triqs::hilbert_space {

  /// The index type of an operator
  using indices_t = std::vector<std::variant<int, std::string>>;

  /// Type type describing the structure of a Block Green's function
  using gf_struct_t = std::vector<std::pair<std::string, indices_t>>;

  /// This class represents an ordered set of **indices** of the canonical operators (see [[many_body_operator]]) used to build the Fock states.
  /**
   * Every element of the set is an arbitrarily long sequence of integers/strings (types can be mixed within one sequence).
   * The elements maintain the order they are inserted in
   * @include triqs/hilbert_space/fundamental_operator_set.hpp
   */
  class fundamental_operator_set {
    public:
    /// Sequence of indices (`std::vector` of int/string variant objects)
    using indices_t = triqs::hilbert_space::indices_t;

    /// The basic container `std::vector<indices_t>`
    using data_t = std::vector<indices_t>;

    private:
    data_t vec = {};

    // internal only
    fundamental_operator_set(std::vector<std::vector<std::string>> const &);

    public:
    /// Construct an empty set
    fundamental_operator_set() {}

    /// Construct a set with each stored index being a pair of integers `(i,j)`
    /**
     * @param v `i` runs from 0 to `v.size()-1`; `j` runs from 0 to `v[i].size()-1` for each `i`
     */
    fundamental_operator_set(std::vector<int> const &v) {
      for (int i = 0; i < v.size(); ++i)
        for (int j = 0; j < v[i]; ++j) insert(i, j);
    }

    /// Construct from a set of generic index sequences
    /**
     * @param s Set of indices
     */
    template <typename IndexType> fundamental_operator_set(std::set<IndexType> const &s) {
      for (auto const &i : s) insert(i);
    }

    /// Construct from a vector of index sequences
    /**
     * @param v Vector of indices
     */
    explicit fundamental_operator_set(data_t const &v) : vec(v) {}

    /// Construct fundamental_operator_set on a GF structure
    /**
     * @param gf_struct GF structure object
     */
    fundamental_operator_set(gf_struct_t const &gf_struct) {
      for (auto const &block : gf_struct)
        for (auto const &inner : block.second) insert(block.first, inner);
    }

    /// Reduce to a `std::vector<indices_t>`
    explicit operator data_t() const { return vec; }

    /// Insert a new index sequence given as `indices_t`
    /**
     * @param ind `indices_t` object
     */
    void insert_from_indices_t(indices_t const &ind) {
      if (!has_indices(ind)) vec.push_back(ind);
    }

    /// Insert a new index sequence given as multiple `int`/`std::string` arguments
    template <typename... IndexType> void insert(IndexType const &... ind) { insert_from_indices_t(indices_t{ind...}); }

    /// Number of elements in this set
    /**
     * @return Size of the set
     */
    int size() const { return vec.size(); }

    /// Check if a given index sequence is in this set
    /**
     * @param t Index sequence to look up
     * @return `true` if `t` is in this set
     */
    bool has_indices(indices_t const &t) const { return std::find(vec.begin(), vec.end(), t) != vec.end(); }

    /// Request position of a given index sequence
    /**
     * @param t Index sequence to look up
     * @return Position of the requested index sequence
     */
    int operator[](indices_t const &t) const {
      auto it = std::find(vec.begin(), vec.end(), t);
      if (it == vec.end()) TRIQS_RUNTIME_ERROR << "Operator with indices (" << t << ") does not belong to this fundamental set!";
      return std::distance(vec.begin(), it);
    }

    /// Comparison with another fundamental operator set
    bool operator==(fundamental_operator_set const &fops) const { return vec == fops.vec; }

    /// Return the data vector: `v[int]` -> `indices_t`
    /**
     * @return The data vector
     */
    data_t const &data() const { return vec; }

    // Dereference type for const_iterator
    struct _cdress {
      indices_t const &index;
      int linear_index;
      using base_const_iterator = decltype(triqs::utility::enumerate(vec).cbegin());
      _cdress(base_const_iterator _it) : linear_index(std::get<0>(*_it)), index(std::get<1>(*_it)) {}
    };

    /// Constant bidirectional iterator over all stored index sequences. For an iterator `it`, `it->index` gives the `indices_t` object pointed by this iterator, and `it->linear_index` is its position in the set.
    using const_iterator = triqs::utility::dressed_iterator<_cdress::base_const_iterator, _cdress>;

    /// Return `const_iterator` to the first element of this set
    /**
     * @return Iterator to the first index sequence
     */
    const_iterator begin() const noexcept { return triqs::utility::enumerate(vec).begin(); }

    /// Return `const_iterator` to the past-the-end element of this set
    /**
     * @return Iterator to the past-the-end element
     */
    const_iterator end() const noexcept { return triqs::utility::enumerate(vec).end(); }

    /// Equivalent to [[fundamental_operator_set_begin]]
    /**
     * @return Iterator to the first index sequence
     */
    const_iterator cbegin() const noexcept { return triqs::utility::enumerate(vec).cbegin(); }

    /// Equivalent to [[fundamental_operator_set_end]]
    /**
     * @return Iterator to the past-the-end element
     */
    const_iterator cend() const noexcept { return triqs::utility::enumerate(vec).cend(); }

    /// Write this set as an HDF5 attribute
    /**
     * @param id ID of an HDF5 object to attach the attribute to
     * @param name Name of the attribute
     * @param f Fundamental set to write
     */
    friend void h5_write_attribute(hid_t id, std::string const &name, fundamental_operator_set const &f);

    /// Read a set from an HDF5 attribute
    /**
     * @param id ID of an HDF5 object the attribute is attached to
     * @param name Name of the attribute
     * @param f Reference to a fundamental set to be read
     */
    friend void h5_read_attribute(hid_t id, std::string const &name, fundamental_operator_set &f);
  };
} // namespace triqs::hilbert_space
