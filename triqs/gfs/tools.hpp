/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
#include <triqs/arrays.hpp>
#include "triqs/utility/complex_ops.hpp"
#include <triqs/utility/view_tools.hpp>
#include <triqs/utility/expression_template_tools.hpp>
#include <utility>
#include <boost/iterator/iterator_facade.hpp>

namespace triqs {
namespace gfs {
 namespace mpl = boost::mpl;

 namespace tag {
  struct composite {};
  struct mesh_point {};
 }

 // scalar_valued, matrix_valued, tensor_valued
 struct scalar_valued {};

 template <int R> struct tensor_valued {
  static_assert(R > 0, "tensor_valued only for rank >0");
 };

 struct matrix_valued {};

 //------------------------------------------------------

 using dcomplex = std::complex<double>;

 /** The statistics : Boson or Fermion
  */
 enum statistic_enum {
  Boson,
  Fermion
 };

 struct freq_infty {}; // the point at infinity

 //------------------------------------------------------

 template <typename... T> struct closest_pt_wrap;

 template <typename T> struct closest_pt_wrap<T> : tag::mesh_point {
  T value;
  template <typename U> explicit closest_pt_wrap(U &&x) : value(std::forward<U>(x)) {}
 };

 template <typename T1, typename T2, typename... Ts> struct closest_pt_wrap<T1, T2, Ts...> : tag::mesh_point {
  std::tuple<T1, T2, Ts...> value_tuple;
  template <typename... U> explicit closest_pt_wrap(U &&... x) : value_tuple(std::forward<U>(x)...) {}
 };

 template <typename... T> closest_pt_wrap<T...> closest_mesh_pt(T &&... x) {
  return closest_pt_wrap<T...>{std::forward<T>(x)...};
 }
 //------------------------------------------------------

 // A simple indice struct
 // Replace empty state by optional ?
 struct indices_2 {

  std::vector<std::vector<std::string>> ind;

  indices_2() = default;

  indices_2(std::vector<std::vector<std::string>> _ind) : ind(std::move(_ind)) {}

  // from a size
  indices_2(int L) : indices_2(arrays::mini_vector<int, 2>{L, L}) {};

  // from a shape
  indices_2(arrays::mini_vector<int, 2> const & shape) : ind(2) {
   for (int i = 0; i < shape[0]; ++i) ind[0].push_back(std::to_string(i));
   for (int i = 0; i < shape[1]; ++i) ind[1].push_back(std::to_string(i));
  }

  // indices from a vector<T> : L and R are the same.
  template <typename T> indices_2(std::vector<T> const & _ind) : ind(2) {
   for (auto const &i : _ind) ind[0].push_back(std::to_string(i));
   ind[1] = ind[0];
  }

  bool is_empty() const { return ind.size() == 0; }

  void resize(int s) { ind.resize(s);}

  template <typename G> bool check_size(G *g) const {
   return (is_empty() ||
           ((ind.size() == 2) && (ind[0].size() == get_target_shape(*g)[0]) && (ind[1].size() == get_target_shape(*g)[1])));
  }

  std::vector<std::string> operator[](int i) const {
   if (is_empty())
    return std::vector<std::string> {};
   else
    return ind[i];
  }

  arrays::range convert_index(std::string const &s, int l_r) const {
   if (!is_empty()) {
    auto b = ind[l_r].begin(), e = ind[l_r].end();
    auto it = std::find(b, e, s);
    if (it != e) return it - b;
   }
   TRIQS_RUNTIME_ERROR << "There are no string indices for this Green function";
  }

  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) { ar &TRIQS_MAKE_NVP("ind", ind); }

  friend void h5_write(h5::group fg, std::string subgroup_name, indices_2 const &g) {
   if (g.is_empty()) return;
   auto gr = fg.create_group(subgroup_name);
   h5_write(gr, "left", g.ind[0]);
   h5_write(gr, "right", g.ind[1]);
  }

  friend void h5_read(h5::group fg, std::string subgroup_name, indices_2 &g) {
   h5::group gr = fg; // no default construction
   try {
    gr = fg.open_group(subgroup_name);
   }
   catch (...) {
    g = indices_2{}; // empty, no file
    return;
   }
   g.resize(2);
   h5_read(gr, "left", g.ind[0]);
   h5_read(gr, "right", g.ind[1]);
  }
 };

 // inline indices_2 slice(indices_2 const & ind, arrays::range rl, arrays::range rr) {
 // return {};
 // }

 inline indices_2 slice(indices_2 const &ind, arrays::range rl, arrays::range rr) {
  if (ind.is_empty()) return {};
  return {}; // MODIFY : slice the indices !!!
  TRIQS_RUNTIME_ERROR << "Not implemented : slice of string indices";
 }

 inline indices_2 transpose(indices_2 const &x) {
  return std::vector<std::vector<std::string>> {x.ind[1],x.ind[0]};
 }

 //------------------------------------------------------

 // A simple replacement of tail when there is none to maintain generic code simple...
 struct nothing {
  template <typename... Args> explicit nothing(Args &&...) {} // takes anything, do nothing..
  nothing() {}
  using view_type = nothing;
  using regular_type = nothing;
  void rebind(nothing) {}
  template <typename RHS> void operator=(RHS &&) {}
  friend void h5_write(h5::group, std::string subgroup_name, nothing) {}
  friend void h5_read(h5::group, std::string subgroup_name, nothing) {}
  template <typename... A> friend nothing slice(nothing, A...) { return nothing(); }
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) {}
  friend nothing operator+(nothing, nothing) { return nothing(); }
  template <typename RHS> friend void assign_from_expression(nothing &, RHS) {}
  template<typename A> bool check_size(A) {return true;}
  bool is_empty() const { return false;}
 };

 inline nothing transpose(nothing) { return {};}
 inline nothing inverse(nothing) { return {};}
 inline nothing conj(nothing) { return {};}
 template <typename... T> nothing slice_target(nothing, T...) { return nothing(); }
 template <typename T> nothing operator+(nothing, T const &) { return nothing(); }
 template <typename T> nothing operator-(nothing, T const &) { return nothing(); }
 template <typename T> nothing operator*(nothing, T const &) { return nothing(); }
 template <typename T> nothing operator/(nothing, T const &) { return nothing(); }
 template <typename T> TYPE_DISABLE_IF(nothing, std::is_same<T, nothing>) operator+(T const &, nothing) { return nothing(); }
 template <typename T> TYPE_DISABLE_IF(nothing, std::is_same<T, nothing>) operator-(T const &, nothing) { return nothing(); }
 template <typename T> TYPE_DISABLE_IF(nothing, std::is_same<T, nothing>) operator*(T const &, nothing) { return nothing(); }
 template <typename T> TYPE_DISABLE_IF(nothing, std::is_same<T, nothing>) operator/(T const &, nothing) { return nothing(); }
}
}
