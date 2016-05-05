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
#include "./defs.hpp"
#include "./gf_indices.hpp"
#include "../singularity/tail.hpp"
#include "../singularity/nothing.hpp"
#include "./data_proxy.hpp"

namespace triqs {
namespace gfs {

 /*----------------------------------------------------------
  *   Declaration of main types : gf, gf_view, gf_const_view
  *--------------------------------------------------------*/

 template <typename Var, typename Target = matrix_valued> class gf;
 template <typename Var, typename Target = matrix_valued> class gf_view;
 template <typename Var, typename Target = matrix_valued> class gf_const_view;

 /*----------------------------------------------------------
  *   Traits
  *--------------------------------------------------------*/

 // The trait that "marks" the Green function
 TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT(ImmutableGreenFunction);

 // Is G a gf, gf_view, gf_const_view
 // is_gf<G> is true iif G is a gf or a view
 // is_gf<G,M0> is true iif  G is a gf or a view and its mesh is M0
 template <typename G, typename M0 = void> struct is_gf : std::false_type {};

 template <typename M, typename T> struct is_gf<gf<M, T>, void> : std::true_type {};
 template <typename M, typename T> struct is_gf<gf<M, T>, M> : std::true_type {};
 template <typename M, typename T> struct is_gf<gf_view<M, T>, void> : std::true_type {};
 template <typename M, typename T> struct is_gf<gf_view<M, T>, M> : std::true_type {};

 template <typename M, typename T>
 struct is_gf<gf_const_view<M, T>, void> : std::true_type {};

 template <typename M, typename T> struct is_gf<gf_const_view<M, T>, M> : std::true_type {};

 template <typename G, typename M> struct is_gf<G &, M> : is_gf<G, M> {};
 template <typename G, typename M> struct is_gf<G const &, M> : is_gf<G, M> {};
 template <typename G, typename M> struct is_gf<G &&, M> : is_gf<G, M> {};

 /// ---------------------------  implementation  ---------------------------------

 namespace details { 
  // FIXME : replace by if constexpr when possible
  template<typename A> void _rebind_helper(A & x, A const & y) { x.rebind(y);}
  inline void _rebind_helper(dcomplex & x, dcomplex const &y) { x = y;}
  inline void _rebind_helper(double & x, double const &y) { x = y;}
 }

 /*mako
 %for ROOT in ['gf'] :
  %for RVC in ['regular', 'view', 'const_view'] :
  <%
     EXT = '_' + RVC if 'view' in RVC else ''
     GF = ROOT + EXT
     GV = ROOT + '_' + (RVC if 'view' in RVC else 'view')
     ISVIEW = str('view' in GF).lower()
     ISCONST = str(RVC == 'const_view').lower()
  %>
 */

 // ----------------------  MAKO_GF -----------------------------------------
 /**
  * MAKO_GF : the main class
  */
 template <typename Var, typename Target>
 class MAKO_GF : TRIQS_CONCEPT_TAG_NAME(ImmutableGreenFunction) {

  public:

  static constexpr bool is_view = MAKO_ISVIEW;
  static constexpr bool is_const = MAKO_ISCONST;

  using mutable_view_type = gf_view<Var, Target>;

  /// The associated const view type (gf_const_view <....>)
  using const_view_type = gf_const_view<Var, Target>;

  /// The associated view type (non const)
  using view_type = MAKO_GV<Var, Target>;

  /// The associated regular type (gf<....>)
  using regular_type = gf<Var, Target>;

  using variable_t = Var;
  using target_t = Target;

  /// Type of the mesh
  using mesh_t = gf_mesh<Var>;
  using domain_t = typename mesh_t::domain_t;
  static constexpr int arity = get_n_variables<Var>::value;

  using mesh_point_t = typename mesh_t::mesh_point_t;
  using mesh_index_t = typename mesh_t::index_t;
  using linear_mesh_index_t = typename mesh_t::linear_index_t;
  using indices_t = gf_indices;
  using evaluator_t = gf_evaluator<Var, Target> ;
  
  using scalar_t = typename Target::scalar_t;

