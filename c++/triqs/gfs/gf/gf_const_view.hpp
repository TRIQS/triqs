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

/**
 * @file
 * @brief Provides the triqs::gfs::gf_const_view container, a read-only non-owning view of a Green's function.
 */

#pragma once

#include "./gf.hpp"
#include "../../utility/concept_tools.hpp"

#include <itertools/itertools.hpp>

#include <array>
#include <utility>

namespace triqs::gfs {

  /**
   * @ingroup triqs-gfs-containers
   * @brief A read-only, non-owning view of a Green's function.
   *
   * @details triqs::gfs::gf_const_view refers to the mesh and data of an existing Green's function (or a slice
   * thereof) without owning them and without allowing modification of the underlying data. See triqs::gfs::gf for the
   * owning container and triqs::gfs::gf_view for the mutable view.
   *
   * @tparam M Mesh type, modeling triqs::mesh::Mesh; fixes the domain of definition.
   * @tparam Target Target type; fixes the value stored at each mesh point.
   * @tparam Layout Memory layout policy of the data array view.
   */
  template <Mesh M, typename Target, typename Layout> class gf_const_view : is_view_tag, TRIQS_CONCEPT_TAG_NAME(GreenFunction) {

    using this_t = gf_const_view<M, Target, Layout>; // used in common code

    public:
    /// Whether this type is a view (always `true` for triqs::gfs::gf_const_view).
    static constexpr bool is_view = true;

    /// Whether this type is a const view (always `true` for triqs::gfs::gf_const_view).
    static constexpr bool is_const = true;

    /// Associated mutable view type.
    using mutable_view_type = gf_view<M, Target, Layout>;

    /// Associated const view type.
    using const_view_type = gf_const_view<M, Target, Layout>;

    /// Associated (non const) view type.
    using view_type = gf_const_view<M, Target, Layout>;

    /// Associated regular (owning) type.
    using regular_type = gf<M, Target>; // FIXME : find the Layout

    /// Associated real-valued type.
    using real_t = gf_const_view<M, typename Target::real_t, Layout>;

    /// Associated complex-valued type.
    using complex_t = gf_const_view<M, typename Target::complex_t, Layout>;

    /// Target type.
    using target_t = Target;

    /// Mesh type.
    using mesh_t = M;

    /// Type of a mesh point.
    using mesh_point_t = typename mesh_t::mesh_point_t;

    /// Type of a mesh index.
    using mesh_index_t = typename mesh_t::index_t;

    /// Scalar type of the target (real or complex).
    using scalar_t = typename Target::scalar_t;

    /// Arity of the function, i.e. the number of mesh variables.
    static constexpr int arity = n_variables<M>;

    /// Rank of the target.
    static constexpr int target_rank = Target::rank;

    /// Rank of the data array representing the function (`arity + target_rank`).
    static constexpr int data_rank = arity + Target::rank;

    /// Type of the (const) data array view.
    using data_t = nda::basic_array_view<const scalar_t, data_rank, Layout>;

    /// Type of the target shape, i.e. an array holding the extent of each target dimension.
    using target_shape_t = std::array<long, Target::rank>;

    /// Lightweight handle bundling the target type and its shape.
    struct target_and_shape_t {
      /// The shape of the target.
      target_shape_t _shape;

      /// The target type.
      using target_t = Target;

      /// Get the shape of the target.
      target_shape_t const &shape() const { return _shape; }
    };

    // ------------- Accessors -----------------------------

    /**
     * @brief Get the mesh of the Green's function.
     * @return A const reference to the mesh.
     */
    mesh_t const &mesh() const { return _mesh; }

    /**
     * @brief Get the data array view.
     * @return A reference to the (const) data array view.
     */
    data_t &data() & { return _data; }

    /**
     * @brief Get the data array view (const overload).
     * @return A const reference to the (const) data array view.
     */
    data_t const &data() const & { return _data; }

    /**
     * @brief Get the data array view (rvalue overload).
     * @return The (const) data array view, moved out of the (expiring) view.
     */
    data_t data() && { return std::move(_data); }

    /**
     * @brief Get the shape of the data array.
     *
     * @details Needed for generic code: expressions of Green's functions (e.g. `g1 + g2`) have a data shape but no
     * data array.
     *
     * @return A const reference to the shape of the data array.
     */
    auto const &data_shape() const { return _data.shape(); }

    /**
     * @brief Get a handle to the target and its shape.
     * @return A triqs::gfs::gf_const_view::target_and_shape_t bundling the target type and shape (mesh dims dropped).
     */
    target_and_shape_t target() const { return target_and_shape_t{stdutil::front_mpop<arity>(_data.shape())}; } // drop arity dims

    /**
     * @brief Get the shape of the target.
     * @return An array holding the extent of each target dimension.
     */
    std::array<long, Target::rank> target_shape() const { return target().shape(); } // drop arity dims

    /**
     * @brief Get a generator over the multi-indices of the target space.
     * @return A range iterating over all index tuples of the target.
     */
    auto target_indices() const { return itertools::product_range(target().shape()); }

    private:
    mesh_t _mesh;
    data_t _data;

    // -------------------------------- impl. details common to all classes -----------------------------------------------

    public:
    /**
     * @brief Copy constructor (shallow: the new view refers to the same data).
     * @param x View to copy from.
     */
    gf_const_view(gf_const_view const &x) = default;

    /// Move constructor.
    gf_const_view(gf_const_view &&) = default;

    private:
    void swap_impl(gf_const_view &b) noexcept {
      using std::swap;
      swap(this->_mesh, b._mesh);
      swap(this->_data, b._data);
    }

    public:
    // ---------------  Constructors --------------------

    /// Construct an empty view, not bound to any data.
    gf_const_view() = default;

    /**
     * @brief Construct a const view from a (mutable) view.
     * @param g View to view as const.
     */
    gf_const_view(gf_view<M, Target> const &g) : _mesh(g.mesh()), _data(g.data()) {}

    /**
     * @brief Construct a const view onto a const Green's function.
     * @param g Green's function to view.
     */
    gf_const_view(gf<M, Target> const &g) : _mesh(g.mesh()), _data(g.data()) {}

    /**
     * @brief Construct a const view onto a (non const) Green's function.
     * @param g Green's function to view.
     */
    gf_const_view(gf<M, Target> &g) : _mesh(g.mesh()), _data(g.data()) {} // from a gf &

    /**
     * @brief Construct a const view onto an rvalue Green's function.
     * @param g Green's function to view.
     */
    // NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved): a view binds to the data of `g`, not moving it
    gf_const_view(gf<M, Target> &&g) noexcept : _mesh(std::move(g.mesh())), _data(std::move(g.data())) {} // from a gf &&

    /**
     * @brief Build a const view on top of a mesh and a data array.
     *
     * @param m Mesh.
     * @param dat Data array (view).
     */
    gf_const_view(mesh_t m, data_t dat) : _mesh(std::move(m)), _data(dat) {}

    // ---------------  swap --------------------

    /**
     * @brief Swap two const views.
     * @param a First view.
     * @param b Second view.
     */
    friend void swap(gf_const_view &a, gf_const_view &b) noexcept { a.swap_impl(b); }

    // ---------------  Rebind --------------------

    /**
     * @brief Rebind the const view to refer to the mesh and data of another const view.
     * @param g Const view to rebind onto.
     */
    void rebind(gf_const_view<M, Target> const &g) noexcept {
      this->_mesh = g._mesh;
      this->_data.rebind(g._data);
    }

    /**
     * @brief Rebind the const view onto a (mutable) view.
     * @param X View to rebind onto.
     */
    void rebind(gf_view<M, Target> const &X) noexcept { rebind(gf_const_view{X}); }

    // ---------------  No = since it is const ... --------------------

    /// Deleted: a const view cannot be assigned to.
    gf_const_view &operator=(gf_const_view const &) = delete; // a const view can not be assigned to

    public:
    // ------------- apply_on_data -----------------------------

    /**
     * @brief Build a const view whose data is the result of applying a function to the data array.
     *
     * @tparam Fdata Type of the callable applied to the data array.
     * @param fd Callable taking the data array and returning a transformed array.
     * @return A triqs::gfs::gf_const_view onto the transformed data, sharing this view's mesh.
     */
    template <typename Fdata> auto apply_on_data(Fdata &&fd) {
      auto d2    = std::forward<Fdata>(fd)(_data);
      using t2   = target_from_array<decltype(d2), arity>;
      using gv_t = gf_const_view<M, t2>;
      return gv_t{mesh(), d2};
    }

    /**
     * @brief Build a const view whose data is the result of applying a function to the data array (const overload).
     *
     * @tparam Fdata Type of the callable applied to the data array.
     * @param fd Callable taking the data array and returning a transformed array.
     * @return A triqs::gfs::gf_const_view onto the transformed data, sharing this view's mesh.
     */
    template <typename Fdata> auto apply_on_data(Fdata &&fd) const {
      auto d2    = std::forward<Fdata>(fd)(_data);
      using t2   = target_from_array<decltype(d2), arity>;
      using gv_t = gf_const_view<M, t2>;
      return gv_t{mesh(), d2};
    }

    // Common code for gf, gf_view, gf_const_view
#include "./_gf_view_common.hpp"
  };

} // namespace triqs::gfs
