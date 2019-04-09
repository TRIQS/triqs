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
//#include <triqs/utility/lazy_bracket.hpp>
#include "./defs.hpp"
#include "./gf_indices.hpp"
#include "./data_proxy.hpp"

namespace triqs {
  namespace gfs {

    // forward
    namespace details {
      template <typename Mesh, typename... A> struct is_ok;
      template <typename G, typename... Args> decltype(auto) partial_eval(G *g, Args const &... args);
    } // namespace details

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

    template <typename M, typename T> struct is_gf<gf_const_view<M, T>, void> : std::true_type {};

    template <typename M, typename T> struct is_gf<gf_const_view<M, T>, M> : std::true_type {};

    template <typename G, typename M> struct is_gf<G &, M> : is_gf<G, M> {};
    template <typename G, typename M> struct is_gf<G const &, M> : is_gf<G, M> {};
    template <typename G, typename M> struct is_gf<G &&, M> : is_gf<G, M> {};

    template <typename G, typename M> inline constexpr bool  is_gf_v = is_gf<G, M>::value;

    /// ---------------------------  implementation  ---------------------------------

    namespace details {
      // FIXME : replace by if constexpr when possible
      template <typename A> void _rebind_helper(A &x, A const &y) { x.rebind(y); }
      inline void _rebind_helper(dcomplex &x, dcomplex const &y) { x = y; }
      inline void _rebind_helper(double &x, double const &y) { x = y; }
    } // namespace details
    struct impl_tag {};
    struct impl_tag2 {};

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
    class MAKO_GF :
       // mako %if 'view' in RVC:
       is_view_tag,
       // mako %endif
       TRIQS_CONCEPT_TAG_NAME(ImmutableGreenFunction) {

      public:
      static constexpr bool is_view  = MAKO_ISVIEW;
      static constexpr bool is_const = MAKO_ISCONST;

      using mutable_view_type = gf_view<Var, Target>;

      /// The associated const view type (gf_const_view <....>)
      using const_view_type = gf_const_view<Var, Target>;

      /// The associated view type (non const)
      using view_type = MAKO_GV<Var, Target>;

      /// The associated regular type (gf<....>)
      using regular_type = gf<Var, Target>;

      /// The associated real type
      using real_t = MAKO_GF<Var, typename Target::real_t>;

      using variable_t = Var;
      using target_t   = Target;

      /// Type of the mesh
      using mesh_t                   = gf_mesh<Var>;
      using domain_t                 = typename mesh_t::domain_t;
      static constexpr int arity     = get_n_variables<Var>::value;
      static constexpr int data_rank = arity + Target::rank;

      using mesh_point_t        = typename mesh_t::mesh_point_t;
      using mesh_index_t        = typename mesh_t::index_t;
      using linear_mesh_index_t = typename mesh_t::linear_index_t;
      using indices_t           = gf_indices;
      using evaluator_t         = gf_evaluator<Var, Target>;

      using scalar_t = typename Target::scalar_t;

      using data_regular_t    = arrays::array<scalar_t, data_rank>;
      using data_view_t       = typename data_regular_t::view_type;
      using data_const_view_t = typename data_regular_t::const_view_type;
      using data_t            = data_MAKO_RVC_t;

      using data_memory_layout_t = memory_layout_t<data_rank>;

      using zero_regular_t    = std14::conditional_t<Target::rank != 0, arrays::array<scalar_t, Target::rank>, scalar_t>;
      using zero_const_view_t = std14::conditional_t<Target::rank != 0, arrays::array_const_view<scalar_t, Target::rank>, scalar_t>;
      using zero_view_t       = zero_const_view_t;
      using zero_t            = zero_MAKO_RVC_t;

      // FIXME : std::array with NDA
      using target_shape_t = arrays::mini_vector<int, Target::rank>;

      struct target_and_shape_t {
        target_shape_t _shape;
        using target_t = Target;
        target_shape_t const &shape() const { return _shape; }
      };

      // ------------- Accessors -----------------------------

      /// Access the  mesh
      mesh_t const &mesh() const { return _mesh; }
      domain_t const &domain() const { return _mesh.domain(); }

      /// Direct access to the data array
      data_t &data() & { return _data; }

      /// Const version
      data_t const &data() const & { return _data; }

      /// Move data in case of rvalue gf
      data_t data() && { return std::move(_data); }

      /// Shape of the target
      //auto target_shape() const { return _data.shape().template front_mpop<arity>(); } // drop arity dims
      target_and_shape_t target() const { return target_and_shape_t{_data.shape().template front_mpop<arity>()}; } // drop arity dims

      auto target_shape() const { return target().shape(); } // drop arity dims

      auto target_indices() const { return itertools::product_range(target().shape()); }

      /// Shape of the data
      auto const &data_shape() const { return _data.shape(); }

      /// Memorylayout of the data
      auto const &memory_layout() const { return _data.indexmap().memory_layout(); }

      ///
      zero_t const &get_zero() const { return _zero; }

      indices_t const &indices() const { return _indices; }

      private:
      mesh_t _mesh;
      data_t _data;
      zero_t _zero;
      indices_t _indices;

      private:
      using dproxy_t = details::_data_proxy<Target>;

      // -------------------------------- impl. details common to all classes -----------------------------------------------

      // for delegation only
      private:
      // build a zero from a slice of data
      // MUST be static since it is used in constructors... (otherwise bug in clang)
      template <typename T> static zero_t __make_zero(T, data_t const &d) {
        auto r = zero_regular_t{d.shape().template front_mpop<arity>()};
        r()    = 0;
        return r;
      }
      static zero_t __make_zero(scalar_valued, data_t const &d) { return 0; }      // special case
      static zero_t __make_zero(scalar_real_valued, data_t const &d) { return 0; } // special case
      static zero_t _make_zero(data_t const &d) { return __make_zero(Target{}, d); }
      zero_t _remake_zero() { return _zero = _make_zero(_data); } // NOT in constructor...

      template <typename G> MAKO_GF(impl_tag2, G &&x) : _mesh(x.mesh()), _data(x.data()), _zero(_make_zero(_data)), _indices(x.indices()) {}

      template <typename M, typename D>
      MAKO_GF(impl_tag, M &&m, D &&dat, indices_t ind)
         : _mesh(std::forward<M>(m)), _data(std::forward<D>(dat)), _zero(_make_zero(_data)), _indices(std::move(ind)) {
        if (!(_indices.empty() or _indices.has_shape(target_shape()))) TRIQS_RUNTIME_ERROR << "Size of indices mismatch with data size";
      }

      public:
      // mako %if RVC == 'regular' :

      /// Construct an empty Green function (with empty array).
      gf() {} // all arrays of zero size (empty)

      // mako %endif

      /// Copy constructor
      MAKO_GF(MAKO_GF const &x) = default;
      /// Move constructor
      MAKO_GF(MAKO_GF &&) = default;

      void swap_impl(MAKO_GF &b) noexcept {
        using std::swap;
        swap(this->_mesh, b._mesh);
        swap(this->_data, b._data);
        swap(this->_zero, b._zero);
        swap(this->_indices, b._indices);
      }

      // mako ## ------------------------------------------------  regular class ---------------------------------
      // mako %if RVC == 'regular' :

      private:
      // FIXME : simplify
      template <typename U> static auto make_data_shape(U, mesh_t const &m, target_shape_t const &shap) { return join(m.size_of_components(), shap); }

      public:
      // Construct from the data. Using the "pass by value" pattern + move
      gf(mesh_t m, data_t dat, indices_t ind) : gf(impl_tag{}, std::move(m), std::move(dat), std::move(ind)) {}

      // Construct from the data. Using the "pass by value" pattern + move
      gf(mesh_t m, data_t dat, arrays::memory_layout_t<arity + Target::rank> const &ml, indices_t ind)
         : gf(impl_tag{}, std::move(m), data_t(dat, ml), std::move(ind)) {}

      // Construct from mesh, target_shape, memory order
      gf(mesh_t m, target_shape_t shape, arrays::memory_layout_t<arity + Target::rank> const &ml, indices_t const &ind = indices_t{})
         : gf(impl_tag{}, std::move(m), data_t(make_data_shape(Target{}, m, shape), ml), ind) {
        if (this->_indices.empty()) this->_indices = indices_t(shape);
      }

      // Construct from mesh, target_shape, memory order
      gf(mesh_t m, target_shape_t shape = target_shape_t{}, indices_t const &ind = indices_t{})
         : gf(impl_tag{}, std::move(m), data_t(make_data_shape(Target{}, m, shape)), ind) {
        if (this->_indices.empty()) this->_indices = indices_t(shape);
      }

      /// From a gf_view of the same kind
      gf(gf_view<Var, Target> const &g) : gf(impl_tag2{}, g) {}

      /// From a const_gf_view of the same kind
      gf(gf_const_view<Var, Target> const &g) : gf(impl_tag2{}, g) {}

      /// Construct from anything which models ImmutableGreenFunction.
      // TODO: We would like to refine this, G should have the same mesh, target, at least ...
      template <typename G> gf(G const &x, std14::enable_if_t<ImmutableGreenFunction<G>::value> *dummy = 0) : gf() { *this = x; }

      /// Construct from the mpi lazy class of the implementation class, cf mpi section
      // NB : type must be the same, e.g. g2(reduce(g1)) will work only if mesh, Target, Singularity are the same...
      template <typename Tag> gf(mpi_lazy<Tag, gf_const_view<Var, Target>> x) : gf() { operator=(x); }

      /// ---------------  swap --------------------

      /// implement the swap
      friend void swap(gf &a, gf &b) noexcept { a.swap_impl(b); }

      /// ---------------  Operator = --------------------

      /// Copy assignment
      gf &operator=(gf &rhs) = default;
      gf &operator=(gf const &rhs) = default;

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
        _indices = rhs.indices();
        if (_indices.empty()) _indices = indices_t(target_shape());
        //if (not _indices.has_shape(target_shape())) _indices = indices_t(target_shape());
        // to be implemented : there is none in the gf_expr in particular....
        return *this;
      }