  using data_regular_t = arrays::array<scalar_t, arity + Target::rank>;
  using data_view_t = typename data_regular_t::view_type;
  using data_const_view_t = typename data_regular_t::const_view_type;
  using data_t = data_MAKO_RVC_t;

  using zero_regular_t = std14::conditional_t<Target::rank !=0, arrays::array<scalar_t, Target::rank>, scalar_t>;
  using zero_const_view_t = std14::conditional_t<Target::rank !=0, arrays::array_const_view<scalar_t, Target::rank>, scalar_t>;
  using zero_view_t = zero_const_view_t;
  using zero_t = zero_MAKO_RVC_t;

  using _singularity_regular_t = gf_singularity_t<Var, Target>;
  using _singularity_view_t = typename _singularity_regular_t::view_type;
  using _singularity_const_view_t = typename _singularity_regular_t::const_view_type;
  using singularity_t = _singularity_MAKO_RVC_t;

  // ------------- Accessors -----------------------------

  /// Access the  mesh
  mesh_t const &mesh() const { return _mesh; }
  domain_t const &domain() const { return _mesh.domain(); }

  /// Direct access to the data array
  data_t &data() { return _data; }
  
  /// Const version
  data_t const &data() const { return _data; }

  /// Shape of the target
  auto target_shape() const { return _data.shape().template front_mpop<arity>(); } // drop arity dims

  /// Shape of the data
  auto data_shape() const { return _data.shape(); }

  ///
  zero_t const &get_zero() const { return _zero; }

  /// Access to the singularity
  singularity_t &singularity() { return _singularity; }

  /// Const version
  singularity_t const &singularity() const { return _singularity; }

  indices_t const &indices() const { return _indices; }

  private:
  mesh_t _mesh;
  data_t _data;
  zero_t _zero;
  singularity_t _singularity;
  indices_t _indices;

  public:
  std::string name;

  private:
  
  using dproxy_t = details::_data_proxy<Target>;

  // -------------------------------- impl. details common to all classes -----------------------------------------------

  // for delegation only
  private:
  struct impl_tag {};
  struct impl_tag2 {};

  // build a zero from a slice of data
  // MUST be static since it is used in constructors... (otherwise bug in clang)
  template <typename T> static zero_t __make_zero(T, data_t const &d) {
   auto r = zero_regular_t{d.shape().template front_mpop<arity>()};
   r() = 0;
   return r;
  }
  static zero_t __make_zero(scalar_valued, data_t const &d) { return 0; } // special case
  static zero_t _make_zero(data_t const &d) { return __make_zero(Target{}, d); }
  zero_t _remake_zero() { return _zero = _make_zero(_data); } // NOT in constructor...

  template <typename G>
  MAKO_GF(impl_tag2, G &&x)
     : _mesh(x.mesh())
     , _data(x.data())
     , _zero(_make_zero(_data))
     , _singularity(x.singularity())
     , _indices(x.indices())
     , name(x.name) {}

  template <typename M, typename D, typename S>
  MAKO_GF(impl_tag, M &&m, D &&dat, S &&sing, indices_t ind)
     : _mesh(std::forward<M>(m))
     , _data(std::forward<D>(dat))
     , _zero(_make_zero(_data))
     , _singularity(std::forward<S>(sing))
     , _indices(std::move(ind)) {
   if (!(_indices.empty() or (is_tail_valued(Target())) or  _indices.has_shape(target_shape())))
    TRIQS_RUNTIME_ERROR << "Size of indices mismatch with data size";
  }

  public:
  // mako %if RVC == 'regular' :

  /// Construct an empty Green function (with empty array).
  gf() {} // all arrays of zero size (empty)

  // mako %endif

  /// Copy constructor
  MAKO_GF(MAKO_GF const &x)
     : _mesh(x.mesh()), _data(x.data()), _zero(x._zero), _singularity(x.singularity()), _indices(x.indices()), name(x.name) {}

  /// Move constructor
  MAKO_GF(MAKO_GF &&) = default;

