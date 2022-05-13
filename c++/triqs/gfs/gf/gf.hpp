// Copyright (c) 2019-2020 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once
#include "./defs.hpp"
#include "./gf_indices.hpp"
#include "./comma.hpp"

namespace triqs::gfs {

  // forward
  namespace details {
    template <typename Mesh, typename... A> struct is_ok;
    template <typename G, typename... Args> decltype(auto) slice_or_access_general(G &g, Args const &... args);
  } // namespace details

  /*------------------------------------------------------------------------
   *   Forward Declaration of the main types : gf, gf_view, gf_const_view
   *-----------------------------------------------------------------------*/
  using nda::C_layout;

  template <typename Mesh, typename Target = matrix_valued, typename Layout = nda::C_layout, typename EvalPolicy = default_evaluator> class gf;
  template <typename Mesh, typename Target = matrix_valued, typename Layout = nda::C_stride_layout, typename EvalPolicy = default_evaluator> class gf_view;
  template <typename Mesh, typename Target = matrix_valued, typename Layout = nda::C_stride_layout, typename EvalPolicy = default_evaluator> class gf_const_view;

  /*----------------------------------------------------------
   *   Traits
   *--------------------------------------------------------*/

  // The trait that "marks" the Green function
  TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT(GreenFunction);

  template <template <typename...> class TMPLT, typename T> struct is_instantiation_of : std::false_type {};
  template <template <typename...> class TMPLT, typename... U> struct is_instantiation_of<TMPLT, TMPLT<U...>> : std::true_type {};
  template <template <typename...> class gf, typename T>
  inline constexpr bool is_instantiation_of_v = is_instantiation_of<gf, std::decay_t<T>>::value;

  //template <typename G> using mesh_t_of = std::decay_t<decltype(std::declval<G>().mesh())>;

  // is_gf<G> Check if G fullfills the Green function concecpt
  template <typename G, typename M = void> inline constexpr bool is_gf_v = false;
  template <typename G>
  inline constexpr bool is_gf_v<G, void> =
     is_instantiation_of_v<gf, G> or is_instantiation_of_v<gf_view, G> or is_instantiation_of_v<gf_const_view, G>;
  template <typename G> inline constexpr bool is_gf_v<G, typename std::decay_t<G>::mesh_t> = is_gf_v<G, void>;

  /// ---------------------------  tags for some implementation details  ---------------------------------

  struct impl_tag {};
  struct impl_tag2 {};

