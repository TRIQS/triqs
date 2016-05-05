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

 namespace details_proxy {

  // a trait to remove the &&
  template <class T> struct _rm_rvalue_impl { using type = T; };
  template <class T> struct _rm_rvalue_impl<T &&> { using type = T; };
  template <class T> using _rm_rvalue = typename _rm_rvalue_impl<T>::type;

  template <size_t I> using _long = long;
  template <typename IM, typename T> struct _sli;
  template <typename IM, size_t... Is> struct _sli<IM, std14::index_sequence<Is...>> {
   using type = indexmaps::slicer<IM, _long<Is>..., ellipsis>;
  };

  // implementation class
  template <typename A, typename FrozenArgs, bool IsConst> struct _proxy_impl {

   using A_t = std14::decay_t<A>;
   using value_type = typename A_t::value_type;
   A a;
   FrozenArgs frozen_args;
   static constexpr size_t n_args = std::tuple_size<FrozenArgs>::value;
   using _seq = std14::make_index_sequence<n_args>;

   using call_rt = std14::conditional_t<IsConst, value_type const &, value_type &>;
   static constexpr bool is_const = IsConst;

   using slicer_t = typename _sli<typename A_t::indexmap_type, _seq>::type;
   using indexmap_type = typename slicer_t::r_type;

   private:
   template <size_t... Is> auto _indexmap_impl(std14::index_sequence<Is...>) const {
    return slicer_t::invoke(a.indexmap(), std::get<Is>(frozen_args)..., ellipsis());
   }

   public:
   auto indexmap() const { return _indexmap_impl(_seq{}); }

   private: // FIXME use if constexpr
   template <size_t... Is, typename... Args> FORCEINLINE call_rt _call(std14::index_sequence<Is...>, Args &&... args) const {
    return a(std::get<Is>(frozen_args)..., std::forward<Args>(args)...);
   }

   public:
   template <typename... Args>
   std::c14::enable_if_t<!triqs::clef::is_any_lazy<Args...>::value, call_rt> operator()(Args &&... args) const {
    return _call(std14::make_index_sequence<n_args>(), std::forward<Args>(args)...);
   }
  };

  // specialize for simple case
  template <typename A, bool IsConst> struct _proxy_impl<A, long, IsConst> {

   using A_t = std14::decay_t<A>;
   using value_type = typename A_t::value_type;
   A a;
   long frozen_args;
   static constexpr size_t n_args = 1;

   using call_rt = std14::conditional_t<IsConst, value_type const &, value_type &>;
   static constexpr bool is_const = IsConst;

   using slicer_t = indexmaps::slicer<typename A_t::indexmap_type, long, ellipsis>;
   using indexmap_type = typename slicer_t::r_type;
   indexmap_type indexmap() const { return slicer_t::invoke(a.indexmap(), frozen_args, ellipsis()); }

   template <typename... Args>
   std::c14::enable_if_t<!triqs::clef::is_any_lazy<Args...>::value, call_rt> operator()(Args &&... args) const {
    return a(frozen_args, std::forward<Args>(args)...);
   }
  };
 } // details_proxy

 // mako %for AM in ['array', 'matrix']:
 // mako %for CV in ['_const', ''] :
 /*mako
  <%
   TAG = ('Immutable' if CV else 'Mutable') + AM.capitalize()
   AMCV = AM + CV
   ISCONST = str(bool(CV)).lower()
  %>
 */
 template <typename A, typename Tuple>
 struct MAKO_AMCV_proxy : public details_proxy::_proxy_impl<A, Tuple, MAKO_ISCONST>, TRIQS_CONCEPT_TAG_NAME(MAKO_TAG) {

  using B = details_proxy::_proxy_impl<A, Tuple, MAKO_ISCONST>;
  using value_type = typename B::value_type;
  using indexmap_type = typename B::indexmap_type;
  using A_t = std14::decay_t<A>;
  static constexpr int rank = A_t::rank;

  using traversal_order_t = typename A_t::traversal_order_t;
  using domain_type = typename indexmap_type::domain_type;

  // mako %if AM == 'matrix' :
  using view_type = matrixMAKO_CV_view<value_type>;
  using const_view_type = matrix_const_view<value_type>;
  // mako %else :
  using view_type = arrayMAKO_CV_view<value_type, domain_type::rank>;
  using const_view_type = array_const_view<value_type, domain_type::rank>;
  // mako %endif

  template <typename AA> MAKO_AMCV_proxy(AA &&a_, Tuple const &t) : B{std::forward<AA>(a_), t} {}

  domain_type domain() const { return this->indexmap().domain(); }

  auto storage() const { return this->a.storage(); }
  value_type const *restrict data_start() const { return &storage()[this->indexmap().start_shift()]; }
  value_type *restrict data_start() { return &storage()[this->indexmap().start_shift()]; }

  view_type operator()() const { return *this; }

  using B::operator();
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

 template <typename A, typename T> auto get_shape(MAKO_AMCV_proxy<A,T> const &x) {
  return get_shape(x.a).template front_mpop<MAKO_AMCV_proxy<A,T>::n_args>();
 }

 // mako %endfor
 // mako ## loop on const

 // if A is a const_view or A is passed by const &, then the proxy is const, otherwise it is not.
 template <typename A, typename... T>
 std14::conditional_t<std14::decay_t<A>::is_const || std::is_const<std14::remove_reference_t<A>>::value,
                      MAKO_AM_const_proxy<details_proxy::_rm_rvalue<A>, std::tuple<T...>>, MAKO_AM_proxy<details_proxy::_rm_rvalue<A>, std::tuple<T...>>>
 make_MAKO_AM_proxy(A &&a, std::tuple<T...> const &n) {
  return {std::forward<A>(a), n};
 }

 // if A is a const_view or A is passed by const &, then the proxy is const, otherwise it is not.
 template <typename A>
 std14::conditional_t<std14::decay_t<A>::is_const || std::is_const<std14::remove_reference_t<A>>::value,
                      MAKO_AM_const_proxy<details_proxy::_rm_rvalue<A>, long>, MAKO_AM_proxy<details_proxy::_rm_rvalue<A>, long>>
 make_MAKO_AM_proxy(A &&a, long n) {
  return {std::forward<A>(a), n};
 }

 // mako %endfor
 // mako ## loop on array, matrix
}
}

