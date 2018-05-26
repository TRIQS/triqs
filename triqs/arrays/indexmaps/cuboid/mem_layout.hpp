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
namespace triqs {
  namespace arrays {

    struct __mem_layout_fortran_order {};
    struct __mem_layout_c_order {};
#define C_LAYOUT (triqs::arrays::__mem_layout_c_order())
#define FORTRAN_LAYOUT (triqs::arrays::__mem_layout_fortran_order())

    using utility::mini_vector;

    enum class memory_order_type { C, Fortran, Custom };

    /* The storage order is given by a permutation P
  *   P[0] : the slowest index,
  *   P[Rank-1] : the fastest index
  *   Example :
  *   210 : Fortran, the first index is the fastest
  *   012 : C the last index is the fastest
  *   120 : storage (i,j,k) is : index j is slowest, then k, then i
  */
    template <int Rank> class memory_layout_t {

      memory_order_type order  = memory_order_type::C;
      mini_vector<int, Rank> p = utility::no_init_tag{};

      public:
      static const int rank = Rank;

      explicit memory_layout_t() {
        for (int u = 0; u < Rank; ++u) p[u] = u;
      }

      memory_layout_t(__mem_layout_fortran_order) {
        order = memory_order_type::Fortran;
        for (int u = 0; u < Rank; ++u) p[u] = Rank - u - 1;
      }

      // Does not compile on gcc 4.8, probably a bug ?
      //memory_layout_t(__mem_layout_c_order) : memory_layout_t() {}
      memory_layout_t(__mem_layout_c_order) {
        for (int u = 0; u < Rank; ++u) p[u] = u;
      }

      // internal use only : no check , we KNOW the order is not C or Fortran
      template <typename T> explicit memory_layout_t(mini_vector<T, Rank> v, bool) : p(std::move(v)) { order = memory_order_type::Custom; }

      // For the user : we find out whether it is C or Fortran
      template <typename T> explicit memory_layout_t(mini_vector<T, Rank> v) : p(std::move(v)) {
        bool c = true, f = true;
        for (int u = 0; u < Rank; ++u) {
          c = c && (v[u] == u);
          f = f && (v[u] == Rank - u - 1);
        }
        if (c) return;
        order = (f ? memory_order_type::Fortran : memory_order_type::Custom);
      }

      template <typename... INT> explicit memory_layout_t(int i0, INT... in) : memory_layout_t(mini_vector<int, sizeof...(INT) + 1>{i0, in...}) {}

      bool operator==(memory_layout_t const &ml) const { return p == ml.p; }
      bool operator!=(memory_layout_t const &ml) const { return !operator==(ml); }

      int operator[](int u) const { return p[u]; }

      bool is_c() const { return order == memory_order_type::C; }
      bool is_fortran() const { return order == memory_order_type::Fortran; }

      mini_vector<int, Rank> get_memory_positions() const {
        auto r = mini_vector<int, Rank>(utility::no_init_tag{});
        for (int u = 0; u < Rank; ++u) r[p[u]] = u;
        return r;
      }

      mini_vector<int, Rank> get_indices_in_order() const { return p; }

      friend std::ostream &operator<<(std::ostream &out, memory_layout_t const &s) { return out << s.p; }
    };

    // ------------------------- functions -------------------------------------

    /// Make a memory_layout_t from the indices
    template <typename... T> memory_layout_t<sizeof...(T)> make_memory_layout(T... x) { return memory_layout_t<sizeof...(T)>(x...); }

    /// Make a memory_layout_t from the strides
    template <int Rank> memory_layout_t<Rank> memory_layout_from_strides(mini_vector<std::ptrdiff_t, Rank> const &strides) {
      mini_vector<int, Rank> c; // rely on init to 0 by default of mini_vector

      for (int i = 0; i < Rank; ++i)
        for (int j = i + 1; j < Rank; ++j)
          if (strides[i] < strides[j])
            c[i]++;
          else
            c[j]++;
      // we computed the map : index -> memory_rank, which is the inverse map, cf mem_layout

      //std::cout << " memory_layout_from_strides\n"<< "    strides = "<< strides << "\n    memlayout"<< c <<std::endl;
      return memory_layout_t<Rank>{c};
    }

    /// Apply a permutation to the indices
    template <int R> memory_layout_t<R> transpose(memory_layout_t<R> const &ml, mini_vector<int, R> const &perm) {
      mini_vector<int, R> res;
      for (int u = 0; u < R; ++u) res[u] = perm[ml[u]];
      return memory_layout_t<R>{res};
    }
  } // namespace arrays
} // namespace triqs
