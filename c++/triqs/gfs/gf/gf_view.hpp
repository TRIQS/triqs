// Copyright (c) 2019-2023 Simons Foundation
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
#include "./gf.hpp"

namespace triqs::gfs {

  // ----------------------  gf_view -----------------------------------------
  /**
   * The Green function container. 
   *
   * @tparam M       The domain of definition
   * @tparam Target  The target domain
   *
   * @include triqs/gfs.hpp
   */
  template <Mesh M, typename Target, typename Layout> class gf_view : is_view_tag, TRIQS_CONCEPT_TAG_NAME(GreenFunction) {

    using this_t = gf_view<M, Target, Layout>; // used in common code

    public:
    static constexpr bool is_view  = true;
    static constexpr bool is_const = false;

    using mutable_view_type = gf_view<M, Target, Layout>;

    /// Associated const view type
    using const_view_type = gf_const_view<M, Target, Layout>;

    /// Associated (non const) view type
    using view_type = gf_view<M, Target, Layout>;

    /// Associated regular type (gf<....>)
    using regular_type = gf<M, Target, typename Layout::contiguous_t>;

    /// The associated real type
    using real_t = gf_view<M, typename Target::real_t, Layout>;

    /// The associated complex type
    using complex_t = gf_view<M, typename Target::complex_t, Layout>;

    /// Template type
    using target_t = Target;

    /// Mesh type
    using mesh_t = M;

    /// Type of the mesh point
    using mesh_point_t = typename mesh_t::mesh_point_t;

    // NO DOC
    using mesh_index_t = typename mesh_t::index_t;

    /// Real or Complex
    using scalar_t = typename Target::scalar_t;

    /// Arity of the function (number of variables)
    static constexpr int arity = n_variables<M>;

    /// Rank of the Target
    static constexpr int target_rank = Target::rank;

    /// Rank of the data array representing the function
    static constexpr int data_rank = arity + Target::rank;

    /// Type of the data array
    using data_t = nda::basic_array_view<scalar_t, data_rank, Layout>;

    using target_shape_t = std::array<long, Target::rank>;

    struct target_and_shape_t {
      target_shape_t _shape;
      using target_t = Target;
      target_shape_t const &shape() const { return _shape; }
      typename Target::value_t make_value() const {
        if constexpr (target_t::rank == 0)
          return 0;
        else
          return typename Target::value_t{shape()};
      }
    };

    // ------------- Accessors -----------------------------

    /// Access the  mesh
    mesh_t const &mesh() const { return _mesh; }

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

    private:
    mesh_t _mesh;
    data_t _data;

    // -------------------------------- impl. details common to all classes -----------------------------------------------

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
    }

    public:
    // ---------------  Constructors --------------------

    ///
    gf_view() = default;

    // NO DOC
    gf_view(gf_const_view<M, Target> const &g) = delete;

    // NO DOC
    template <typename L> gf_view(gf<M, Target, L> const &g) = delete;

    ///
    template <typename L> gf_view(gf<M, Target, L> &g) : _mesh(g.mesh()), _data(g.data()) {}

    ///
    template <typename L> gf_view(gf<M, Target, L> &&g) noexcept : _mesh(std::move(g.mesh())), _data(g.data()) {}

    /**
       * Builds a view on top of a mesh, a data array
       * 
       * @tparam ArrayType Type of the data array 
       * @param dat Data array
       */
    gf_view(mesh_t m, data_t dat) : _mesh(std::move(m)), _data(dat) {}

    // ---------------  swap --------------------

    friend void swap(gf_view &a, gf_view &b) noexcept { a.swap_impl(b); }

    // ---------------  Rebind --------------------
    /**
       *  Rebinds the view
       *
       * @param g The const view to rebind into
       */
    void rebind(gf_view<M, Target> const &g) noexcept {
      this->_mesh = g._mesh;
      this->_data.rebind(g._data);
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

    template <typename Fdata> auto apply_on_data(Fdata &&fd) {
      auto d2    = fd(_data);
      using t2   = target_from_array<decltype(d2), arity>;
      using gv_t = gf_view<M, t2>;
      return gv_t{mesh(), d2};
    }

    template <typename Fdata> auto apply_on_data(Fdata &&fd) const {
      auto d2    = fd(_data);
      using t2   = target_from_array<decltype(d2), arity>;
      using gv_t = gf_const_view<M, t2>;
      return gv_t{mesh(), d2};
    }

    //-------------  MPI operation

    /**
    * Performs MPI reduce
    * @param l The lazy object returned by mpi::reduce
    */
    void operator=(mpi::lazy<mpi::tag::reduce, gf_const_view<M, Target>> l) {
      _mesh = l.rhs.mesh();
      _data = mpi::reduce(l.rhs.data(), l.c, l.root, l.all, l.op); // nda:: necessary on gcc 5. why ??
    }

    /**
     * Performs MPI scatter
     * @param l The lazy object returned by reduce
     */
    void operator=(mpi::lazy<mpi::tag::scatter, gf_const_view<M, Target>> l) {
      _mesh = mpi::scatter(l.rhs.mesh(), l.c, l.root);
      _data = mpi::scatter(l.rhs.data(), l.c, l.root, true);
    }

    /**
     * Performs MPI gather
     * @param l The lazy object returned by mpi::reduce
     */
    void operator=(mpi::lazy<mpi::tag::gather, gf_const_view<M, Target>> l) {
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
    if constexpr (nda::is_scalar_v<RHS>) {
      for (auto w : g.mesh()) g[w] = rhs;
    } else {
      if (!(g.mesh() == rhs.mesh())) TRIQS_RUNTIME_ERROR << "Gf Assignment in View : incompatible mesh \n" << g.mesh() << "\n vs \n" << rhs.mesh();
      for (auto w : g.mesh()) g[w] = rhs[w];
    }
  }
} // namespace triqs::gfs
/*------------------------------------------------------------------------------------------------------
 *             Delete std::swap for views, as for arrays
 *-----------------------------------------------------------------------------------------------------*/
namespace std {
  template <typename M, typename Target> void swap(triqs::gfs::gf_view<M, Target> &a, triqs::gfs::gf_view<M, Target> &b) = delete;
}
