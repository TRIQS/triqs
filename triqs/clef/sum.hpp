/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by O. Parcollet
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
#include "../clef.hpp"

namespace triqs {
namespace clef {

 //--------------------------------------------------------------------------------------------------
 //  sum of expressions
 // -------------------------------------------------------------------------------------------------

 // sum a function f on a domain D, using a simple foreach
 template <typename F, typename D>
 // requires(!triqs::clef::is_any_lazy<F, D>::value)
 auto sum_f_domain_impl(F const& f, D const& d)
     -> std::c14::enable_if_t<!triqs::clef::is_any_lazy<F, D>::value, std14::decay_t<decltype(f(*(d.begin())))>> {
  auto it = d.begin(), ite = d.end();
  if (it == ite) TRIQS_RUNTIME_ERROR << "Sum over an empty domain";
  auto res = f(*it);
  ++it;
  for (; it != ite; ++it) res = res + f(*it);
  return res;
 }

 TRIQS_CLEF_MAKE_FNT_LAZY(sum_f_domain_impl);

#ifndef TRIQS_CPP11
 // sum( expression, i = domain)
 template <typename Expr, int N, typename D> decltype(auto) sum(Expr const& f, clef::pair<N, D> const& d) {
  return sum_f_domain_impl(make_function(f, clef::placeholder<N>()), d.rhs);
 }
 // warning : danger here : if the d is a temporary, the domain MUST be moved in case the Expr
 // is still lazy after eval, or we will obtain a dangling reference.
 template <typename Expr, int N, typename D> decltype(auto) sum(Expr const& f, clef::pair<N, D>&& d) {
  return sum_f_domain_impl(make_function(f, clef::placeholder<N>()), std::move(d.rhs));
 }

 // two or more indices : sum recursively
 template <typename Expr, typename A0, typename A1, typename... A> auto sum(Expr const& f, A0&& a0, A1&& a1, A&&... a) {
  return sum(sum(f, std::forward<A0>(a0)), std::forward<A1>(a1), std::forward<A>(a)...);
 }

#else
 // sum( expression, i = domain)
 template <typename Expr, int N, typename D>
 auto sum(Expr const& f, clef::pair<N, D> const& d)
     DECL_AND_RETURN(sum_f_domain_impl(make_function(f, clef::placeholder<N>()), d.rhs));

 // warning : danger here : if the d is a temporary, the domain MUST be moved in case the Expr
 // is still lazy after eval, or we will obtain a dangling reference.
 template <typename Expr, int N, typename D>
 auto sum(Expr const& f, clef::pair<N, D>&& d)
     DECL_AND_RETURN(sum_f_domain_impl(make_function(f, clef::placeholder<N>()), std::move(d.rhs)));

 // two or more indices : sum recursively
 template <typename Expr, typename A0, typename A1, typename... A>
 auto sum(Expr const& f, A0&& a0, A1&& a1, A&&... a)
     DECL_AND_RETURN(sum(sum(f, std::forward<A0>(a0)), std::forward<A1>(a1), std::forward<A>(a)...));

#endif
}
}