      // other = late, cf MPI

      // mako ## ------------------------------------------------  view class ---------------------------------
      // mako %elif RVC == 'const_view' :

      // ---------------  Constructors --------------------

      //
      gf_const_view() = default;

      /// Makes a const view
      gf_const_view(gf_view<Var, Target> const &g) : gf_const_view(impl_tag2{}, g) {}

      /// Makes a const view
      gf_const_view(gf<Var, Target> const &g) : gf_const_view(impl_tag2{}, g) {}

      /// Makes a const view
      gf_const_view(gf<Var, Target> &g) : gf_const_view(impl_tag2{}, g) {} // from a gf &

      /// Makes a const view
      gf_const_view(gf<Var, Target> &&g) noexcept : gf_const_view(impl_tag2{}, std::move(g)) {} // from a gf &&

      // Construct from mesh, data, ....
      template <typename D> gf_const_view(mesh_t m, D const &dat, indices_t const &ind) : gf_const_view(impl_tag{}, std::move(m), dat, ind) {}

      // Construct from mesh, data.
      template <typename D> gf_const_view(mesh_t m, D const &dat) : gf_const_view(impl_tag{}, std::move(m), dat, {}) {}

      // ---------------  swap --------------------

      /// Swap
      friend void swap(gf_const_view &a, gf_const_view &b) noexcept { a.swap_impl(b); }