  void swap_impl(MAKO_GF &b) noexcept {
   using std::swap;
   swap(this->_mesh, b._mesh);
   swap(this->_data, b._data);
   swap(this->_zero, b._zero);
   swap(this->_singularity, b._singularity);
   swap(this->_indices, b._indices);
   swap(this->name, b.name);
  }
  
  using singularity_factory = gf_singularity_factory<_singularity_regular_t>;

  // mako ## ------------------------------------------------  regular class ---------------------------------
  // mako %if RVC == 'regular' :
 
  private:
  using target_shape_t = arrays::mini_vector<int, Target::rank - is_tail_valued(Target{})>; 

  template <typename U> static auto make_data_shape(U, mesh_t const &m, target_shape_t const &shap) {
   return join(m.size_of_components(), shap);
  }

  template <typename U> static auto make_data_shape(tail_valued<U>, mesh_t const &m, target_shape_t const &shap) {
   return join(mini_vector<int,2>{int(m.size()), __tail<matrix_valued>::_size()}, shap); // shap.front_append(m.size());
  }

  public:
  // Construct from the data. Using the "pass by value" pattern + move
  gf(mesh_t m, data_t dat, singularity_t si, indices_t ind)
     : gf(impl_tag{}, std::move(m), std::move(dat), std::move(si), std::move(ind)) {}

  // Construct from mesh, target_shape, memory order
  gf(mesh_t m, target_shape_t shape, arrays::memory_layout<arity + Target::rank> const &ml, indices_t const &ind = indices_t{})
     : gf(impl_tag{}, std::move(m), data_t(make_data_shape(Target{}, m, shape), ml), singularity_factory::make(m, shape), ind) {
   if (this->_indices.empty()) this->_indices = indices_t(shape);
  }

  // Construct from mesh, target_shape, memory order
  gf(mesh_t m, target_shape_t shape = target_shape_t{}, indices_t const &ind = indices_t{}, std::string _name = {})
     : gf(impl_tag{}, std::move(m), data_t(make_data_shape(Target{}, m, shape)), singularity_factory::make(m, shape), ind) {
   if (this->_indices.empty()) this->_indices = indices_t(shape);
   name = std::move(_name);
  }

  /// From a gf_view of the same kind
  gf(gf_view<Var, Target> const &g) : gf(impl_tag2{} , g) {}

  /// From a const_gf_view of the same kind
  gf(gf_const_view<Var, Target> const &g) : gf(impl_tag2{} , g) {}

  /// Construct from anything which models ImmutableGreenFunction.
  // TODO: We would like to refine this, G should have the same mesh, target, at least ...
  template <typename G> gf(G const &x, std14::enable_if_t<ImmutableGreenFunction<G>::value> *dummy = 0) : gf() { *this = x; }

  /// Construct from the mpi lazy class of the implementation class, cf mpi section
  // NB : type must be the same, e.g. g2(mpi_reduce(g1)) will work only if mesh, Target, Singularity are the same...
  template <typename Tag> gf(mpi_lazy<Tag, gf_const_view<Var, Target>> x) : gf() { operator=(x); }

  /// ---------------  swap --------------------

  /// implement the swap
  friend void swap(gf &a, gf &b) noexcept { a.swap_impl(b); }

  /// ---------------  Operator = --------------------

  /// Copy assignment
  gf &operator=(gf const &rhs) { return *this = gf(rhs); } // use move =
  //
  gf &operator=(gf &rhs) { return *this = gf(rhs); } // use move =
  /// Move assignment
  gf &operator=(gf &&rhs) noexcept {
   this->swap_impl(rhs);
   return *this;
  }

  /**
   * Assignment operator
   *
   * @tparam RHS Type of the right hand side rhs
   *
   * 		 RHS can be anything modeling the gf concept TBW
   * 		 In particular lazy expression with Green functions
   * @param rhs
   * @example    gf_assign_0
   *
   * The assignment resizes the mesh and the data, invalidating all pointers on them.
   *
   */
  template <typename RHS> gf &operator=(RHS &&rhs) {
   _mesh = rhs.mesh();
   _data.resize(rhs.data_shape());
   _remake_zero();
   for (auto const &w : _mesh) (*this)[w] = rhs[w];
   _singularity = rhs.singularity();
   // to be implemented : there is none in the gf_expr in particular....
   // indices and name are not affected by it ???
   return *this;
  }

