/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2017 by O. Parcollet
 * Copyright (C) 2019 by Simons Foundation
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
#include "./first_include.hpp"
#include <iterator>

namespace triqs::utility {

  template <typename Iter, typename Value, typename Reference = Value &, typename Difference = std::ptrdiff_t,
            typename Tag = std::forward_iterator_tag>
  struct iterator_facade {

    private:
    Iter &self() { return static_cast<Iter &>(*this); }
    Iter const &self() const { return static_cast<const Iter &>(*this); }

    public:
    using value_type        = Value;
    using reference         = Reference;
    using pointer           = Value *;
    using difference_type   = Difference;
    using iterator_category = Tag;

    Iter &operator++() {
      self().increment();
      return self();
    }

    Iter operator++(int) {
      Iter c = *this;
      self().increment();
      return c;
    }

    template <typename U> bool operator==(U const &other) const { return self().equal(other); }
    template <typename U> bool operator!=(U const &other) const { return (!operator==(other)); }

    decltype(auto) operator*() { return self().dereference(); }
    decltype(auto) operator*() const { return self().dereference(); }
    decltype(auto) operator-> () { return operator*(); }
    decltype(auto) operator-> () const { return operator*(); }
  };

} // namespace triqs::utility