      // ---------------  Rebind --------------------
      /// Rebind the view
      void rebind(gf_const_view<Var, Target> const &X) noexcept {
        this->_mesh = X._mesh;
        this->_data.rebind(X._data);
        details::_rebind_helper(_zero, X._zero);
        this->_indices = X._indices;
      }

      /// Rebind on a non const view
      void rebind(gf_view<Var, Target> const &X) noexcept { rebind(gf_const_view{X}); }

      // ---------------  No = since it is const ... --------------------
      gf_const_view &operator=(gf_const_view const &) = delete; // a const view can not be assigned to

      // mako ## ------------------------------------------------  view class ---------------------------------
      // mako %elif RVC == 'view' :

      public:
      // ---------------  Constructors --------------------

      //
      gf_view() = default;

      // Allow to construct a view from a gf with a different evaluator, except const_views ...
      /// Makes a view
      gf_view(gf_const_view<Var, Target> const &g) = delete;

      /// Makes a view
      gf_view(gf<Var, Target> const &g) = delete;

      /// Makes a view
      gf_view(gf<Var, Target> &g) : gf_view(impl_tag2{}, g) {}

      /// Makes a view
      gf_view(gf<Var, Target> &&g) noexcept : gf_view(impl_tag2{}, std::move(g)) {} // from a gf &&

