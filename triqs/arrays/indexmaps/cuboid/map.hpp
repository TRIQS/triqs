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
#include "./domain.hpp"
#include "./mem_layout.hpp"
#include <vector>
#include <triqs/utility/itertools.hpp>

namespace triqs {
  namespace arrays {

    struct _traversal_c {};
    struct _traversal_fortran {};
    struct _traversal_dynamical {};
    template <int... Is> struct _traversal_custom {};

    template <typename T> struct _get_traversal_order_t { using type = T; };
    template <> struct _get_traversal_order_t<void> { using type = _traversal_c; };

    constexpr ull_t _get_traversal_order_permutation(int R, _traversal_c) { return permutations::identity(R); }
    constexpr ull_t _get_traversal_order_permutation(int R, _traversal_fortran) { return permutations::ridentity(R); }
    template <int... Is> constexpr ull_t _get_traversal_order_permutation(int R, _traversal_custom<Is...>) {
      static_assert(sizeof...(Is) == R, " Rank mismatch");
      return permutations::permutation(Is...);
    }

    namespace indexmaps {
      namespace cuboid {

        /** Standard hyper_rectangular arrays, implementing the IndexMap concept.
 */
        template <int Rank, typename TraversalOrder = void> class map {
          public:
          static const int rank             = Rank;
          using lengths_type                = mini_vector<size_t, rank>;
          using strides_type                = mini_vector<std::ptrdiff_t, rank>;
          using domain_type                 = domain_t<Rank>;
          using traversal_order_in_template = TraversalOrder;
          using has_traversal_order_tag     = void;
          domain_type const &domain() const { return mydomain; }

          // semi-regular type
          map() : start_shift_(0), memory_order_() {}
          map(map const &C) = default;
          map(map &&C)      = default;
          map &operator=(map const &m) = default;
          map &operator=(map &&m) = default;

          // basic construction
          map(memory_layout_t<Rank> const &ml) : mydomain(), start_shift_(0), memory_order_(ml) {}
          map(domain_type const &C) : mydomain(C), start_shift_(0), memory_order_() { compute_stride_compact(); }
          map(domain_type const &C, memory_layout_t<Rank> ml) : mydomain(C), start_shift_(0), memory_order_(std::move(ml)) {
            compute_stride_compact();
          }

          /// Construction from the length, the stride, start_shift
          map(lengths_type Lengths, strides_type strides, std::ptrdiff_t start_shift)
             : mydomain(std::move(Lengths)),
               strides_(std::move(strides)),
               start_shift_(start_shift),
               memory_order_(memory_layout_from_strides(strides_)) {}

          /// Construction from the length, the stride, start_shift, ml
          map(lengths_type Lengths, strides_type strides, std::ptrdiff_t start_shift, memory_layout_t<Rank> const &ml)
             : mydomain(std::move(Lengths)), strides_(std::move(strides)), start_shift_(start_shift), memory_order_(ml) {}

          /// Construction from another map with the same order
          template <typename To2>
          map(map<Rank, To2> const &C)
             : mydomain(C.domain()), strides_(C.strides()), start_shift_(C.start_shift()), memory_order_(C.memory_layout()) {}

          template <typename To2> map &operator=(map<Rank, To2> const &m) { *this = map{m}; }

          // transposition
          friend map transpose(map const &m, mini_vector<int, Rank> const &perm) {
            lengths_type l;
            strides_type s;
            for (int u = 0; u < Rank; ++u) {
              l[perm[u]] = m.domain().lengths()[u];
              s[perm[u]] = m.strides_[u];
            }
            return map{l, s, m.start_shift_, transpose(m.memory_order_, perm)};
          }

          /// Returns the shift in position of the element key.
          template <typename KeyType> size_t operator[](KeyType const &key) const {
#ifdef TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
            this->domain().assert_key_in_domain(key);
#endif
            return start_shift_ + dot_product(key, this->strides());
          }

          friend std::ostream &operator<<(std::ostream &out, const map &P) {
            return out << "  ordering = {" << P.memory_layout() << "}" << std::endl
                       << "  Lengths  :  " << P.lengths() << std::endl
                       << "  Stride  : " << P.strides_ << std::endl;
          }

          /// TODO: replace by a tuple call....
          template <typename... Args> size_t operator()(Args const &... args) const {
#ifdef TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
            this->domain().assert_key_in_domain_v(args...);
#endif
            return start_shift_ + _call_impl<0>(args...);
          }

          private:
          template <int N, typename Arg0, typename... Args> size_t _call_impl(Arg0 const &arg0, Args const &... args) const {
            return arg0 * strides_[N] + _call_impl<N + 1>(args...);
          }
          template <int N> size_t _call_impl() const { return 0; }

          public:
          ///
          bool is_contiguous() const {
            int slowest_index = memory_order_[0];
            return (strides()[slowest_index] * this->lengths()[slowest_index] == mydomain.number_of_elements());
          }

          size_t start_shift() const { return start_shift_; }
          lengths_type const &lengths() const { return mydomain.lengths(); }
          strides_type const &strides() const { return this->strides_; }
          long size() const { return mydomain.number_of_elements(); }

          memory_layout_t<Rank> const &memory_layout() const { return memory_order_; }
          bool memory_layout_is_c() const { return memory_layout().is_c(); }
          bool memory_layout_is_fortran() const { return memory_layout().is_fortran(); }

          private:
          domain_type mydomain;
          strides_type strides_;
          std::ptrdiff_t start_shift_;
          memory_layout_t<Rank> memory_order_;

          //  BOOST Serialization
          friend class boost::serialization::access;
          template <class Archive> void serialize(Archive &ar, const unsigned int version) {
            ar &TRIQS_MAKE_NVP("domain", mydomain);
            ar &TRIQS_MAKE_NVP("strides", strides_);
            ar &TRIQS_MAKE_NVP("start_shift", start_shift_);
          }
          // for construction
          // TODO : use tupletools
          void compute_stride_compact() {
            size_t str = 1;
            csc_impl(memory_order_, str, std::integral_constant<int, rank>());
            assert(this->domain().number_of_elements() == str);
          }
          // call for indices fastest (rank -1) to slowest (0)
          template <int v> void csc_impl(memory_layout_t<Rank> const &ml, size_t &str, std::integral_constant<int, v>) {
            // size_t u = mem_layout::memory_rank_to_index(order, rank-v);
            int u             = ml[v - 1];
            this->strides_[u] = str;
            str *= this->lengths()[u];
            csc_impl(ml, str, std::integral_constant<int, v - 1>());
          }
          void csc_impl(memory_layout_t<Rank> const &, size_t &, std::integral_constant<int, 0>) {}

          // iterator helper impl.
          static constexpr int __iter_get_p(int p, map const *im, _traversal_c) { return p; }
          static constexpr int __iter_get_p(int p, map const *im, _traversal_fortran) { return Rank - p - 1; }
          static int __iter_get_p(int p, map const *im, _traversal_dynamical) { return im->memory_layout()[p]; }
          template <int... Is> static constexpr int __iter_get_p(int p, map const *im, _traversal_custom<Is...>) {
            return permutations::apply(_get_traversal_order_permutation(Rank, _traversal_custom<Is...>{}), p);
          }

          public:
          /**
    * Iterator on a cuboid_map, modeling the IndexMapIterator concept.
    * Iteration order is the order in which to iterate on the indices.
    * It is given by a permutation, with the same convention as IndexOrder.
    */
          class iterator : public triqs::utility::iterator_facade<iterator, const std::ptrdiff_t, std::forward_iterator_tag> {
            public:
            using indexmap_type = map;
            using indices_type  = typename domain_type::index_value_type;
            using return_type   = const std::ptrdiff_t;
            iterator() : im(NULL), pos(0), atend(true) {}
            iterator(const map &P, bool atEnd = false, ull_t iteration_order = 0) : im(&P), pos(im->start_shift()), atend(P.size()==0 or atEnd) {}
            indices_type const &indices() const { return indices_tuple; }
            operator bool() const { return !atend; }

            private:
            friend class triqs::utility::iterator_facade<iterator, const std::ptrdiff_t, std::forward_iterator_tag>;
            void increment() { inc_ind_impl(std::integral_constant<int, Rank>()); }
            template <int v> inline void inc_ind_impl(std::integral_constant<int, v>) {
              int p = __iter_get_p(v - 1, im, typename _get_traversal_order_t<TraversalOrder>::type{});
#ifdef TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
              if (atend) TRIQS_RUNTIME_ERROR << "Iterator in cuboid cannot be pushed after end !";
#endif
              if (indices_tuple[p] < im->lengths()[p] - 1) {
                ++(indices_tuple[p]);
                pos += im->strides()[p];
                return;
              }
              indices_tuple[p] = 0;
              pos -= (im->lengths()[p] - 1) * im->strides()[p];
              inc_ind_impl(std::integral_constant<int, v - 1>());
            }
            inline void inc_ind_impl(std::integral_constant<int, 0>) { atend = true; }
            bool equal(iterator const &other) const { return ((other.im == im) && (other.atend == atend) && (other.pos == pos)); }
            return_type &dereference() const {
              assert(!atend);
              return pos;
            }
            map const *im;
            indices_type indices_tuple;
            std::ptrdiff_t pos;
            bool atend;
          };

        }; //------------- end class ---------------------

      } //namespace cuboid

      template <int R1, int R2, typename To1, typename To2>
      bool compatible_for_assignment(const cuboid::map<R1, To1> &X1, const cuboid::map<R2, To2> &X2) {
        return X1.lengths() == X2.lengths();
      }

      template <int R1, int R2, typename To1, typename To2> bool raw_copy_possible(const cuboid::map<R1, To1> &X1, const cuboid::map<R2, To2> &X2) {
        return ((X1.memory_layout() == X2.memory_layout()) && X1.is_contiguous() && X2.is_contiguous()
                && (X1.domain().number_of_elements() == X2.domain().number_of_elements()));
      }
    } // namespace indexmaps
  }   // namespace arrays
} // namespace triqs