  // other = late, cf MPI

  // mako ## ------------------------------------------------  view class ---------------------------------
  // mako %elif RVC == 'const_view' :

  // ---------------  Constructors --------------------

  // views can not be default constructed
  gf_const_view() = delete;

  /// Makes a const view
  gf_const_view(gf_view<Var, Target> const &g) : gf_const_view(impl_tag2{}, g) {}

  /// Makes a const view
  gf_const_view(gf<Var, Target> const &g) : gf_const_view(impl_tag2{}, g) {}

  /// Makes a const view
  gf_const_view(gf<Var, Target> &g) : gf_const_view(impl_tag2{}, g) {} // from a gf &

  /// Makes a const view
  gf_const_view(gf<Var, Target> &&g) noexcept : gf_const_view(impl_tag2{}, std::move(g)) {} // from a gf &&

  // Construct from mesh, data, ....
  template <typename D>
  gf_const_view(mesh_t const &m, D const &dat, singularity_t const &t, indices_t const &ind)
     : gf_const_view(impl_tag{}, m, dat, t, ind) {}

  // Construct from mesh, data, ....
  // Hum... *this is not constructed
  template <typename D>
  gf_const_view(mesh_t const &m, D const &dat)
     : gf_const_view(impl_tag{}, m, dat, singularity_factory::make(m, dat.shape().template front_mpop<arity>()), {}) {}

  // ---------------  swap --------------------

  /// Swap
  friend void swap(gf_const_view &a, gf_const_view &b) noexcept { a.swap_impl(b); }

  // ---------------  Rebind --------------------
  /// Rebind the view
   void rebind(gf_const_view<Var, Target> const &X) noexcept {
   this->_mesh = X._mesh;
   this->_data.rebind(X._data);
   details::_rebind_helper(_zero, X._zero);
   this->_singularity.rebind(X._singularity);
   this->_indices = X._indices;
   this->name = X.name;
  }

  /// Rebind on a non const view
  void rebind(gf_view<Var, Target> const &X) noexcept { rebind(gf_const_view{X}); }

  // ---------------  No = since it is const ... --------------------
  gf_const_view &operator=(gf_const_view const &) = delete; // a const view can not be assigned to

  // mako ## ------------------------------------------------  view class ---------------------------------
  // mako %elif RVC == 'view' :

  public:
  // ---------------  Constructors --------------------

  // views can not be default constructed
  gf_view() = delete;

  // Allow to construct a view from a gf with a different evaluator, except const_views ...
  /// Makes a view
  gf_view(gf_const_view<Var, Target> const &g) = delete;
  
  /// Makes a view
  gf_view(gf<Var, Target> const &g) = delete;
  
  /// Makes a view
  gf_view(gf<Var, Target> &g) : gf_view(impl_tag2{}, g) {}
  
  /// Makes a view
  gf_view(gf<Var, Target> &&g) noexcept : gf_view(impl_tag2{}, std::move(g)) {} // from a gf &&

  // Construct from mesh, data, ....
  template <typename D>
  gf_view(mesh_t const &m, D &&dat, singularity_t const &t, indices_t const &ind = indices_t{})
     : gf_view(impl_tag{}, m, std::forward<D>(dat), t, ind) {}

  // Construct from mesh, data, ....
  // Hum... *this is not constructed
  template <typename D>
  gf_view(mesh_t const &m, D const &dat)
     : gf_view(impl_tag{}, m, dat, singularity_factory::make(m, dat.shape().template front_mpop<arity>()), {}) {}

  // ---------------  swap --------------------
  /// Swap
  friend void swap(gf_view &a, gf_view &b) noexcept { a.swap_impl(b); }

