// Copyright (c) 2019 Simons Foundation
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
// Authors: Olivier Parcollet

#pragma once
#include "./gf.hpp"

namespace triqs::gfs {

  // ----------------------  gf_view -----------------------------------------
  /**
   * The Green function container. 
   *
   * @tparam Var      The domain of definition
   * @tparam Target   The target domain
   *
   * @include triqs/gfs.hpp
   */
  template <typename Var, typename Target> class gf_view : is_view_tag, TRIQS_CONCEPT_TAG_NAME(GreenFunction) {

    using this_t = gf_view<Var, Target>; // used in common code

    public:
    static constexpr bool is_view  = true;
    static constexpr bool is_const = false;

    using mutable_view_type = gf_view<Var, Target>;

    /// Associated const view type
    using const_view_type = gf_const_view<Var, Target>;

    /// Associated (non const) view type
    using view_type = gf_view<Var, Target>;

    /// Associated regular type (gf<....>)
    using regular_type = gf<Var, Target>;

    /// The associated real type
    using real_t = gf_view<Var, typename Target::real_t>;

    /// Template type
    using variable_t = Var;

    /// Template type
    using target_t = Target;

    /// Mesh type
    using mesh_t = gf_mesh<Var>;

    /// Domain type
    using domain_t = typename mesh_t::domain_t;

    /// Type of the mesh point
    using mesh_point_t = typename mesh_t::mesh_point_t;

    // NO DOC
    using mesh_index_t        = typename mesh_t::index_t;
    using linear_mesh_index_t = typename mesh_t::linear_index_t;

    using indices_t   = gf_indices;
    using evaluator_t = gf_evaluator<Var, Target>;

    /// Real or Complex
    using scalar_t = typename Target::scalar_t;

    /// Arity of the function (number of variables)
    static constexpr int arity = get_n_variables<Var>::value;

    /// Rank of the data array representing the function
    static constexpr int data_rank = arity + Target::rank;

    using data_regular_t    = arrays::array<scalar_t, data_rank>;
    using data_view_t       = typename data_regular_t::view_type;
    using data_const_view_t = typename data_regular_t::const_view_type;

    /// Type of the data array
    using data_t = data_view_t;

    /// Type of the memory layout
    using data_memory_layout_t = memory_layout_t<data_rank>;

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
    target_and_shape_t target() const { return target_and_shape_t{_data.shape().template front_mpop<arity>()}; } // drop arity dims

    /**
        * Shape of the target
        *
        * @category Accessors
      */
    arrays::mini_vector<int, Target::rank> target_shape() const { return target().shape(); } // drop arity dims

    /**
        * Generator for the indices of the target space
        *
        * @category Accessors
      */
    auto target_indices() const { return itertools::product_range(target().shape()); }

    /** 
       * Memorylayout of the data
       *
       * @category Accessors
       */
    memory_layout_t<data_rank> const &memory_layout() const { return _data.indexmap().memory_layout(); }

    /// Indices of the Green function (for Python only)
    indices_t const &indices() const { return _indices; }

    private:
    mesh_t _mesh;
    data_t _data;
    indices_t _indices;

    using dproxy_t = details::_data_proxy<Target>;

    // -------------------------------- impl. details common to all classes -----------------------------------------------

    private:

    template <typename G> gf_view(impl_tag2, G &&x) : _mesh(x.mesh()), _data(x.data()), _indices(x.indices()) {}

    template <typename M, typename D>
    gf_view(impl_tag, M &&m, D &&dat, indices_t ind)
       : _mesh(std::forward<M>(m)), _data(std::forward<D>(dat)), _indices(std::move(ind)) {
      if (!(_indices.empty() or _indices.has_shape(target_shape()))) TRIQS_RUNTIME_ERROR << "Size of indices mismatch with data size";
    }

    public:
    /// Copy
    gf_view(gf_view const &x) = default;

    /// Move
    gf_view(gf_view &&) = default;

    private:
    void swap_impl(gf_view &b) noexcept {
      using std::swap;
      swap(this->_mesh, b._mesh);
      swap(this->_data, b._data);
      swap(this->_indices, b._indices);
    }

    public:
    // ---------------  Constructors --------------------

    ///
    gf_view() = default;

    // NO DOC
    gf_view(gf_const_view<Var, Target> const &g) = delete;

    // NO DOC
    gf_view(gf<Var, Target> const &g) = delete;

    ///
    gf_view(gf<Var, Target> &g) : gf_view(impl_tag2{}, g) {}

    ///
    gf_view(gf<Var, Target> &&g) noexcept : gf_view(impl_tag2{}, std::move(g)) {} // from a gf &&

    /**
       * Builds a view on top of a mesh, a data array
       * 
       * @tparam ArrayType Type of the data array 
       * @param dat Data array
       */
    template <typename ArrayType>
    gf_view(mesh_t m, ArrayType &&dat, indices_t const &ind = indices_t{}) : gf_view(impl_tag{}, std::move(m), std::forward<ArrayType>(dat), ind) {}

    // ---------------  swap --------------------

    friend void swap(gf_view &a, gf_view &b) noexcept { a.swap_impl(b); }

    // ---------------  Rebind --------------------
    /**
       *  Rebinds the view
       *
       * @param g The const view to rebind into
       */
    void rebind(gf_view<Var, Target> const &g) noexcept {
      this->_mesh = g._mesh;
      this->_data.rebind(g._data);
      this->_indices = g._indices;
    }

    // ---------------  operator =  --------------------
    /**
       *
       *  Copy the data, without resizing the view.
       *
       *  @warning If the size of `this` is not the same as rhs, the behaviour is undefined.
       */
    gf_view &operator=(gf_view const &rhs) {
      triqs_gf_view_assign_delegation(*this, rhs);
      return *this;
    }

    /**
       *
       * @tparam RHS WHAT IS POSSIBLE THERE ?
       */
    template <typename RHS> gf_view &operator=(RHS const &rhs) {
      triqs_gf_view_assign_delegation(*this, rhs);
      return *this;
    }

    public:
    // ------------- apply_on_data -----------------------------

    template <typename Fdata, typename Find> auto apply_on_data(Fdata &&fd, Find &&fi) {
      auto d2    = fd(_data);
      using t2   = target_from_array<decltype(d2), arity>;
      using gv_t = gf_view<Var, t2>;
      return gv_t{_mesh, d2, fi(_indices)};
    }

    template <typename Fdata> auto apply_on_data(Fdata &&fd) {
      return apply_on_data(std::forward<Fdata>(fd), [](auto &) { return indices_t{}; });
    }

    template <typename Fdata, typename Find> auto apply_on_data(Fdata &&fd, Find &&fi) const {
      auto d2    = fd(_data);
      using t2   = target_from_array<decltype(d2), arity>;
      using gv_t = gf_const_view<Var, t2>;
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
    void operator=(mpi_lazy<mpi::tag::reduce, gf_const_view<Var, Target>> l) {
      _mesh = l.rhs.mesh();
      _data = mpi::reduce(l.rhs.data(), l.c, l.root, l.all, l.op); // arrays:: necessary on gcc 5. why ??
    }

    /**
     * Performs MPI scatter
     * @param l The lazy object returned by reduce
     */
    void operator=(mpi_lazy<mpi::tag::scatter, gf_const_view<Var, Target>> l) {
      _mesh = mpi::scatter(l.rhs.mesh(), l.c, l.root);
      _data = mpi::scatter(l.rhs.data(), l.c, l.root, true);
    }

    /**
     * Performs MPI gather
     * @param l The lazy object returned by mpi::reduce
     */
    void operator=(mpi_lazy<mpi::tag::gather, gf_const_view<Var, Target>> l) {
      _mesh = mpi::gather(l.rhs.mesh(), l.c, l.root);
      _data = mpi::gather(l.rhs.data(), l.c, l.root, l.all);
    }
    
    // Common code for gf, gf_view, gf_const_view
#include "./_gf_view_common.hpp"
  };

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
} // namespace triqs::gfs
/*------------------------------------------------------------------------------------------------------
 *             Delete std::swap for views, as for arrays
 *-----------------------------------------------------------------------------------------------------*/
namespace std {
  template <typename Var, typename Target> void swap(triqs::gfs::gf_view<Var, Target> &a, triqs::gfs::gf_view<Var, Target> &b) = delete;
}
