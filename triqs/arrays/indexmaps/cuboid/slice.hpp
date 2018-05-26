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
#include <triqs/utility/count_type_occurrence.hpp>
namespace triqs {
  namespace arrays {
    namespace indexmaps {
      namespace cuboid_details {
#define LISILOSO l_type const *li, s_type const *si, l_type *lo, s_type *so, int *imap
        using l_type = size_t;
        using s_type = std::ptrdiff_t;

        inline void _check_BC(int N, int ind, size_t B, int ind_min = 0) {
#ifdef TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
          if (!((ind >= ind_min) && (ind < int(B))))
            TRIQS_ARRAYS_KEY_ERROR << " index " << N << " is out of domain: \n    " << ind << " is not within [0," << B << "[\n";
#endif
        }

        struct slice_calc { // group in a struct to avoid declaration order issue with the cross call of templates...

          template <int N, int P> static void one_step(LISILOSO, std::ptrdiff_t &offset, size_t R) {
            _check_BC(N, R, li[N]);
            offset += R * si[N];
            imap[N] = -1;
          }

          template <int N, int P> static void one_step(LISILOSO, std::ptrdiff_t &offset, range R) {
            _check_BC(N, R.first(), li[N]);
            lo[P] = ((R.last() == -1 ? li[N] : R.last()) - R.first() + R.step() - 1) / R.step(); // python behaviour
            _check_BC(N, R.first() + (lo[P] != 0 ? (lo[P] - 1) : 0) * R.step(), li[N], -1);
            so[P] = si[N] * R.step();
            offset += R.first() * si[N];
            imap[N] = P;
          }

          // ellipsis is a total range, we can simplify the computation in that case...
          template <int N, int P> static void one_step(LISILOSO, std::ptrdiff_t &offset, ellipsis) {
            _check_BC(N, 0, li[N]);
            lo[P] = li[N];
            _check_BC(N, (lo[P] != 0 ? (lo[P] - 1) : 0), li[N], -1);
            so[P]   = si[N];
            imap[N] = P;
          }

          template <int N, int P, int EllipsisLength, typename Arg0, typename... Args>
          static std14::enable_if_t<((EllipsisLength == 1) || (!std::is_base_of<ellipsis, Arg0>::type::value)), void>
          invoke(LISILOSO, s_type &offset, Arg0 const &arg0, Args const &... args) {
            constexpr bool dP = (std::is_base_of<range, Arg0>::type::value ? 1 : 0); // Arg0 is range or ellipsis
            one_step<N, P>(li, si, lo, so, imap, offset, arg0);
            invoke<N + 1, P + dP, EllipsisLength>(li, si, lo, so, imap, offset, args...);
          }

          template <int N, int P, int EllipsisLength, typename Arg0, typename... Args>
          static std14::enable_if_t<((EllipsisLength == 0) && std::is_base_of<ellipsis, Arg0>::type::value), void>
          invoke(LISILOSO, s_type &offset, Arg0 const &arg0, Args const &... args) {
            invoke<N, P, EllipsisLength>(li, si, lo, so, imap, offset, args...);
          }

          template <int N, int P, int EllipsisLength, typename Arg0, typename... Args>
          static std14::enable_if_t<((EllipsisLength > 1) && std::is_base_of<ellipsis, Arg0>::type::value), void>
          invoke(LISILOSO, s_type &offset, Arg0 const &arg0, Args const &... args) {
            one_step<N, P>(li, si, lo, so, imap, offset, arg0);
            invoke<N + 1, P + 1, EllipsisLength - 1>(li, si, lo, so, imap, offset, arg0, args...);
          }

          template <int N, int P, int EllipsisLength> static void invoke(LISILOSO, s_type &offset) {}
        };
#undef LISILOSO
      } // namespace cuboid_details

      // special case of no argument :
      template <int R, typename To> struct slicer<cuboid::map<R, To>> { using r_type = cuboid::map<R, To>; };

      // general case
      template <int R, typename To, typename... Args> struct slicer<cuboid::map<R, To>, Args...> {

        static const unsigned int len      = sizeof...(Args);
        static constexpr bool has_ellipsis = (count_type_occurrence<ellipsis, Args...>::value > 0);
        static_assert((count_type_occurrence<ellipsis, Args...>::value < 2), "Only one ellipsis is permitted");
        static_assert((len >= R || has_ellipsis), "Too few arguments in slice");
        static_assert((len <= R + (has_ellipsis ? 1 : 0)), "Too many arguments in slice"); // + one to allow an empty ellipsis

        using im_t              = cuboid::map<R, To>;
        static constexpr int Rf = R - count_type_occurrence_not<range, Args...>::value;
        // TO DO : compute the new traversal order using the same routine, constexpr, if needed ?
        // Af the moment, using simply the C traversal order.
        using Tof    = void; //_traversal_c;
        using r_type = cuboid::map<Rf, Tof>;

        static r_type invoke(im_t const &X, Args... args) {
          typename r_type::lengths_type newlengths;
          typename r_type::strides_type newstrides;
          std::ptrdiff_t newstart      = X.start_shift();
          constexpr int EllipsisLength = R - len + 1;
          mini_vector<int, R> imap(utility::no_init_tag{});
          cuboid_details::slice_calc::invoke<0, 0, EllipsisLength>(&X.lengths()[0], &X.strides()[0], &newlengths[0], &newstrides[0], imap.ptr(),
                                                                   newstart, args...);

          if (X.memory_layout().is_c()) return r_type(std::move(newlengths), std::move(newstrides), newstart, memory_layout_t<Rf>{});
          if (X.memory_layout().is_fortran())
            return r_type(std::move(newlengths), std::move(newstrides), newstart, memory_layout_t<Rf>{FORTRAN_LAYOUT});

          // Compute the new memory index order
          mini_vector<int, Rf> p(utility::no_init_tag{});
          for (int i = 0, j = 0; j < R; ++j) {
            auto k = imap[X.memory_layout()[j]];
            if (k != -1) p[i++] = k;
          }

          return r_type(std::move(newlengths), std::move(newstrides), newstart, memory_layout_t<Rf>{p, bool()});
        };
      };
    } // namespace indexmaps
  }   // namespace arrays
} // namespace triqs