  // ---------------  Rebind --------------------
  /// Rebind
   void rebind(gf_view<Var, Target> const &X) noexcept {
   this->_mesh = X._mesh;
   this->_data.rebind(X._data);
   details::_rebind_helper(_zero, X._zero);
   this->_singularity.rebind(X._singularity);
   this->_indices = X._indices;
   this->name = X.name;
  }

  // ---------------  operator =  --------------------
  /// Copy the data, without resizing the view.
  gf_view &operator=(gf_view const &rhs) {
   triqs_gf_view_assign_delegation(*this, rhs);
   return *this;
  }

  /**
   * Assignment
   *
   * @tparam RHS WRITE A LIST OF POSSIBLES ...
   */
  template <typename RHS> gf_view &operator=(RHS const &rhs) {
   triqs_gf_view_assign_delegation(*this, rhs);
   return *this;
  }

  // mako %endif
  // mako ## ----- the rest is common to the three classes .... ---------------------------------

  public:
  // ------------- apply_on_data -----------------------------

  // mako %for C in ['', 'const'] :
  // mako <% GFVIEW = 'gf_const_view' if (C or RVC == 'const_view') else 'gf_view' %>
  template <typename Fdata, typename Fsing, typename Find> auto apply_on_data(Fdata &&fd, Fsing &&fs, Find &&fi) MAKO_C {
   auto d2 = fd(_data);
   using t2 = std14::conditional_t<is_tail_valued(Target()), tail_valued<target_from_array<decltype(d2), arity + 1>>,
                                   target_from_array<decltype(d2), arity>>;
   using gv_t = MAKO_GFVIEW<Var, t2>;
   return gv_t{_mesh, d2, typename gv_t::singularity_t{fs(_singularity)}, fi(_indices)};
  }

  template <typename Fdata, typename Fsing> auto apply_on_data(Fdata &&fd, Fsing &&fs) MAKO_C {
   return apply_on_data(std::forward<Fdata>(fd), std::forward<Fsing>(fs), [](auto &) { return indices_t{}; });
  }
  // mako %endfor

  // ------------- All the call operators without lazy arguments -----------------------------

  // First, a simple () returns a view, like for an array...
  /// Makes a const view of *this
  const_view_type operator()() const { return *this; }
  /// Makes a view of *this if it is non const
  view_type operator()() { return *this; }

  
  // Calls are (perfectly) forwarded to the evaluator::operator(), except mesh_point_t and when
  // there is at least one lazy argument ...
  template <typename... Args>        // match any argument list, picking out the first type : () is not permitted
  typename boost::lazy_disable_if_c< // disable the template if one the following conditions it true
      (sizeof...(Args) == 0) || clef::is_any_lazy<Args...>::value ||
          ((sizeof...(Args) != evaluator_t::arity) && (evaluator_t::arity != -1)) // if -1 : no check
      ,
      std::result_of<evaluator_t(MAKO_GF, Args...)> // what is the result type of call
      >::type                                      // end of lazy_disable_if
  operator()(Args &&... args) const {
   return evaluator_t()(*this, std::forward<Args>(args)...);
  }

  // ------------- Call with lazy arguments -----------------------------

  // Calls with at least one lazy argument : we make a clef expression, cf clef documentation
  template <typename... Args> clef::make_expr_call_t<MAKO_GF &, Args...> operator()(Args &&... args) & {
   return clef::make_expr_call(*this, std::forward<Args>(args)...);
  }

  template <typename... Args> clef::make_expr_call_t<MAKO_GF const &, Args...> operator()(Args &&... args) const & {
   return clef::make_expr_call(*this, std::forward<Args>(args)...);
  }

  template <typename... Args> clef::make_expr_call_t<MAKO_GF, Args...> operator()(Args &&... args) && {
   return clef::make_expr_call(std::move(*this), std::forward<Args>(args)...);
  }

  // ------------- All the [] operators without lazy arguments -----------------------------

  // pass a index_t of the mesh
  decltype(auto) operator[](mesh_index_t const &arg) { return dproxy_t::invoke(_data, _mesh.index_to_linear(arg)); }
  decltype(auto) operator[](mesh_index_t const &arg) const { return dproxy_t::invoke(_data, _mesh.index_to_linear(arg)); }

