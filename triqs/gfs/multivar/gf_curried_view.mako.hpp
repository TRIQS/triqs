/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2016 by O. Parcollet
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

/*mako
<%
  g1 = ['gf_curried_view',        'view',        'gf_view',       'false']
  g2 = ['gf_curried_const_view',  'const_view',  'gf_const_view', 'true']
%>
%for GF, GRVC, GV, ISCONST in [g1,g2]:
*/


namespace triqs {
namespace gfs {

 template <typename Var, typename Lambda> class gf_curried_const_view;

 template <typename Var, typename Lambda> class MAKO_GF : TRIQS_CONCEPT_TAG_NAME(ImmutableGreenFunction) {

  using self_t = MAKO_GF; // simplify code generation later
 public:
  using const_view_type = gf_curried_const_view<Var, Lambda>;
  using view_type = self_t;

  using variable_t = Var;
  using mesh_t = gf_mesh<Var>;

  using domain_t = typename mesh_t::domain_t;
  using mesh_point_t = typename mesh_t::mesh_point_t;
  using mesh_index_t = typename mesh_t::index_t;
  using linear_mesh_index_t = typename mesh_t::linear_index_t;

  static constexpr bool is_view = true;
  static constexpr bool is_const = MAKO_ISCONST;

  // ------------- Constructors -----------------------------

  MAKO_GF(mesh_t m, Lambda l) : _mesh(std::move(m)), lambda(std::move(l)) {}

  MAKO_GF(MAKO_GF const &) = default;
  MAKO_GF(MAKO_GF &&) = default;

  MAKO_GF&operator=(MAKO_GF const &) = delete;
  MAKO_GF&operator=(MAKO_GF &&) = delete;

//mako %if GRVC=='const_view':
  gf_curried_const_view(gf_curried_view<Var, Lambda> const &g) : _mesh(g._mesh), lambda(g.lambda) {}
//mako %endif

  // ------------- Data & Accessors -----------------------------

  private:
  mesh_t _mesh;
  Lambda lambda;

  public:
  mesh_t const &mesh() const { return _mesh; }
  domain_t const &domain() const { return _mesh.domain(); }

  // ------------- All the call operators without lazy arguments -----------------------------

  const_view_type operator()() const { return *this; }

  template <typename... Args,
            typename Enable = std14::enable_if_t<(sizeof...(Args) != 0) && !triqs::clef::is_any_lazy<Args...>::value>>
  decltype(auto) operator()(Args && ... args) const {
   if (!_mesh.is_within_boundary(args...)) TRIQS_RUNTIME_ERROR << "Evaluation out of the mesh";
   return _mesh.evaluate(typename mesh_t::default_interpol_policy{}, *this, std::forward<Args>(args)...);
  }

  // ------------- Call with lazy arguments -----------------------------

  /// Calls with at least one lazy argument : we make a clef expression, cf clef documentation
  template <typename... Args> clef::make_expr_call_t<self_t &, Args...> operator()(Args &&... args) & {
   return clef::make_expr_call(*this, std::forward<Args>(args)...);
  }

  template <typename... Args>
  clef::make_expr_call_t<self_t const &, Args...> operator()(Args &&... args) const &{
   return clef::make_expr_call(*this, std::forward<Args>(args)...);
  }

  template <typename... Args> clef::make_expr_call_t<self_t, Args...> operator()(Args &&... args) && {
   return clef::make_expr_call(std::move(*this), std::forward<Args>(args)...);
  }

  // ------------- All the [] operators without lazy arguments -----------------------------

  public:
  decltype(auto) operator[](mesh_index_t const &arg) { return lambda(_mesh.index_to_linear(arg)); }
  decltype(auto) operator[](mesh_index_t const &arg) const { return lambda(_mesh.index_to_linear(arg)); }

  decltype(auto) operator[](mesh_point_t const &x) {
#ifdef TRIQS_DEBUG
   if (this->_mesh != x.mesh()) TRIQS_RUNTIME_ERROR << "gf[ ] : mesh point's mesh and gf's mesh mismatch";
#endif
   return lambda(x.linear_index());
  }
  decltype(auto) operator[](mesh_point_t const &x) const {
#ifdef TRIQS_DEBUG
   if (this->_mesh != x.mesh()) TRIQS_RUNTIME_ERROR << "gf[ ] : mesh point's mesh and gf's mesh mismatch";
#endif
   return lambda(x.linear_index());
  }

  /// no lazy argument ...
  //template <typename Args, typename Enable = std14::enable_if_t<!triqs::clef::is_any_lazy<Args>::value>>
  //decltype(auto) operator[](Args && args) const {
  // return operator[](std::forward<Args>(args));
  // }
 
  // ------------- [] with lazy arguments -----------------------------

  template <typename Arg>
  clef::make_expr_subscript_t<self_t const &, Arg> operator[](Arg &&arg) const &{
   return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
  }

  template <typename Arg> clef::make_expr_subscript_t<self_t &, Arg> operator[](Arg &&arg) & {
   return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
  }

  template <typename Arg> clef::make_expr_subscript_t<self_t, Arg> operator[](Arg &&arg) && {
   return clef::make_expr_subscript(std::move(*this), std::forward<Arg>(arg));
  }
 
  /// --------------------- A direct access to the grid point --------------------------
  //  [] and access to the grid point

  template <typename... Args> auto get_from_linear_index(Args &&... args) {
   return operator[](linear_mesh_index_t(std::forward<Args>(args)...));
  }

  template <typename... Args> auto get_from_linear_index(Args &&... args) const {
   return operator[](linear_mesh_index_t(std::forward<Args>(args)...));
  }

  template <typename... Args> auto on_mesh(Args &&... args) {
   return operator[](mesh_index_t(std::forward<Args>(args)...));
  }

  template <typename... Args> auto on_mesh(Args &&... args) const {
   return operator[](mesh_index_t(std::forward<Args>(args)...));
  }
    
  /// --------------------- on mesh (g) : the call before [] -------------------------
  // This is a workaround the the lack of multi argument [] in C++

  // mesh points should be treated slighly differently : take their index....
  template <typename... T> auto on_mesh(mesh_point<T> const &... args) { return on_mesh(args.index()...); }
  template <typename... T> auto on_mesh(mesh_point<T> const &... args) const { return on_mesh(args.index()...); }

  // The on_mesh little adaptor ....
  private:
  struct _on_mesh_wrapper_const {
   MAKO_GF const &f;
   template <typename... Args, typename Enable = std14::enable_if_t<!triqs::clef::is_any_lazy<Args...>::value>>
   // requires(triqs::clef::is_any_lazy<Args...>)
   auto operator()(Args &&... args) const {
    return f.on_mesh(std::forward<Args>(args)...);
   }
   TRIQS_CLEF_IMPLEMENT_LAZY_CALL();
  };
  struct _on_mesh_wrapper {
   MAKO_GF &f;
   template <typename... Args, typename Enable = std14::enable_if_t<!triqs::clef::is_any_lazy<Args...>::value>>
   auto operator()(Args &&... args) const {
    return f.on_mesh(std::forward<Args>(args)...);
   }
   TRIQS_CLEF_IMPLEMENT_LAZY_CALL();
  };

  public:
  _on_mesh_wrapper_const friend on_mesh(MAKO_GF const &f) {
   return {f};
  }
  _on_mesh_wrapper friend on_mesh(MAKO_GF &f) {
   return {f};
  }
 
 };

 template <typename M, typename L, typename RHS> void triqs_clef_auto_assign(MAKO_GF<M, L> &g, RHS const &rhs) {
  triqs_clef_auto_assign_impl(g, rhs, typename std::is_base_of<tag::composite, gf_mesh<M>>::type());
 }

 // enable the writing g[om_] << ... also
 template <typename M, typename L, typename RHS> void triqs_clef_auto_assign_subscript(MAKO_GF<M, L> &g, RHS const &rhs) {
  triqs_clef_auto_assign(g, rhs);
 }

}
}
// mako %endfor

