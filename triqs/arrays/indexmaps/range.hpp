/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
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
#ifndef TRIQS_ARRAYS_RANGE_H
#define TRIQS_ARRAYS_RANGE_H
#include <triqs/utility/first_include.hpp>
#include <ostream>
#include <boost/iterator/iterator_facade.hpp>

namespace triqs {
namespace arrays {

 /**
  */
 class range {
  std::ptrdiff_t first_, last_, step_;

  public:
  typedef std::ptrdiff_t index_type;

  range() : first_(0), last_(-1), step_(1) {} // i.e. all
  range(const range& r) : first_(r.first_), last_(r.last_), step_(r.step_) {}
  range(std::ptrdiff_t first__, std::ptrdiff_t last__, std::ptrdiff_t step__ = 1)
     : first_(first__), last_(last__), step_(step__) {}

  std::ptrdiff_t first() const { return first_; }
  std::ptrdiff_t last() const { return last_; }
  std::ptrdiff_t step() const { return step_; }
  size_t size() const {
   std::ptrdiff_t r = (last_ - first_ + 1) / step_;
   if (r < 0) TRIQS_RUNTIME_ERROR << " range with negative size";
   return size_t(r);
  }

  range operator+(std::ptrdiff_t shift) const { return range(first_ + shift, last_ + shift, step_); }

  friend inline std::ostream& operator<<(std::ostream& os, const range& range) {
   os << "range(" << range.first() << "," << range.last() << "," << range.step() << ")";
   return os;
  }

  class const_iterator : public boost::iterator_facade<const_iterator, const std::ptrdiff_t, boost::forward_traversal_tag, const std::ptrdiff_t> {
   public:
   const_iterator(range const* r, bool atEnd) {
    last = r->last();
    pos = (atEnd ? last : r->first());
   }
   private:
   std::ptrdiff_t last, pos;
   friend class boost::iterator_core_access;
   void increment() { ++pos; }
   bool equal(const_iterator const& other) const { return (other.pos == pos); }
   std::ptrdiff_t dereference() const { return pos; }
  };

  const_iterator begin() const { return const_iterator(this, false); }
  const_iterator end() const { return const_iterator(this, true); }
  const_iterator cbegin() const { return const_iterator(this,false); }
  const_iterator cend() const { return const_iterator(this, true); }
 };

 // foreach
  template <typename F> void foreach(range const& r, F const& f) {
   std::ptrdiff_t i = r.first(), last = r.last(), step = r.step();
   for (; i < last; i += step) f(i);
  }

 /**
  */
 class ellipsis : public range {
  public:
  ellipsis(std::ptrdiff_t first__, std::ptrdiff_t last__, std::ptrdiff_t step__ = 1) : range(first__, last__, step__) {}
  ellipsis() : range() {}
 };

 // for the case A(i, ellipsis) where A is of dim 1...
 inline int operator*(ellipsis, int) { return 0; }
 inline int operator*(int, ellipsis) { return 0; }
}
} // namespace triqs::arrays
#endif