  // ----------------------  gf -----------------------------------------
  /**
   * The Green function container. 
   *
   * @tparam Mesh      The domain of definition
   * @tparam Target   The target domain
   *
   * @include triqs/gfs.hpp
   */
  template <typename Mesh, typename Target, typename Layout, typename EvalPolicy> class gf : TRIQS_CONCEPT_TAG_NAME(GreenFunction) {

    static_assert(not std::is_same_v<Mesh, triqs::lattice::brillouin_zone>,
                  "Since TRIQS 2.3, brillouin_zone is replaced by mesh::brzone as a mesh name. Cf Doc, changelog");

    using this_t = gf<Mesh, Target, Layout, EvalPolicy>; // used in common code

    public:
    static constexpr bool is_view  = false;
    static constexpr bool is_const = false;

    using mutable_view_type = gf_view<Mesh, Target, typename Layout::with_lowest_guarantee_t, EvalPolicy>;

    /// Associated const view type
    using const_view_type = gf_const_view<Mesh, Target, typename Layout::with_lowest_guarantee_t, EvalPolicy>;

    /// Associated (non const) view type
    using view_type = gf_view<Mesh, Target, typename Layout::with_lowest_guarantee_t, EvalPolicy>;

    /// Associated regular type (gf<....>)
    using regular_type = gf<Mesh, Target, Layout, EvalPolicy>;

    /// The associated real type
    using real_t = gf<Mesh, typename Target::real_t, Layout, EvalPolicy>;

    /// Template type
    using target_t = Target;

    /// Mesh type
    using mesh_t = Mesh;

    /// Domain type
    using domain_t = typename mesh_t::domain_t;

    /// Type of the mesh point
    using mesh_point_t = typename mesh_t::mesh_point_t;

    // NO DOC
    using mesh_index_t        = typename mesh_t::index_t;
    using linear_mesh_index_t = typename mesh_t::linear_index_t;

    using indices_t   = gf_indices;
    using evaluator_t = typename EvalPolicy::template evaluator_t<Mesh, Target>;

    /// Real or Complex
    using scalar_t = typename Target::scalar_t;

    /// Arity of the function (number of variables)
    static constexpr int arity = get_n_variables<Mesh>::value;

    /// Rank of the data array representing the function
    static constexpr int data_rank = arity + Target::rank;

    /// Type of the data array
    using data_t = nda::basic_array<scalar_t, data_rank, Layout, 'A', nda::heap<>>;

    // FIXME : std::array with NDA
    using target_shape_t = std::array<long, Target::rank>;

    struct target_and_shape_t {
      target_shape_t _shape;
      using target_t = Target;
      target_shape_t const &shape() const { return _shape; }
    };

    // ------------- Accessors -----------------------------

    /// Access the  mesh
    mesh_t const &mesh() const { return _mesh; }

    /// Access the domain of the mesh
    domain_t const &domain() const { return _mesh.domain(); }

    // DOC : fix data type here array<scalar_t, data_rank> to avoid multiply type in visible part

    /**
     * Data array
     *
     * @category Accessors
     */
    data_t &data() & { return _data; }

    /**
     * Data array (const)
     *
     * @category Accessors
     */
    data_t const &data() const & { return _data; }

    /**
     * Data array : move data in case of rvalue
     *
     * @category Accessors
     */
    data_t data() && { return std::move(_data); }

    /**
     * Shape of the data
     *
     * NB : Needed for generic code. Expression of gf (e.g. g1 + g2) have a data_shape, but not data
     * @category Accessors
     */
    auto const &data_shape() const { return _data.shape(); }

    // FIXME : No doc : internal only ? for make_gf
    target_and_shape_t target() const { return target_and_shape_t{stdutil::front_mpop<arity>(_data.shape())}; } // drop arity dims

    /**
     * Shape of the target
     *
     * @category Accessors
     */
    std::array<long, Target::rank> target_shape() const { return target().shape(); } // drop arity dims

    /**
     * Generator for the indices of the target space
     *
     * @category Accessors
     */
    auto target_indices() const { return itertools::product_range(target().shape()); }

    /// Indices of the Green function (for Python only)
    indices_t const &indices() const { return _indices; }

    private:
    mesh_t _mesh;
    data_t _data;
    indices_t _indices;

    // -------------------------------- impl. details common to all classes -----------------------------------------------

    private:
    template <typename G> gf(impl_tag2, G &&x) : _mesh(x.mesh()), _data(x.data()), _indices(x.indices()) {}

    template <typename M, typename D>
    gf(impl_tag, M &&m, D &&dat, indices_t ind) : _mesh(std::forward<M>(m)), _data(std::forward<D>(dat)), _indices(std::move(ind)) {
      if (!(_indices.empty() or _indices.has_shape(target_shape()))) TRIQS_RUNTIME_ERROR << "Size of indices mismatch with data size";
    }

    public:
    /// Empty Green function (with empty array).
    gf() {}

    ///
    gf(gf const &x) = default;

    ///
    gf(gf &&) = default;

    private:
    void swap_impl(gf &b) noexcept {
      using std::swap;
      swap(this->_mesh, b._mesh);
      swap(this->_data, b._data);
      swap(this->_indices, b._indices);
    }

    private:
    // FIXME : simplify
    template <typename U> static auto make_data_shape(U, mesh_t const &m, target_shape_t const &shap) {
      if constexpr (mesh::is_product_v<mesh_t>)
        return stdutil::join(m.size_of_components(), shap);
      else
        return stdutil::front_append(shap, m.size()); 
    }

    public:

    /**
     *  @param m Mesh
     *  @param dat data arrray
     *  @param ind Indices
     *
     *  @note  Using the "pass by value" and move
     *  @example triqs/gfs/gf_constructors_0.cpp
     */
    gf(mesh_t m, data_t dat, indices_t ind) : gf(impl_tag{}, std::move(m), std::move(dat), std::move(ind)) {}

    /**
     *  @param m Mesh
     *  @param shape Target shape
     *  @param ind Indices
     * 
     */
    gf(mesh_t m, target_shape_t shape = target_shape_t{}, indices_t const &ind = indices_t{})
       : gf(impl_tag{}, std::move(m), data_t(make_data_shape(Target{}, m, shape)), ind) {
      if (this->_indices.empty()) this->_indices = indices_t(shape);
    }

    /**
     *  Makes a deep copy of the data
     */
    explicit gf(gf_view<Mesh, Target> const &g) : gf(impl_tag2{}, g) {}

    /**
     *  Makes a deep copy of the data
     */
    explicit gf(gf_const_view<Mesh, Target> const &g) : gf(impl_tag2{}, g) {}

    /** 
     *  From any object modeling the :ref:`concept_GreenFunction`.
     * 
     *  @tparam G A type modeling :ref:`concept_GreenFunction`.
     *  @param g 
     */
    template <typename G> explicit gf(G const &g) requires(GreenFunction<G>::value) : gf() { *this = g; } // explicit is very important here.	
    // TODO: We would like to refine this, G should have the same mesh, target, at least ...

    /** 
     *  from the mpi lazy operation. Cf MPI section !
     * 
     *  @tparam Tag
     *  @param l The lazy object
     *  
     *  NB : type must be the same, e.g. g2(reduce(g1)) will work only if mesh, Target, Singularity are the same...
     */
    template <typename Tag> gf(mpi::lazy<Tag, gf_const_view<Mesh, Target>> l) : gf() { operator=(l); }

    /// ---------------  swap --------------------

    // FIXME : REMOVE THIS SWAP ?
    friend void swap(gf &a, gf &b) noexcept { a.swap_impl(b); }

    /// ---------------  Operator = --------------------

    ///
    gf &operator=(gf &rhs) = default;

    ///
    gf &operator=(gf const &rhs) = default;

    ///
    gf &operator=(gf &&rhs) noexcept {
      this->swap_impl(rhs);
      return *this;
    }

    /**
     *
     * The assignment resizes the mesh and the data, invalidating all pointers on them.
     *
     * @tparam RHS  Type of the right hand side rhs. Must model GreenFunction concept.
     *
     * 		  RHS can be anything modeling the gf concept TBW
     * 		  In particular lazy expression with Green functions
     * @param rhs
     * @example    triqs/gfs/gf_assign_0.cpp
     */
    template <typename RHS> gf &operator=(RHS &&rhs) requires(GreenFunction<RHS>::value) {
      _mesh = rhs.mesh();
      _data.resize(rhs.data_shape());
      for (auto const &w : _mesh) (*this)[w] = rhs[w];
      _indices = rhs.indices();
      if (_indices.empty()) _indices = indices_t(target_shape());
      //if (not _indices.has_shape(target_shape())) _indices = indices_t(target_shape());
      // to be implemented : there is none in the gf_expr in particular....
      return *this;
    }

    // other = late, cf MPI

    public:
    // ------------- apply_on_data -----------------------------

    template <typename Fdata, typename Find> auto apply_on_data(Fdata &&fd, Find &&fi) {
      auto d2    = fd(_data);
      using t2   = target_from_array<decltype(d2), arity>;
      using gv_t = gf_view<Mesh, t2>;
      return gv_t{_mesh, d2, fi(_indices)};
    }

    template <typename Fdata> auto apply_on_data(Fdata &&fd) {
      return apply_on_data(std::forward<Fdata>(fd), [](auto &) { return indices_t{}; });
    }

    template <typename Fdata, typename Find> auto apply_on_data(Fdata &&fd, Find &&fi) const {
      auto d2    = fd(_data);
      using t2   = target_from_array<decltype(d2), arity>;
      using gv_t = gf_const_view<Mesh, t2>;
      return gv_t{_mesh, d2, fi(_indices)};
    }

    template <typename Fdata> auto apply_on_data(Fdata &&fd) const {
      return apply_on_data(std::forward<Fdata>(fd), [](auto &) { return indices_t{}; });
    }

    //-------------  MPI operation

    /**
     * Performs MPI reduce
     * @param l The lazy object returned by mpi::reduce
     */
    void operator=(mpi::lazy<mpi::tag::reduce, gf_const_view<Mesh, Target>> l) {
      _mesh    = l.rhs.mesh();
      _data    = mpi::reduce(l.rhs.data(), l.c, l.root, l.all, l.op); // nda:: necessary on gcc 5. why ??
      _indices = l.rhs.indices();
    }

    /**
     * Performs MPI scatter
     * @param l The lazy object returned by mpi::scatter
     */
    void operator=(mpi::lazy<mpi::tag::scatter, gf_const_view<Mesh, Target>> l) {
      _mesh    = mpi::scatter(l.rhs.mesh(), l.c, l.root);
      _data    = mpi::scatter(l.rhs.data(), l.c, l.root, true);
      _indices = mpi::scatter(l.rhs.indices(), l.c, l.root);
    }

    /**
     * Performs MPI gather
     * @param l The lazy object returned by mpi::gather
     */
    void operator=(mpi::lazy<mpi::tag::gather, gf_const_view<Mesh, Target>> l) {
      _mesh    = mpi::gather(l.rhs.mesh(), l.c, l.root);
      _data    = mpi::gather(l.rhs.data(), l.c, l.root, l.all);
      _indices = mpi::gather(l.rhs.indices(), l.c, l.root);
    }

    // Common code for gf, gf_view, gf_const_view
#include "./_gf_view_common.hpp"
  };

  /*------------------------------------------------------------------------
   *   Deduction guides
   *-----------------------------------------------------------------------*/

  // Forward declare gf_expr
  template <typename Tag, typename L, typename R> struct gf_expr;

  template <typename Tag, typename L, typename R>
  gf(gf_expr<Tag, L, R> &&) -> gf<typename gf_expr<Tag, L, R>::mesh_t, typename gf_expr<Tag, L, R>::target_t>;

  template <typename Tag, typename L, typename R>
  gf(gf_expr<Tag, L, R> const &) -> gf<typename gf_expr<Tag, L, R>::mesh_t, typename gf_expr<Tag, L, R>::target_t>;

} // namespace triqs::gfs
