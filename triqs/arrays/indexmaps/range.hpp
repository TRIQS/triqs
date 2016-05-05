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

 ///A range of indices
 /**
  `range` mimics the Python `range`.
  */
 class range {
  std::ptrdiff_t first_ = 0, last_ = -1, step_ = 1;

  public:
  using index_type = std::ptrdiff_t;

  ///default constructor
  /**
  * range() stands for the whole set of indices in the dimension (like `:` in python) ::    
    
     A(range(), 0) // take the first column of A
  */
  range() = default;

  ///constructor
  /**

   - two arguments to specify a range ::

     A(range (0,3), 0)  // means  A(0,0), A(1,0), A(2,0)

   -  three arguments : a range with a step ::

      A(range(0,4,2), 0) // means A(0,0), A(2,0)  

    @warning the second element is excluded: range(0,3) is 0,1,2, like in Python.
  */
  range(std::ptrdiff_t first__, std::ptrdiff_t last__, std::ptrdiff_t step__ = 1)
     : first_(first__), last_(last__), step_(step__) {}

  ///constructor
  /**

   - one argument to specify a range ::

     range (3) // is equivalent to range(0,3,1) 
  */
  explicit range(std::ptrdiff_t i) : range(0, i, 1) {}

  ///first index of the range
  std::ptrdiff_t first() const { return first_; }
  ///last index of the range (is excluded from the list of indices)
  std::ptrdiff_t last() const { return last_; }
  ///step between two indices
  std::ptrdiff_t step() const { return step_; }
  ///number of indices in the range
  size_t size() const {
   std::ptrdiff_t r = (last_ - first_) / step_;
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
    step = r->step();
    pos = (atEnd ? last : r->first());
   }
   private:
   std::ptrdiff_t last, pos, step;
   friend class boost::iterator_core_access;
   void increment() { pos+= step; }
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

  ///Ellipsis
 /**
   Ellipsis can be provided in place of [[range]], as in python. The type `ellipsis` is similar to [[range]]
  except that it is implicitly repeated to as much as necessary.
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
