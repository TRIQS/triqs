
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
#pragma once
#include <iterator>

namespace triqs {
  namespace arrays {
    /**
  * Iterator on a couple (IndexMap, StorageType).
  * It transforms the mini-iterator on the IndexMap into a STL-compliant iterator.
  * Template parameters are :
  *    - Const : true -> const_iterator, false -> non const version.
  *    - IndexMapIterator : an IndexMapIterator
  *    - StorageType : the storage.
  */

    template <bool Const, typename IndexMapIterator, typename StorageType>
    class iterator_adapter
       : public std::iterator<std::forward_iterator_tag,
                              std::conditional_t<Const, const typename StorageType::value_type, typename StorageType::value_type>> {

      StorageType storage_;
      IndexMapIterator it;

      public:
      using value_type             = std::conditional_t<Const, const typename StorageType::value_type, typename StorageType::value_type>;
      using indexmap_iterator_type = IndexMapIterator;

      iterator_adapter()                         = default;
      iterator_adapter(iterator_adapter const &) = default;
      iterator_adapter(const typename IndexMapIterator::indexmap_type &Ind, const StorageType &ST, bool atEnd = false)
         : storage_(ST), it(Ind, atEnd) {}

      value_type &operator*() const { return storage_[*it]; }
      value_type &operator->() const { return storage_[*it]; }

      iterator_adapter &operator++() {
        ++it;
        return *this;
      }

      iterator_adapter operator++(int) {
        auto c = *this;
        ++it;
        return c;
      }

      bool operator==(iterator_adapter const &other) const { return (other.it == it); }
      bool operator!=(iterator_adapter const &other) const { return (!operator==(other)); }

      // not in forward iterator concept
      operator bool() const { return bool(it); }

      decltype(auto) indices() const { return it.indices(); }
      IndexMapIterator const &indexmap_iterator() const { return it; }
    };
  } // namespace arrays
} // namespace triqs