  // pass a mesh_point of the mesh
  decltype(auto) operator[](mesh_point_t const &x) {
#ifdef TRIQS_DEBUG
   if (this->_mesh != x.mesh()) TRIQS_RUNTIME_ERROR << "gf[ ] : mesh point's mesh and gf's mesh mismatch";
#endif
   return dproxy_t::invoke(_data, x.linear_index());
  }

  decltype(auto) operator[](mesh_point_t const &x) const {
#ifdef TRIQS_DEBUG
   if (this->_mesh != x.mesh()) TRIQS_RUNTIME_ERROR << "gf[ ] : mesh point's mesh and gf's mesh mismatch";
#endif
   return dproxy_t::invoke(_data, x.linear_index());
  }

  // pass an abtract closest_point. We extract the value of the domain from p, call the gf_closest_point trait
  template <typename... U> decltype(auto) operator[](closest_pt_wrap<U...> const &p) {
   return dproxy_t::invoke(_data, _mesh.index_to_linear(gf_closest_point<Var, Target>::invoke(this, p)));
  }
  template <typename... U> decltype(auto) operator[](closest_pt_wrap<U...> const &p) const {
   return dproxy_t::invoke(_data, _mesh.index_to_linear(gf_closest_point<Var, Target>::invoke(this, p)));
  }

  // ------------- [] with lazy arguments -----------------------------

  template <typename Arg> clef::make_expr_subscript_t<MAKO_GF const &, Arg> operator[](Arg &&arg) const & {
   return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
  }

  template <typename Arg> clef::make_expr_subscript_t<MAKO_GF &, Arg> operator[](Arg &&arg) & {
   return clef::make_expr_subscript(*this, std::forward<Arg>(arg));
  }

  template <typename Arg> clef::make_expr_subscript_t<MAKO_GF, Arg> operator[](Arg &&arg) && {
   return clef::make_expr_subscript(std::move(*this), std::forward<Arg>(arg));
  }

  // --------------------- A direct access to the grid point --------------------------

  template <typename... Args> decltype(auto) get_from_linear_index(Args &&... args) {
   return dproxy_t::invoke(_data, linear_mesh_index_t(std::forward<Args>(args)...));
  }

  template <typename... Args> decltype(auto) get_from_linear_index(Args &&... args) const {
   return dproxy_t::invoke(_data, linear_mesh_index_t(std::forward<Args>(args)...));
  }

  template <typename... Args> decltype(auto) on_mesh(Args &&... args) {
   return dproxy_t::invoke(_data, _mesh.index_to_linear(mesh_index_t(std::forward<Args>(args)...)));
  }

  template <typename... Args> decltype(auto) on_mesh(Args &&... args) const {
   return dproxy_t::invoke(_data, _mesh.index_to_linear(mesh_index_t(std::forward<Args>(args)...)));
  }

  // --------------------- on mesh (g) : the call before [] -------------------------
  // This is a workaround the the lack of multi argument [] in C++

  // mesh points should be treated slighly differently : take their index....
  template <typename... T> decltype(auto) on_mesh(mesh_point<T> const &... args) { return on_mesh(args.index()...); }
  template <typename... T> decltype(auto) on_mesh(mesh_point<T> const &... args) const { return on_mesh(args.index()...); }

  // The on_mesh little adaptor ....
  private:
    template <typename G> struct _on_mesh_wrapper {
   G &f;
   template <typename... Args>
   auto operator()(Args &&... args) const
       -> std14::enable_if_t<!triqs::clef::is_any_lazy<Args...>::value, decltype(f.on_mesh(std::forward<Args>(args)...))> {
    return f.on_mesh(std::forward<Args>(args)...);
   }
   TRIQS_CLEF_IMPLEMENT_LAZY_CALL();
  };

  public:
  _on_mesh_wrapper<MAKO_GF const> friend on_mesh(MAKO_GF const &f) { return {f}; }
  _on_mesh_wrapper<MAKO_GF> friend on_mesh(MAKO_GF &f) { return {f}; }

  //----------------------------- HDF5 -----------------------------