      /// Construct from mesh, data, ....
      template <typename D>
      gf_view(mesh_t m, D &&dat, indices_t const &ind = indices_t{}) : gf_view(impl_tag{}, std::move(m), std::forward<D>(dat), ind) {}

      // Construct from mesh, data. Only for partial_eval
      template <typename D> gf_view(mesh_t m, D const &dat) : gf_view(impl_tag{}, std::move(m), dat, {}) {}

      // ---------------  swap --------------------
      /// Swap
      friend void swap(gf_view &a, gf_view &b) noexcept { a.swap_impl(b); }

      // ---------------  Rebind --------------------
      /// Rebind
      void rebind(gf_view<Var, Target> const &X) noexcept {
        this->_mesh = X._mesh;
        this->_data.rebind(X._data);
        details::_rebind_helper(_zero, X._zero);
        this->_indices = X._indices;
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
      template <typename Fdata, typename Find> auto apply_on_data(Fdata &&fd, Find &&fi) MAKO_C {
        auto d2    = fd(_data);
        using t2   = target_from_array<decltype(d2), arity>;
        using gv_t = MAKO_GFVIEW<Var, t2>;
        return gv_t{_mesh, d2, fi(_indices)};
      }

      template <typename Fdata> auto apply_on_data(Fdata &&fd) MAKO_C {
        return apply_on_data(std::forward<Fdata>(fd), [](auto &) { return indices_t{}; });
      }
      // mako %endfor

      // ------------- All the call operators arguments -----------------------------

      template <typename... Args> decltype(auto) operator()(Args &&... args) const & {
        if constexpr (sizeof...(Args) == 0)
          return const_view_type{*this};
        else {
          static_assert((sizeof...(Args) == evaluator_t::arity) or (evaluator_t::arity == -1), "Incorrect number of arguments");
          if constexpr ((... or clef::is_any_lazy<Args>::value)) // any argument is lazy ?
            return clef::make_expr_call(*this, std::forward<Args>(args)...);
          else
            return evaluator_t()(*this, std::forward<Args>(args)...);
        }
      }
      template <typename... Args> decltype(auto) operator()(Args &&... args) & {
        if constexpr (sizeof...(Args) == 0)
          return view_type{*this};
        else {
          static_assert((sizeof...(Args) == evaluator_t::arity) or (evaluator_t::arity == -1), "Incorrect number of arguments");
          if constexpr ((... or clef::is_any_lazy<Args>::value)) // any argument is lazy ?
            return clef::make_expr_call(*this, std::forward<Args>(args)...);
          else
            return evaluator_t()(*this, std::forward<Args>(args)...);
        }
      }
      template <typename... Args> decltype(auto) operator()(Args &&... args) && {
        if constexpr (sizeof...(Args) == 0)
          return view_type{std::move(*this)};
        else {
          static_assert((sizeof...(Args) == evaluator_t::arity) or (evaluator_t::arity == -1), "Incorrect number of arguments");
          if constexpr ((... or clef::is_any_lazy<Args>::value)) // any argument is lazy ?
            return clef::make_expr_call(std::move(*this), std::forward<Args>(args)...);
          else
            return evaluator_t()(std::move(*this), std::forward<Args>(args)...);
        }
      }

      // ------------- All the [] operators without lazy arguments -----------------------------

      // pass a index_t of the mesh
      decltype(auto) operator[](mesh_index_t const &arg) { return dproxy_t::invoke(_data, _mesh.index_to_linear(arg)); }
      decltype(auto) operator[](mesh_index_t const &arg) const { return dproxy_t::invoke(_data, _mesh.index_to_linear(arg)); }

      // pass a mesh_point of the mesh
      decltype(auto) operator[](mesh_point_t const &x) {
#ifdef TRIQS_DEBUG
        if (!mesh_point_compatible_to_mesh(x, _mesh)) TRIQS_RUNTIME_ERROR << "gf[ ] : mesh point's mesh and gf's mesh mismatch";
#endif
        return dproxy_t::invoke(_data, x.linear_index());
      }

      decltype(auto) operator[](mesh_point_t const &x) const {
#ifdef TRIQS_DEBUG
        if (!mesh_point_compatible_to_mesh(x, _mesh)) TRIQS_RUNTIME_ERROR << "gf[ ] : mesh point's mesh and gf's mesh mismatch";
#endif
        return dproxy_t::invoke(_data, x.linear_index());
      }

      // pass an abtract closest_point. We extract the value of the domain from p, call the gf_closest_point trait
      template <typename... U> decltype(auto) operator[](closest_pt_wrap<U...> const &p) {
        return dproxy_t::invoke(_data, _mesh.index_to_linear(gf_closest_point<Var, Target>::invoke(this->mesh(), p)));
      }
      template <typename... U> decltype(auto) operator[](closest_pt_wrap<U...> const &p) const {
        return dproxy_t::invoke(_data, _mesh.index_to_linear(gf_closest_point<Var, Target>::invoke(this->mesh(), p)));
      }

      // -------------- operator [] with tuple_com. Distinguich the lazy and non lazy case
      public:
      template <typename... U> decltype(auto) operator[](tuple_com<U...> const &tu) & {
        static_assert(sizeof...(U) == get_n_variables<Var>::value, "Incorrect number of argument in [] operator");
        if constexpr ((... or clef::is_any_lazy<U>::value)) // any argument is lazy ?
          return clef::make_expr_subscript(*this, tu);
        else {
          static_assert(details::is_ok<mesh_t, U...>::value, "Argument type incorrect");
          auto l = [this](auto &&... y) -> decltype(auto) { return details::partial_eval(this, y...); };
          return triqs::tuple::apply(l, tu._t);
        }
      }

      template <typename... U> decltype(auto) operator[](tuple_com<U...> const &tu) const & {
        static_assert(sizeof...(U) == get_n_variables<Var>::value, "Incorrect number of argument in [] operator");
        if constexpr ((... or clef::is_any_lazy<U>::value)) // any argument is lazy ?
          return clef::make_expr_subscript(*this, tu);
        else {
          static_assert(details::is_ok<mesh_t, U...>::value, "Argument type incorrect");
          auto l = [this](auto &&... y) -> decltype(auto) { return details::partial_eval(this, y...); };
          return triqs::tuple::apply(l, tu._t);
        }
      }

      template <typename... U> decltype(auto) operator[](tuple_com<U...> const &tu) && {
        static_assert(sizeof...(U) == get_n_variables<Var>::value, "Incorrect number of argument in [] operator");
        if constexpr ((... or clef::is_any_lazy<U>::value)) // any argument is lazy ?
          return clef::make_expr_subscript(std::move(*this), tu);
        else {
          static_assert(details::is_ok<mesh_t, U...>::value, "Argument type incorrect");
          auto l = [this](auto &&... y) -> decltype(auto) { return details::partial_eval(this, y...); };
          return triqs::tuple::apply(l, tu._t);
        }
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

      //----------------------------- HDF5 -----------------------------

      /// HDF5 name
      static std::string hdf5_scheme() { return "Gf"; }

      friend struct gf_h5_rw<Var, Target>;

      /// Write into HDF5
      friend void h5_write(h5::group fg, std::string const &subgroup_name, MAKO_GF const &g) {
        auto gr = fg.create_group(subgroup_name);
        gr.write_hdf5_scheme(g);
        gf_h5_rw<Var, Target>::write(gr, g);
      }

      /// Read from HDF5
      friend void h5_read(h5::group fg, std::string const &subgroup_name, MAKO_GF &g) {
        auto gr       = fg.open_group(subgroup_name);
        auto tag_file = gr.read_hdf5_scheme();
        if (!(tag_file[0] == 'G' and tag_file[1] == 'f'))
          TRIQS_RUNTIME_ERROR << "h5_read : For a Green function, the type tag should be Gf (or Gfxxxx for old archive) "
                              << " while I found " << tag_file;
        gf_h5_rw<Var, Target>::read(gr, g);
        g._remake_zero();
      }

      //-----------------------------  BOOST Serialization -----------------------------
      friend class boost::serialization::access;
      /// The serialization as required by Boost
      template <class Archive> void serialize(Archive &ar, const unsigned int version) {
        ar &_data;
        ar &_mesh;
        ar &_indices;
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
	mpi::broadcast(g.data(), c, root);
      }

      // mako ${mpidoc("Reduce")}
      friend mpi_lazy<mpi::tag::reduce, const_view_type> mpi_reduce(MAKO_GF const &a, mpi::communicator c = {}, int root = 0, bool all = false,
                                                                    MPI_Op op = MPI_SUM) {
        return {a(), c, root, all, op};
      }

      // mako ${mpidoc("Scatter")}
      friend mpi_lazy<mpi::tag::scatter, const_view_type> mpi_scatter(MAKO_GF const &a, mpi::communicator c = {}, int root = 0) {
        return {a(), c, root, true};
      }

      // mako ${mpidoc("Gather")}
      friend mpi_lazy<mpi::tag::gather, const_view_type> mpi_gather(MAKO_GF const &a, mpi::communicator c = {}, int root = 0, bool all = false) {
        return {a(), c, root, all};
      }

      // mako %if RVC != "const_view":

      //-------------  corresponding operator = overload

      /**
    * Performs MPI reduce
    * @param l The lazy object returned by mpi::reduce
    */
      void operator=(mpi_lazy<mpi::tag::reduce, gf_const_view<Var, Target>> l) {
        _mesh = l.rhs.mesh();
        _data = mpi::reduce(l.rhs.data(), l.c, l.root, l.all, l.op); // arrays:: necessary on gcc 5. why ??
        // mako %if GRV == "regular" :
        _remake_zero();
        // mako %endif
      }

      /**
     * Performs MPI scatter
     * @param l The lazy object returned by reduce
     */
      void operator=(mpi_lazy<mpi::tag::scatter, gf_const_view<Var, Target>> l) {
        _mesh = mpi::scatter(l.rhs.mesh(), l.c, l.root);
        _data = mpi::scatter(l.rhs.data(), l.c, l.root, true);
        // mako %if GRV == "regular" :
        _remake_zero();
        // mako %endif
      }

      /**
     * Performs MPI gather
     * @param l The lazy object returned by mpi::reduce
     */
      void operator=(mpi_lazy<mpi::tag::gather, gf_const_view<Var, Target>> l) {
        _mesh = mpi::gather(l.rhs.mesh(), l.c, l.root);
        _data = mpi::gather(l.rhs.data(), l.c, l.root, l.all);
        // mako %if GRV == "regular" :
        _remake_zero();
        // mako %endif
      }
      // mako %endif
    };
    // mako %endfor
    // mako %endfor

    /*------------------------------------------------------------------------------------------------------
 *                                     View  assignment
 *-----------------------------------------------------------------------------------------------------*/

    template <typename M, typename T, typename RHS> void triqs_gf_view_assign_delegation(gf_view<M, T> g, RHS const &rhs) {
      if constexpr (arrays::is_scalar<RHS>::value) {
        for (auto const &w : g.mesh()) g[w] = rhs;
      } else {
        if (!(g.mesh() == rhs.mesh())) TRIQS_RUNTIME_ERROR << "Gf Assignment in View : incompatible mesh \n" << g.mesh() << "\n vs \n" << rhs.mesh();
        for (auto const &w : g.mesh()) g[w] = rhs[w];
      }
    }
  } // namespace gfs
} // namespace triqs
/*------------------------------------------------------------------------------------------------------
 *             Delete std::swap for views, as for arrays
 *-----------------------------------------------------------------------------------------------------*/
namespace std {
  template <typename Var, typename Target> void swap(triqs::gfs::gf_view<Var, Target> &a, triqs::gfs::gf_view<Var, Target> &b) = delete;
}
