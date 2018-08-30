/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2014 by O. Parcollet
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
#include "../common.hpp"
#include "../range.hpp"
#include "./mem_layout.hpp"
#include "../permutation.hpp"
#include <triqs/utility/mini_vector.hpp>
#include "../../impl/exceptions.hpp"
#include <iostream>
#include <sstream>

namespace triqs {
  namespace arrays {
    namespace indexmaps {
      namespace cuboid {
        using namespace triqs::arrays::permutations;

        /// Standard hyper_rectangular domain for arrays
        template <int Rank> class domain_t {
          using n_uple = mini_vector<size_t, Rank>;
          n_uple lengths_;
          friend class boost::serialization::access;
          template <class Archive> void serialize(Archive &ar, const unsigned int version) { ar &TRIQS_MAKE_NVP("dimensions", lengths_); }

          public:
          static constexpr unsigned int rank = Rank;
          using index_value_type             = n_uple;

          domain_t()                  = default;
          domain_t(const domain_t &C) = default;
          domain_t(domain_t &&C)      = default;
          domain_t &operator=(domain_t const &) = default;
          domain_t &operator=(domain_t &&x) = default;

          domain_t(n_uple lengths) : lengths_(std::move(lengths)) {}
          domain_t(mini_vector<int, Rank> const &lengths) : lengths_(lengths) {}
          domain_t(std::vector<std::size_t> const &l) : lengths_() {
            if (!(l.size() == Rank))
              TRIQS_RUNTIME_ERROR << "cuboid domain_t construction : vector size incorrect : got " << l.size() << " while expected " << Rank;
            lengths_ = n_uple(l);
          }
          template <typename... T> explicit domain_t(size_t i0, T... t) : lengths_(i0, t...) {}
          //template <typename... T> domain_t(size_t i0, size_t i1, T... t) : lengths_(i0, i1, t...) {}
          //template <int R= Rank> explicit domain_t(std::enable_if_t<R==1, size_t> i0) : lengths_(i0) {}

          size_t number_of_elements() const { return lengths_.product_of_elements(); }
          bool operator==(domain_t const &X) const { return this->lengths_ == X.lengths_; }
          bool operator!=(domain_t const &X) const { return !(*this == X); }
          n_uple const &lengths() const & { return lengths_; }
          n_uple lengths() && { return lengths_; }
          /** Generates the value of the indices of a cuboid_domain.  */
          static constexpr ull_t iteration_order_default = permutations::identity(Rank);
          template <ull_t IterationOrder = iteration_order_default> class gal_generator {
            using indices_type = index_value_type;
            const domain_t *dom;
            indices_type indices_tuple;
            bool atend;

            public:
            gal_generator(const domain_t &P, bool atEnd = false) : dom(&P), atend(P.number_of_elements()==0 or atEnd) {}
            bool operator==(const gal_generator &IT2) const {
              assert((IT2.dom == dom));
              return ((IT2.atend == atend));
            }
            bool operator!=(const gal_generator &IT2) const { return (!operator==(IT2)); }
            indices_type const &operator*() const { return indices_tuple; }
            explicit operator bool() const { return !atend; }
            gal_generator &operator++() {
              assert(!atend);
              atend = advance_impl(std::integral_constant<int, 0>());
              return *this;
            }

            private:
            template <int r> bool advance_impl(std::integral_constant<int, r>) {
              constexpr int p = permutations::apply(IterationOrder, r);
              if (indices_tuple[p] < dom->lengths()[p] - 1) {
                ++(indices_tuple[p]);
                return false;
              }
              indices_tuple[p] = 0;
              return advance_impl(std::integral_constant<int, r + 1>());
            }
            bool advance_impl(std::integral_constant<int, rank>) { return true; }
          };

          using generator = gal_generator<>;

          generator begin() const { return generator(*this, false); }
          generator end() const { return generator(*this, true); }
          /* End of generator */

          // Check that key in in the domain
          template <class KeyType> void assert_key_in_domain(KeyType const &key) const {
            std::stringstream fs;
            bool res = key_check_impl(std::integral_constant<int, 0>(), key, this->lengths_, fs);
            if (!res) TRIQS_ARRAYS_KEY_ERROR << " key out of domain \n" << fs.str();
          }
          template <int r, class KeyType>
          bool key_check_impl(std::integral_constant<int, r>, KeyType const &key, n_uple const &L, std::stringstream &fs) const {
            bool cond = ((size_t(std::get<r>(key)) < L[r]));
            if (!cond) fs << "key [" << r << "] = " << std::get<r>(key) << " is not within [0," << L[r] << "[\n";
            return key_check_impl(std::integral_constant<int, r + 1>(), key, L, fs) && cond;
          }
          template <class KeyType>
          bool key_check_impl(std::integral_constant<int, rank>, KeyType const &, n_uple const &, std::stringstream &) const {
            return true;
          }

          // Check that key in in the domain : variadic form. No need for speed optimisation here, it is just for debug
          template <typename... Args> void assert_key_in_domain_v(Args const &... args) const { assert_key_in_domain(std::make_tuple(args...)); }

          friend std::ostream &operator<<(std::ostream &out, domain_t const &x) {
            return out << "Cuboid of rank " << x.rank << " and dimensions " << x.lengths();
          }
        };
      } // namespace cuboid
    }   // namespace indexmaps

    template <typename... U> indexmaps::cuboid::domain_t<sizeof...(U)> make_cuboid_domain(U... u) { return {size_t(u)...}; }

    using matrix_shape_t                    = indexmaps::cuboid::domain_t<2>;
    using vector_shape_t                    = indexmaps::cuboid::domain_t<1>;
    template <int Rank> using array_shape_t = indexmaps::cuboid::domain_t<Rank>;
  } // namespace arrays
} // namespace triqs
