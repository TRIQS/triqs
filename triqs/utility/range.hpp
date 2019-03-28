/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2018 O. Parcollet
 * Copyright (C) 2019 The Simons Foundation
 *   author : N. Wentzell
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
#include "./itertools.hpp"

namespace triqs::utility {

  /**
   * A range of integer indices that mimics the Python `range`.
   */
  class range {

    public:
    using index_t = std::ptrdiff_t;

    /**
     * Default constructor
     *
     * range() stands for the whole set of indices in the dimension (like `:` in python)
     * Example: A(range(), 0) takes the first column of A
     */
    range() = default;

    /**
     * Constructor
     *
     * @param first__
     * The first index of the range
     *
     * @param last__
     * The first index past the end of the range
     *
     * @param step__
     * The step between two indices
     */
    range(index_t first__, index_t last__, index_t step__ = 1) : first_(first__), last_(last__), step_(step__) {}

    /**
     * Constructor
     *
     * Constructs range(0, i, 1)
     */
    explicit range(index_t i) : range(0, i, 1) {}

    /// The first index of the range
    index_t first() const { return first_; }

    /// The first index past the end of the range
    index_t last() const { return last_; }

    /// The step between two indices
    index_t step() const { return step_; }

    /// Number of indices in the range
    size_t size() const {
      index_t r = (last_ - first_) / step_;
      if (r < 0) throw std::runtime_error("range with negative size");
      return size_t(r);
    }

    range operator+(index_t shift) const { return range(first_ + shift, last_ + shift, step_); }

    friend inline std::ostream &operator<<(std::ostream &os, const range &range) {
      os << "range(" << range.first() << "," << range.last() << "," << range.step() << ")";
      return os;
    }

    class const_iterator : public iterator_facade<const_iterator, const index_t, std::forward_iterator_tag, const index_t> {
      public:
      const_iterator(range const *r, bool atEnd) {
        last = r->last();
        step = r->step();
        pos  = (atEnd ? last : r->first());
      }

      private:
      index_t last, pos, step;
      friend class triqs::utility::iterator_facade<const_iterator, const index_t, std::forward_iterator_tag, const index_t>;
      void increment() { pos += step; }
      bool equal(const_iterator const &other) const { return (other.pos == pos); }
      index_t dereference() const { return pos; }
    };

    const_iterator begin() const { return const_iterator(this, false); }
    const_iterator end() const { return const_iterator(this, true); }
    const_iterator cbegin() const { return const_iterator(this, false); }
    const_iterator cend() const { return const_iterator(this, true); }

    private:
    index_t first_ = 0, last_ = -1, step_ = 1;
  };

  /**
   * Ellipsis
   *
   * Ellipsis can be provided in place of [[range]], as in python.
   * The type `ellipsis` is similar to [[range]] except that it is implicitly repeated as much as necessary.
   */
  class ellipsis : public range {
    public:
    ellipsis(std::ptrdiff_t first__, std::ptrdiff_t last__, std::ptrdiff_t step__ = 1) : range(first__, last__, step__) {}
    ellipsis() : range() {}
  };
  // for the case A(i, ellipsis) where A is of dim 1...
  inline int operator*(ellipsis, int) { return 0; }
  inline int operator*(int, ellipsis) { return 0; }

  /**
   * A product of an arbitrary number of integer ranges
   * given a set of integers or an integer tuple
   *
   * @tparam Integers The integer types
   */
  template <typename... Integers, typename EnableIf = std::enable_if_t<(std::is_integral_v<Integers> and ...), int>>
  auto product_range(Integers... Is) {
    return product(range(Is)...);
  }

  namespace details {
    template <typename Tuple, size_t... Is> auto product_range_impl(Tuple const &idx_tpl, std::index_sequence<Is...>) {
      return product_range(std::get<Is>(idx_tpl)...);
    }
  } // namespace details

  template <typename... Integers, typename EnableIf = std::enable_if_t<(std::is_integral_v<Integers> and ...), int>>
  auto product_range(std::tuple<Integers...> const &idx_tpl) {
    return details::product_range_impl(idx_tpl, std::make_index_sequence<sizeof...(Integers)>{});
  }

  /**
   * Apply a function f to every element of an integer range
   *
   * @param r
   * The range to apply the function to
   *
   * @param f
   * The function to apply
   */
  template <typename F> void foreach (range const &r, F const &f) {
    std::ptrdiff_t i = r.first(), last = r.last(), step = r.step();
    for (; i < last; i += step) f(i);
  }
} // namespace triqs::utility
