/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013-2016 by O. Parcollet
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
#pragma
#include <triqs/arrays.hpp>

namespace triqs {
namespace arrays {

 // a trait to remove the &&
 template <class T> struct _rm_rvalue_impl { using type = T; };
 template <class T> struct _rm_rvalue_impl<T &&> { using type = T; };
 template <class T> using _rm_rvalue = typename _rm_rvalue_impl<T>::type;

 // mako %for AM in ['array', 'matrix']:
 // mako %for CV in ['_const', ''] :
 /*mako
  <%
   TAG = ('Immutable' if CV else 'Mutable') + AM.capitalize()
   AMCV = AM + CV
  %>
 */
 template <typename A> struct MAKO_AMCV_proxy : TRIQS_CONCEPT_TAG_NAME(MAKO_TAG) {

  using A_t = std14::remove_reference_t<A>;
  static constexpr int rank = A_t::rank;

  A a;
  long n;

  using value_type = typename A_t::value_type;
  using traversal_order_t = typename A_t::traversal_order_t;
  using slicer_t = indexmaps::slicer<typename A_t::indexmap_type, long, ellipsis>;
  using indexmap_type = typename slicer_t::r_type;
  using domain_type = typename indexmap_type::domain_type;

  // mako %if AM == 'matrix' :
  using view_type = matrixMAKO_CV_view<value_type>;
  using const_view_type = matrix_const_view<value_type>;
  // mako %else :
  using view_type = arrayMAKO_CV_view<value_type, domain_type::rank>;
  using const_view_type = array_const_view<value_type, domain_type::rank>;
  // mako %endif

  // mako %if CV :
  using call_rt = value_type const &;
  static constexpr bool is_const = true;
  // mako %else :
  using call_rt = value_type &;
  static constexpr bool is_const = false;
  // mako %endif

  template <typename AA> MAKO_AMCV_proxy(AA &&a_, long n_) : a(std::forward<AA>(a_)), n(n_) {}

  indexmap_type indexmap() const { return slicer_t::invoke(a.indexmap(), n, ellipsis()); }
  domain_type domain() const { return indexmap().domain(); }

  auto storage() const { return a.storage(); }
  value_type const *restrict data_start() const { return &storage()[indexmap().start_shift()]; }
  value_type *restrict data_start() { return &storage()[indexmap().start_shift()]; }

  view_type operator()() const { return *this; }

  friend auto get_shape(MAKO_AMCV_proxy const &x) { return get_shape(x.a).front_pop(); } // FIXME  nvar

  template <typename... Args>
  std::c14::enable_if_t<!triqs::clef::is_any_lazy<Args...>::value, call_rt> operator()(Args &&... args) const {
   return a(n, std::forward<Args>(args)...);
  }
  TRIQS_CLEF_IMPLEMENT_LAZY_CALL();

  // mako %if CV :

  template <typename RHS> MAKO_AMCV_proxy &operator=(const RHS &X) = delete; // cannot assign to a const
  TRIQS_DELETE_COMPOUND_OPERATORS(MAKO_AMCV_proxy);

  // mako %else :

  template <typename RHS> MAKO_AMCV_proxy &operator=(const RHS &X) {
   triqs_arrays_assign_delegation(*this, X);
   return *this;
  }
  // same for the same class (not captured by the template).
  MAKO_AMCV_proxy &operator=(MAKO_AMCV_proxy const &X) {
   triqs_arrays_assign_delegation(*this, X);
   return *this;
  }

  TRIQS_DEFINE_COMPOUND_OPERATORS(MAKO_AMCV_proxy);

  // auto_assign like the main classes
  template <typename F> friend void triqs_clef_auto_assign(MAKO_AMCV_proxy x, F &&f) {
   foreach (x, array_auto_assign_worker<MAKO_AMCV_proxy, F>{x, f})
    ;
  }

  // mako %endif

  friend std::ostream &operator<<(std::ostream &out, MAKO_AMCV_proxy const &x) { return out << view_type(x); }
 };

 // mako %endfor
 // mako ## loop on const

 // if A is a const_view or A is passed by const &, then the proxy is const, otherwise it is not.
 template <typename A>
 std14::conditional_t<std14::decay_t<A>::is_const || std::is_const<std14::remove_reference_t<A>>::value,
                      MAKO_AM_const_proxy<_rm_rvalue<A>>, MAKO_AM_proxy<_rm_rvalue<A>>>
 make_MAKO_AM_proxy(A &&a, long n) {
  return {std::forward<A>(a), n};
 }

 // mako %endfor
 // mako ## loop on array, matrix
}
}