  /// HDF5 name
  friend std::string get_triqs_hdf5_data_scheme(MAKO_GF const &g) {
   return "Gf" + gf_h5_name<Var, Target>::invoke();
  }

  friend struct gf_h5_rw<Var, Target>;

  /// Write into HDF5
  friend void h5_write(h5::group fg, std::string const &subgroup_name, MAKO_GF const &g) {
   auto gr = fg.create_group(subgroup_name);
   gr.write_triqs_hdf5_data_scheme(g);
   gf_h5_rw<Var, Target>::write(gr, g);
  }

  /// Read from HDF5
  friend void h5_read(h5::group fg, std::string const &subgroup_name, MAKO_GF &g) {
   auto gr = fg.open_group(subgroup_name);
   // Check the attribute or throw
   auto tag_file = gr.read_triqs_hdf5_data_scheme();
   auto tag_expected = get_triqs_hdf5_data_scheme(g);
   if (tag_file != tag_expected)
    TRIQS_RUNTIME_ERROR << "h5_read : mismatch of the tag TRIQS_HDF5_data_scheme tag in the h5 group : found " << tag_file
                        << " while I expected " << tag_expected;
   gf_h5_rw<Var, Target>::read(gr, g);
   g._remake_zero();
  }

  //-----------------------------  BOOST Serialization -----------------------------
  friend class boost::serialization::access;
  /// The serialization as required by Boost
  template <class Archive> void serialize(Archive &ar, const unsigned int version) {
   ar & _data;
   ar & _singularity;
   ar & _mesh;
   ar & _indices;
   ar & name;
  }

  //----------------------------- print  -----------------------------

  /// IO
  friend std::ostream &operator<<(std::ostream &out, MAKO_GF const &x) { return out << "MAKO_GF"; }

  //----------------------------- MPI  -----------------------------

  // mako <%def name="mpidoc(OP)">
  /**
    * Initiate (lazy) MPI ${OP}
    *
    * When the returned object is used at the RHS of operator = or in a constructor of a gf,
    * the MPI ${OP} operation is performed.
    *
    * @group MPI
    * @param g The Green function
    * @param c The MPI communicator (default is world)
    * @param root The root of the broadcast communication in the MPI sense.
    * @return Returns a lazy object describing the object and the MPI operation to be performed.
    *
    */
  // mako </%def>

  // mako ${mpidoc("Bcast")}
  friend void mpi_broadcast(MAKO_GF &g, mpi::communicator c = {}, int root = 0) {
   // Shall we bcast mesh ?
   mpi_broadcast(g.data(), c, root);
   mpi_broadcast(g.singularity(), c, root);
  }

  // mako ${mpidoc("Reduce")}
  friend mpi_lazy<mpi::tag::reduce, const_view_type> mpi_reduce(MAKO_GF const &a, mpi::communicator c = {}, int root = 0,
                                                                bool all = false, MPI_Op op = MPI_SUM) {
   return {a(), c, root, all, op};
  }

  // mako ${mpidoc("AllReduce")}
  friend mpi_lazy<mpi::tag::reduce, const_view_type> mpi_all_reduce(MAKO_GF const &a, mpi::communicator c = {}, int root = 0,
                                                                    MPI_Op op = MPI_SUM) {
   return {a(), c, root, true, op};
  }

  // mako ${mpidoc("Scatter")}
  friend mpi_lazy<mpi::tag::scatter, const_view_type> mpi_scatter(MAKO_GF const &a, mpi::communicator c = {}, int root = 0) {
   return {a(), c, root, true};
  }

  // mako ${mpidoc("Gather")}
  friend mpi_lazy<mpi::tag::gather, const_view_type> mpi_gather(MAKO_GF const &a, mpi::communicator c = {}, int root = 0,
                                                                bool all = false) {
   return {a(), c, root, all};
  }

  // mako ${mpidoc("AllGather")}
  friend mpi_lazy<mpi::tag::gather, const_view_type> mpi_all_gather(MAKO_GF const &a, mpi::communicator c = {}, int root = 0) {
   return {a(), c, root, true};
  }

