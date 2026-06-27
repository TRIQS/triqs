// Copyright (c) 2020-2023 Simons Foundation
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
 * @brief Provides a non-owning view of a block Green's function.
 */

#pragma once

#include "./block_gf.hpp"
#include "../../utility/concept_tools.hpp"
#include "../../utility/exceptions.hpp"

#include <ostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace triqs::gfs {

  /**
   * @ingroup triqs-gfs-block
   * @brief A non-owning view of a block Green's function.
   *
   * @details triqs::gfs::block_gf_view refers to the blocks of an existing block Green's function without owning them.
   * Whether it is read-only is fixed by the `IsConst` template parameter (see the triqs::gfs::block_gf_const_view
   * alias). Assigning to a mutable view writes through to the underlying blocks. See triqs::gfs::block_gf for the
   * owning container.
   *
   * @tparam Mesh Mesh type of each block, modeling triqs::mesh::Mesh.
   * @tparam Target Target type of each block.
   * @tparam Layout Memory layout policy of each block's data array view.
   * @tparam Arity Number of block indices (1 for block_gf_view, 2 for block2_gf_view).
   * @tparam IsConst Whether the view is read-only.
   */
  template <typename Mesh, typename Target, typename Layout, int Arity, bool IsConst>
  class block_gf_view : is_view_tag, TRIQS_CONCEPT_TAG_NAME(BlockGreenFunction) {
    using this_t = block_gf_view; // for common code

    public:
    /// Whether this type is a view (always `true` for triqs::gfs::block_gf_view).
    static constexpr bool is_view = true;

    /// Whether this view is read-only.
    static constexpr bool is_const = IsConst;

    /// Number of block indices (1 for block_gf_view, 2 for block2_gf_view).
    static constexpr int arity = Arity;

    /// Mesh type of each block.
    using mesh_t = Mesh;

    /// Target type of each block.
    using target_t = Target;

    /// Associated regular (owning) type.
    using regular_type = block_gf<Mesh, Target, typename Layout::contiguous_t, Arity>;

    /// Associated mutable view type.
    using mutable_view_type = block_gf_view<Mesh, Target, Layout, Arity>;

    /// Associated (non const) view type.
    using view_type = block_gf_view<Mesh, Target, Layout, Arity, false>;

    /// Associated const view type.
    using const_view_type = block_gf_view<Mesh, Target, Layout, Arity, true>;

    /// The associated real type
    using real_t = block_gf_view<Mesh, typename Target::real_t, Layout, Arity, IsConst>;

    /// Type of a single block (a const or mutable view depending on `IsConst`).
    using g_t = std::conditional_t<IsConst, gf_const_view<Mesh, Target, Layout>, gf_view<Mesh, Target, Layout>>;

    /// Type storing the blocks (a vector of blocks, or vector of vectors for a block2_gf_view).
    using data_t = std::conditional_t<Arity == 1, std::vector<g_t>, std::vector<std::vector<g_t>>>;

    /// Type storing the block names (a vector of names, or vector of vectors for a block2_gf_view).
    using block_names_t = std::conditional_t<Arity == 1, std::vector<std::string>, std::vector<std::vector<std::string>>>;

    /// Name of the block Green's function.
    std::string name;

    private:
    block_names_t _block_names;
    data_t _glist;

    // ---------------  Constructors --------------------

    // Tag and constructor used internally to build a view from another block-gf-like object.
    struct impl_tag {};
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): x is only read to build views on its data, not forwarded
    template <typename G> block_gf_view(impl_tag, G &&x) : name(x.name), _block_names(x.block_names()), _glist(factory<data_t>(x.data())) {}

    public:
    /**
     * @brief Copy constructor (shallow: the new view refers to the same blocks).
     * @param x View to copy from.
     */
    block_gf_view(block_gf_view const &x) = default;

    /// Move constructor.
    block_gf_view(block_gf_view &&) = default;

    /**
     * @brief Construct from a list of block names and a list of Green's function views.
     *
     * @details The two lists must have matching sizes (throws a triqs::runtime_error otherwise).
     *
     * @param b Block names.
     * @param d Blocks (Green's function views).
     */
    block_gf_view(block_names_t b, data_t d) : _block_names(std::move(b)), _glist(std::move(d)) {
      if constexpr (Arity == 1) {
        if (_glist.size() != _block_names.size())
          TRIQS_RUNTIME_ERROR << "block_gf(vector<string>, vector<gf>) : the two vectors do not have the same size !";
      } else {
        if (_glist.size() != _block_names[0].size())
          TRIQS_RUNTIME_ERROR << "block2_gf(vector<vector<string>>, vector<vector<gf>>) : Outer vectors have different sizes !";
        if (_glist.size() != 0)
          if (_glist[0].size() != _block_names[1].size())
            TRIQS_RUNTIME_ERROR << "block2_gf(vector<vector<string>>, vector<vector<gf>>) : Inner vectors have different sizes !";
      }
    }

    // ---------------  Constructors --------------------

    /// Construct an empty view, not bound to any data.
    block_gf_view() = default;

    /**
     * @brief Construct a const view onto a block Green's function.
     * @tparam L Layout of the block Green's function.
     * @param g Block Green's function to view.
     */
    template <typename L>
    block_gf_view(block_gf<Mesh, Target, L, Arity> const &g)
      requires(IsConst)
       : block_gf_view(impl_tag{}, g) {}

    /**
     * @brief Construct a mutable view onto a (non const) block Green's function.
     * @tparam L Layout of the block Green's function.
     * @param g Block Green's function to view.
     */
    template <typename L>
    block_gf_view(block_gf<Mesh, Target, L, Arity> &g)
      requires(!IsConst)
       : block_gf_view(impl_tag{}, g) {}

    /**
     * @brief Construct a view onto an rvalue block Green's function.
     * @tparam L Layout of the block Green's function.
     * @param g Block Green's function to view.
     */
    template <typename L> block_gf_view(block_gf<Mesh, Target, L, Arity> &&g) noexcept : block_gf_view(impl_tag{}, std::move(g)) {}

    /**
     * @brief Construct a const view from a mutable view of the same kind.
     * @tparam L Layout of the view.
     * @param g View to copy from.
     */
    template <typename L>
    block_gf_view(block_gf_view<Mesh, Target, L, Arity, !IsConst> const &g)
      requires(IsConst)
       : block_gf_view(impl_tag{}, g) {}

    /// ---------------  Operator = --------------------

    /**
     * @brief Copy assignment: copy the data through the view without resizing it.
     * @param rhs View to copy from.
     * @return A reference to this view.
     */
    block_gf_view &operator=(block_gf_view const &rhs)
      requires(not IsConst)
    {
      _assign_impl(rhs);
      return *this;
    }

    /**
     * @brief Assign from any compatible right hand side, writing through the view without resizing it.
     *
     * @details `rhs` can be a scalar (assigned to every block) or anything with `.block_names()` and `[n]` returning a
     * Green's function. For a non-scalar right hand side the number of blocks must match (throws otherwise).
     *
     * @tparam RHS Type of the right hand side.
     * @param rhs Object to assign from.
     * @return A reference to this view.
     */
    template <typename RHS>
    block_gf_view &operator=(RHS const &rhs)
      requires(not IsConst)
    {
      if constexpr (not nda::is_scalar_v<RHS>) {
        if (!(size() == rhs.size())) TRIQS_RUNTIME_ERROR << "Gf Assignment in View : incompatible size" << size() << " vs " << rhs.size();
        _assign_impl(rhs);
      } else {
        if constexpr (Arity == 1) {
          for (auto &y : _glist) y = rhs;
        } else {
          for (auto &x : _glist)
            for (auto &y : x) y = rhs;
        }
      }
      return *this;
    }

    /**
     * @brief Assignment operator overload specific for triqs::gfs::lazy_transform_t objects.
     *
     * @details Applies the deferred lambda block by block, e.g. the lazy object returned by `fourier(my_block_gf)`.
     *
     * @tparam L Type of the deferred lambda.
     * @tparam G Type of the deferred value.
     * @param rhs The lazy object returned e.g. by fourier(my_block_gf).
     * @return A reference to this view.
     */
    template <typename L, typename G>
    block_gf_view &operator=(lazy_transform_t<L, G> const &rhs)
      requires(not IsConst)
    {
      if constexpr (Arity == 1) {
        for (int i = 0; i < rhs.value.size(); ++i) (*this)[i] = rhs.lambda(rhs.value[i]);
      } else {

        for (int i = 0; i < rhs.value.size1(); ++i)
          for (int j = 0; j < rhs.value.size2(); ++j) (*this)(i, j) = rhs.lambda(rhs.value(i, j));
      }
      return *this;
    }

    // ---------------  Rebind --------------------

    /**
     * @brief Rebind the view to refer to the blocks of another view of the same kind.
     * @param x View to rebind onto.
     */
    void rebind(block_gf_view x) noexcept {
      _block_names = x._block_names;
      _glist       = data_t{x._glist}; // copy of vector<vector<gf_view>>, makes new views on the gf of x
      name         = x.name;
    }

    /**
     * @brief Rebind a const view onto a mutable view of the same kind.
     * @param X View to rebind onto.
     */
    void rebind(block_gf_view<Mesh, Target, Layout, Arity, !IsConst> const &X) noexcept
      requires(IsConst)
    {
      rebind(block_gf_view{X});
    }

    /**
     * @brief Rebind a const view onto a block Green's function.
     * @param X Block Green's function to rebind onto.
     */
    void rebind(block_gf<Mesh, Target, Layout, Arity> const &X) noexcept
      requires(IsConst)
    {
      rebind(block_gf_view{X});
    }

    /**
     * @brief Rebind a view onto a (non const) block Green's function.
     * @param X Block Green's function to rebind onto.
     */
    void rebind(block_gf<Mesh, Target, Layout, Arity> &X) noexcept { rebind(block_gf_view{X}); }

    public:
    //----------------------------- print  -----------------------------

    /// Writing a block Green's function to an output stream is not supported.
    friend std::ostream &operator<<(std::ostream &out, block_gf_view const &) { return out << "block_gf_view"; }

    // Common code for gf, gf_view, gf_const_view
#include "./_block_gf_view_common.hpp"
  };

} // namespace triqs::gfs

/*------------------------------------------------------------------------------------------------------
 *             Delete std::swap for views
 *-----------------------------------------------------------------------------------------------------*/
namespace std {
  // Deleted std::swap for block Green's function views (as for nda array views): use rebind instead.
  // NOLINTNEXTLINE(bugprone-std-namespace-modification): deleting std::swap for views is intentional, as for nda arrays
  template <typename Mesh, typename Target, typename Layout, int Arity, bool IsConst>
  void swap(triqs::gfs::block_gf_view<Mesh, Target, Layout, Arity, IsConst> &a,
            triqs::gfs::block_gf_view<Mesh, Target, Layout, Arity, IsConst> &b) = delete;
} // namespace std
