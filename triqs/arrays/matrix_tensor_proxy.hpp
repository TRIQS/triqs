/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#ifndef TRIQS_GFS_MATRIX_TENSOR_PROXY_H
#define TRIQS_GFS_MATRIX_TENSOR_PROXY_H
#include <triqs/arrays.hpp>

namespace triqs {
namespace arrays {

 template <class T> struct _remove_rvalue_ref {
  typedef T type;
 };
 template <class T> struct _remove_rvalue_ref<T &&> {
  typedef T type;
 };

 // tensor const
 template <typename A, bool IsMatrix>
 struct const_matrix_tensor_proxy : std::conditional<IsMatrix, TRIQS_CONCEPT_TAG_NAME(ImmutableMatrix),
                                                     TRIQS_CONCEPT_TAG_NAME(ImmutableArray)>::type {

  typedef typename std::remove_reference<A>::type A_t;
  static constexpr bool is_const = true;
  
  A a;
  long n;

  typedef typename A_t::value_type value_type;
  using traversal_order_t = typename A_t::traversal_order_t;
  typedef indexmaps::slicer<typename A_t::indexmap_type, long, ellipsis> slicer_t;
  typedef typename slicer_t::r_type indexmap_type;
  typedef typename indexmap_type::domain_type domain_type;
  typedef typename std::conditional<IsMatrix, matrix_const_view<value_type>,
                                    array_const_view<value_type, domain_type::rank>>::type view_type;

  template <typename AA> const_matrix_tensor_proxy(AA &&a_, long n_) : a(std::forward<AA>(a_)), n(n_) {}

  indexmap_type indexmap() const { return slicer_t::invoke(a.indexmap(), n, ellipsis()); }
  domain_type domain() const { return indexmap().domain(); }

  auto storage() const DECL_AND_RETURN(a.storage());
  value_type const *restrict data_start() const { return &storage()[indexmap().start_shift()]; }
  value_type *restrict data_start() { return &storage()[indexmap().start_shift()]; }

  view_type operator()() const { return *this; }

  template <typename... Args>
  std::c14::enable_if_t<!triqs::clef::is_any_lazy<Args...>::value, value_type const &> operator()(Args &&... args) const {
   return a(n, std::forward<Args>(args)...);
  }
  TRIQS_CLEF_IMPLEMENT_LAZY_CALL();

  template <typename RHS> const_matrix_tensor_proxy &operator=(const RHS &X) =delete; // cannot assign to a const 

  TRIQS_DELETE_COMPOUND_OPERATORS(const_matrix_tensor_proxy);
  friend std::ostream &operator<<(std::ostream &out, const_matrix_tensor_proxy const &x) { return out << view_type(x); }
 };

 template <typename A, bool IsMatrix>
 auto get_shape(const_matrix_tensor_proxy<A, IsMatrix> const &x) DECL_AND_RETURN(get_shape(x.a).front_pop());

 // factory
 template <typename A>
 const_matrix_tensor_proxy<typename _remove_rvalue_ref<A>::type, false> make_const_tensor_proxy(A &&a, long n) {
  return {std::forward<A>(a), n};
 }
 template <typename A>
 const_matrix_tensor_proxy<typename _remove_rvalue_ref<A>::type, true> make_const_matrix_proxy(A &&a, long n) {
  return {std::forward<A>(a), n};
 }

 // tensor no const
 template <typename A, bool IsMatrix>
 struct matrix_tensor_proxy : std::conditional<IsMatrix, TRIQS_CONCEPT_TAG_NAME(MutableMatrix),
                                               TRIQS_CONCEPT_TAG_NAME(MutableArray)>::type {

  typedef typename std::remove_reference<A>::type A_t;
  static constexpr bool is_const = false;

  A a;
  long n;

  typedef typename A_t::value_type value_type;
  using traversal_order_t = typename A_t::traversal_order_t;
  typedef indexmaps::slicer<typename A_t::indexmap_type, long, ellipsis> slicer_t;
  typedef typename slicer_t::r_type indexmap_type;
  typedef typename indexmap_type::domain_type domain_type;
  typedef typename std::conditional<IsMatrix, matrix_view<value_type>,
                                    array_view<value_type, domain_type::rank>>::type view_type;

  template <typename AA> matrix_tensor_proxy(AA &&a_, long n_) : a(std::forward<AA>(a_)), n(n_) {}

  indexmap_type indexmap() const { return slicer_t::invoke(a.indexmap(), n, ellipsis()); }
  domain_type domain() const { return indexmap().domain(); }

  auto storage() const DECL_AND_RETURN(a.storage());
  value_type const *restrict data_start() const { return &storage()[indexmap().start_shift()]; }
  value_type *restrict data_start() { return &storage()[indexmap().start_shift()]; }

  view_type operator()() const { return *this; }

  template <typename... Args>
  std::c14::enable_if_t<!triqs::clef::is_any_lazy<Args...>::value, value_type &> operator()(Args &&... args) const {
   return a(n, std::forward<Args>(args)...);
  }

  TRIQS_CLEF_IMPLEMENT_LAZY_CALL();

  template <typename RHS> matrix_tensor_proxy &operator=(const RHS &X) {
   triqs_arrays_assign_delegation(*this, X);
   return *this;
  }

  TRIQS_DEFINE_COMPOUND_OPERATORS(matrix_tensor_proxy);
  friend std::ostream &operator<<(std::ostream &out, matrix_tensor_proxy const &x) {
   return out << view_type{x};
  }

  template <typename F> friend void triqs_clef_auto_assign(matrix_tensor_proxy x, F &&f) { foreach(x(), std::forward<F>(f)); }
 };

 template <typename A, bool IsMatrix>
 auto get_shape(matrix_tensor_proxy<A, IsMatrix> const &x) DECL_AND_RETURN(get_shape(x.a).front_pop());

 // factory
 template <typename A> matrix_tensor_proxy<typename _remove_rvalue_ref<A>::type, false> make_tensor_proxy(A &&a, long n) {
  return {std::forward<A>(a), n};
 }
 template <typename A> matrix_tensor_proxy<typename _remove_rvalue_ref<A>::type, true> make_matrix_proxy(A &&a, long n) {
  return {std::forward<A>(a), n};
 }
}
}
#endif