  // mako %if RVC != "const_view":

  //-------------  corresponding operator = overload

  /**
   * Performs MPI reduce
   * @param l The lazy object returned by mpi_reduce
   */
  void operator=(mpi_lazy<mpi::tag::reduce, gf_const_view<Var, Target>> l) {
   _mesh = l.rhs.mesh();
   _data = arrays::mpi_reduce(l.rhs.data(), l.c, l.root, l.all, l.op);// arrays:: necessary on gcc 5. why ??
   // mako %if GRV == "regular" :
   _remake_zero();
   // mako %endif
   _singularity = mpi_reduce(l.rhs.singularity(), l.c, l.root, l.all, l.op);
  }

  /**
    * Performs MPI scatter
    * @param l The lazy object returned by mpi_reduce
    */
  void operator=(mpi_lazy<mpi::tag::scatter, gf_const_view<Var, Target>> l) {
   _mesh = mpi_scatter(l.rhs.mesh(), l.c, l.root);
   _data = mpi_scatter(l.rhs.data(), l.c, l.root, true);
   // mako %if GRV == "regular" :
   _remake_zero();
   // mako %endif
   if (l.c.rank() == l.root) _singularity = l.rhs.singularity();
   mpi_broadcast(_singularity, l.c, l.root);
  }

  /**
    * Performs MPI gather
    * @param l The lazy object returned by mpi_reduce
    */
  void operator=(mpi_lazy<mpi::tag::gather, gf_const_view<Var, Target>> l) {
   _mesh = mpi_gather(l.rhs.mesh(), l.c, l.root);
   _data = mpi_gather(l.rhs.data(), l.c, l.root, l.all);
   // mako %if GRV == "regular" :
   _remake_zero();
   // mako %endif
   if (l.all || (l.c.rank() == l.root)) _singularity = l.rhs.singularity();
  }
  // mako %endif
 };
// mako %endfor
// mako %endfor

 /*------------------------------------------------------------------------------------------------------
  *                                     View  assignment
  *-----------------------------------------------------------------------------------------------------*/

#ifdef __cpp_if_constexpr
 template <typename M, typename T, typename RHS> void triqs_gf_view_assign_delegation(gf_view<M, T> g, RHS const &rhs) {
  if constexpr(arrays::is_scalar_v<RHS>) {
    for (auto const &w : g.mesh()) g[w] = rhs;
    g.singularity() = rhs;
   }
  else {
   if (!(g.mesh() == rhs.mesh()))
    TRIQS_RUNTIME_ERROR << "Gf Assignment in View : incompatible mesh" << g.mesh() << " vs " << rhs.mesh();
   for (auto const &w : g.mesh()) g[w] = rhs[w];
   g.singularity() = rhs.singularity();
  }
 }
#else
 // delegate = so that I can overload it for specific RHS...
 template <typename M, typename T, typename RHS>
 std14::enable_if_t<!arrays::is_scalar<RHS>::value> triqs_gf_view_assign_delegation(gf_view<M, T> g, RHS const &rhs) {
  if (!(g.mesh() == rhs.mesh()))
   TRIQS_RUNTIME_ERROR << "Gf Assignment in View : incompatible mesh" << g.mesh() << " vs " << rhs.mesh();
  for (auto const &w : g.mesh()) g[w] = rhs[w];
  g.singularity() = rhs.singularity();
 }

 template <typename M, typename T, typename RHS>
 std14::enable_if_t<arrays::is_scalar<RHS>::value> triqs_gf_view_assign_delegation(gf_view<M, T> g, RHS const &rhs) {
  for (auto const &w : g.mesh()) g[w] = rhs;
  g.singularity() = rhs;
 }
#endif

}
}
/*------------------------------------------------------------------------------------------------------
 *             Delete std::swap for views, as for arrays
 *-----------------------------------------------------------------------------------------------------*/
namespace std {
template <typename Var, typename Target>
void swap(triqs::gfs::gf_view<Var, Target> &a, triqs::gfs::gf_view<Var, Target> &b) = delete;
}

