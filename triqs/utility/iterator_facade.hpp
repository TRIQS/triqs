/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2017 by O. Parcollet
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
#include "./first_include.hpp"
#include <iterator>

namespace triqs {
 namespace utility {

  template <class Iter, class Value, class Tag, class Reference = Value&, class Difference = std::ptrdiff_t>
  class iterator_facade;

  // need only specialization for the moment.
  template <class Iter, class Value, class Reference, class Difference>
  struct iterator_facade<Iter, Value, std::forward_iterator_tag, Reference, Difference> {

   Iter& self() { return static_cast<Iter&>(*this); }
   Iter const& self() const { return static_cast<const Iter&>(*this); }

   public:
   using value_type        = Value; // decltype(*(std::declval<Iterator_facade>()));
   using Iterator_category = std::forward_iterator_tag;
   using pointer           = value_type*;
   using difference_type   = Difference;
   using reference         = Reference;

   Iter& operator++() {
    self().increment();
    return self();
   }

   Iter operator++(int) {
    Iter c = *this;
    self().increment();
    return self();
   }

   bool operator==(Iter const& other) const { return self().equal(other); }
   bool operator!=(Iter const& other) const { return (!operator==(other)); }

   decltype(auto) operator*() const { return self().dereference(); }
   decltype(auto) operator-> () const { return operator*(); }
  };
 }
}
