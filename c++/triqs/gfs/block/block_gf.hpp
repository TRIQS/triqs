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
 * @brief Provides the block Green's function container.
 */

#pragma once

#include "./gf_struct.hpp"
#include "../gf/gf.hpp"
#include "../gf/targets.hpp"
#include "../../utility/concept_tools.hpp"
#include "../../utility/exceptions.hpp"
#include "../../utility/factory.hpp"
#include "../../utility/macros.hpp" // IWYU pragma: keep (used by ./_block_gf_view_common.hpp)

#include <itertools/itertools.hpp>
#include <mpi/mpi.hpp>

#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace triqs::gfs {

  using triqs::utility::factory;

  // Forward declarations of the main block types: block_gf, block_gf_view.
  template <typename Mesh, typename Target = matrix_valued, typename Layout = nda::C_layout, int Arity = 1> class block_gf;
  template <typename Mesh, typename Target = matrix_valued, typename Layout = nda::C_stride_layout, int Arity = 1, bool IsConst = false>
  class block_gf_view;

  /**
   * @ingroup triqs-gfs-block
   * @brief Const view alias for a block Green's function.
   * 
   * @tparam Mesh Mesh type of each block.
   * @tparam Target Target type of each block.
   * @tparam Layout Memory layout policy of each block.
   * @tparam Arity Number of block indices.
   */
  template <typename Mesh, typename Target = matrix_valued, typename Layout = nda::C_stride_layout, int Arity = 1>
  using block_gf_const_view = block_gf_view<Mesh, Target, Layout, Arity, true>;

  /**
   * @ingroup triqs-gfs-block
   * @brief Owning two-index block Green's function (a matrix of blocks).
   * 
   * @tparam Mesh Mesh type of each block.
   * @tparam Target Target type of each block.
   * @tparam Layout Memory layout policy of each block.
   */
  template <typename Mesh, typename Target = matrix_valued, typename Layout = nda::C_layout> using block2_gf = block_gf<Mesh, Target, Layout, 2>;

  /**
   * @ingroup triqs-gfs-block
   * @brief Mutable view of a two-index block Green's function.
   * 
   * @tparam Mesh Mesh type of each block.
   * @tparam Target Target type of each block.
   * @tparam Layout Memory layout policy of each block.
   */
  template <typename Mesh, typename Target = matrix_valued, typename Layout = nda::C_stride_layout>
  using block2_gf_view = block_gf_view<Mesh, Target, Layout, 2, false>;

  /**
   * @ingroup triqs-gfs-block
   * @brief Const view of a two-index block Green's function.
   * 
   * @tparam Mesh Mesh type of each block.
   * @tparam Target Target type of each block.
   * @tparam Layout Memory layout policy of each block.
   */
  template <typename Mesh, typename Target = matrix_valued, typename Layout = nda::C_stride_layout>
  using block2_gf_const_view = block_gf_view<Mesh, Target, Layout, 2, true>;

  // --------------------------- CTAD ---------------------------------

  // Deduce a triqs::gfs::block_gf type from a vector of Green's functions.
  template <typename Mesh, typename Target, typename Layout> block_gf(std::vector<gf<Mesh, Target, Layout>>) -> block_gf<Mesh, Target, Layout, 1>;

  // Deduce a (matrix-valued) triqs::gfs::block_gf type from a mesh and a triqs::gfs::gf_struct_t.
  template <typename Mesh> block_gf(Mesh const &, gf_struct_t const &) -> block_gf<Mesh, matrix_valued>;

  // Deduce a triqs::gfs::block_gf type from a triqs::gfs::block_gf_view.
  template <typename Mesh, typename Target, typename Layout, int Arity, bool IsConst>
  block_gf(block_gf_view<Mesh, Target, Layout, Arity, IsConst>) -> block_gf<Mesh, Target, typename Layout::contiguous_t, Arity>;

  /**
   * @addtogroup triqs-gfs-utils
   * @{
   */

  /**
   * @brief Trait to check whether a type is a block Green's function.
   *
   * @details It is `true` if `G` is a triqs::gfs::block_gf or triqs::gfs::block2_gf (or a view thereof).
   *
   * @tparam G Type to test.
   * @tparam n Optional arity to require (0 = any, 1 = block_gf, 2 = block2_gf).
   */
  template <typename G, int n = 0> inline constexpr bool is_block_gf_v = false;

  // Specialization of is_block_gf_v for cvref types, which decays G first.
  template <typename G, int n>
    requires(!std::is_same_v<G, std::remove_cvref_t<G>>)
  inline constexpr bool is_block_gf_v<G, n> = is_block_gf_v<std::remove_cvref_t<G>, n>;

  // Specialization of is_block_gf_v for a one-index block_gf.
  template <typename Mesh, typename Target, typename Layout, int Arity>
  inline constexpr bool is_block_gf_v<block_gf<Mesh, Target, Layout, Arity>, Arity> = true;

  // Specialization of is_block_gf_v for a one-index block_gf_view.
  template <typename Mesh, typename Target, typename Layout, int Arity, bool IsConst>
  inline constexpr bool is_block_gf_v<block_gf_view<Mesh, Target, Layout, Arity, IsConst>, Arity> = true;

  // Specialization of is_block_gf_v without an arity constraint.
  template <typename G> inline constexpr bool is_block_gf_v<G, 0> = is_block_gf_v<G, 1> or is_block_gf_v<G, 2>;

  /// Block arity of a type: `T::arity` if present, `-1` otherwise.
  template <typename, typename = std::void_t<>> inline constexpr int arity_of = -1;

  // Specialization of arity_of for types exposing a static `arity` member.
  template <typename T> inline constexpr int arity_of<T, std::void_t<decltype(T::arity)>> = T::arity;

  /**
   * @brief Get the mesh of a block Green's function, or its N-th component for a product mesh.
   *
   * @details Assumes a uniform mesh across all blocks: only the 0th block (block (0,0) for a block2_gf) is inspected; 
   * the meshes of all other blocks are ignored. 
   * 
   * Delegates to the MemoryGf overload triqs::gfs::get_mesh(G const &) for the per-block logic.
   *
   * @tparam N Index of the mesh component to return (default 0). Ignored for non-product meshes.
   * @tparam BG The type of the block Green's function.
   * @param bg The block Green's function.
   * @return A const reference to the (N-th component of the) mesh of the 0th block.
   */
  template <int N = 0, typename BG>
    requires is_block_gf_v<BG>
  auto const &get_mesh(BG const &bg) {
    if constexpr (is_block_gf_v<BG, 1>)
      return get_mesh<N>(bg[0]);
    else // block2_gf: inspect block (0,0)
      return get_mesh<N>(bg(0, 0));
  }

  /// The mesh type of a block Green's function type `G`.
  template <typename G> using get_mesh_t = typename std::decay_t<G>::mesh_t;

  /// The target type of a block Green's function type `G`.
  template <typename G> using get_target_t = typename std::decay_t<G>::target_t;

  /// The triqs::gfs::block_gf type matching the mesh and target of `G`.
  template <typename G> using block_gf_of = block_gf<get_mesh_t<G>, get_target_t<G>>;

  /// The triqs::gfs::block_gf_view type matching the mesh and target of `G`.
  template <typename G> using block_gf_view_of = block_gf_view<get_mesh_t<G>, get_target_t<G>>;

  /// The triqs::gfs::block_gf_const_view type matching the mesh and target of `G`.
  template <typename G> using block_gf_const_view_of = block_gf_const_view<get_mesh_t<G>, get_target_t<G>>;

  /// The triqs::gfs::block2_gf type matching the mesh and target of `G`.
  template <typename G> using block2_gf_of = block2_gf<get_mesh_t<G>, get_target_t<G>>;

  /// The triqs::gfs::block2_gf_view type matching the mesh and target of `G`.
  template <typename G> using block2_gf_view_of = block2_gf_view<get_mesh_t<G>, get_target_t<G>>;

  /// The triqs::gfs::block2_gf_const_view type matching the mesh and target of `G`.
  template <typename G> using block2_gf_const_view_of = block2_gf_const_view<get_mesh_t<G>, get_target_t<G>>;

  /** @} */

  // The trait and concept tag marking a type as a block Green's function.
  TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT(BlockGreenFunction);

  /**
   * @ingroup triqs-gfs-concepts
   * @brief Concept checking that a type is a block Green's function.
   *
   * @details A type models triqs::gfs::BlockGf if it is one of the block Green's function containers
   * (triqs::gfs::block_gf, triqs::gfs::block_gf_view) or their two-index analogues, i.e. if `BlockGreenFunction_v` is
   * `true` for it.
   *
   * @tparam G Type to check.
   */
  template <typename G>
  concept BlockGf = BlockGreenFunction_v<G>;

  // Forward declarations with default arguments. The corresponding friend declarations inside `block_gf` and
  // the definitions in `./mpi.hpp` may then re-declare these templates without re-introducing defaults.
  template <BlockGf BG> void mpi_broadcast(BG &&bg, mpi::communicator c = {}, int root = 0);
  template <BlockGf BG1, BlockGf BG2>
  void mpi_reduce_into(BG1 const &bg_in, BG2 &&bg_out, mpi::communicator c = {}, int root = 0, bool all = false, MPI_Op op = MPI_SUM);

  // ------------- Helper Types -----------------------------

  // Lightweight pairing of a lambda with a value, used to defer a per-block transform (e.g. fourier(block_gf)).
  template <typename Lambda, typename T> struct lazy_transform_t {
    Lambda lambda;
    T value;
  };

  // Build a lazy_transform_t from a lambda and a value.
  template <typename Lambda, typename T> lazy_transform_t<Lambda, T> make_lazy_transform(Lambda &&l, T &&x) {
    return {std::forward<Lambda>(l), std::forward<T>(x)};
  }

  // ---------------------------  details  ---------------------------------

  namespace details {
    // Build default block names "0", "1", ... for a one-index block_gf of n blocks.
    inline auto _make_block_names1(int n) {
      std::vector<std::string> r(n);
      for (int i = 0; i < n; ++i) r[i] = std::to_string(i);
      return r;
    }
    // Build default block names for a two-index block2_gf of n x p blocks.
    inline std::vector<std::vector<std::string>> _make_block_names2(int n, int p) { return {_make_block_names1(n), _make_block_names1(p)}; }
  } // namespace details

  // ---------------------------  implementation  ---------------------------------

  /**
   * @ingroup triqs-gfs-block
   * @brief The owning block Green's function container.
   *
   * @details triqs::gfs::block_gf is a value-semantic container holding a list of Green's functions (the blocks), each
   * a triqs::gfs::gf, and models the `BlockGreenFunction` concept. 
   * 
   * For `Arity == 1` the blocks form a simple vector (a block_gf). For `Arity == 2` they form a matrix of blocks (a 
   * triqs::gfs::block2_gf). 
   * 
   * Each block is accessed by index (or block name) via `operator[]` (`operator()` for a block2_gf). Non-owning views 
   * are provided by triqs::gfs::block_gf_view.
   *
   * @tparam Mesh Mesh type of each block, modeling triqs::mesh::Mesh.
   * @tparam Target Target type of each block.
   * @tparam Layout Memory layout policy of each block's data array.
   * @tparam Arity Number of block indices (1 for block_gf, 2 for block2_gf).
   */
  template <typename Mesh, typename Target, typename Layout, int Arity> class block_gf : TRIQS_CONCEPT_TAG_NAME(BlockGreenFunction) {
    using this_t = block_gf; // for common code
    public:
    /// Whether this type is a view (always `false` for triqs::gfs::block_gf).
    static constexpr bool is_view = false;

    /// Whether this type is a const view (always `false` for triqs::gfs::block_gf).
    static constexpr bool is_const = false;

    /// Number of block indices (1 for block_gf, 2 for block2_gf).
    static constexpr int arity = Arity;

    /// Mesh type of each block.
    using mesh_t = Mesh;

    /// Target type of each block.
    using target_t = Target;

    /// Associated regular (owning) type.
    using regular_type = block_gf<Mesh, Target, Layout, Arity>;

    /// Associated mutable view type.
    using mutable_view_type = block_gf_view<Mesh, Target, typename Layout::with_lowest_guarantee_t, Arity>;

    /// Associated (non const) view type.
    using view_type = block_gf_view<Mesh, Target, typename Layout::with_lowest_guarantee_t, Arity, false>;

    /// Associated const view type.
    using const_view_type = block_gf_view<Mesh, Target, typename Layout::with_lowest_guarantee_t, Arity, true>;

    /// The associated real type
    using real_t = block_gf<Mesh, typename Target::real_t, Layout, Arity>;

    /// Type of a single block.
    using g_t = gf<Mesh, Target, Layout>;

    /// Type storing the blocks (a vector of blocks, or vector of vectors for a block2_gf).
    using data_t = std::conditional_t<Arity == 1, std::vector<g_t>, std::vector<std::vector<g_t>>>;

    /// Type storing the block names (a vector of names, or vector of vectors for a block2_gf).
    using block_names_t = std::conditional_t<Arity == 1, std::vector<std::string>, std::vector<std::vector<std::string>>>;

    /// Name of the block Green's function.
    std::string name;

    private:
    block_names_t _block_names;
    data_t _glist;

    // ---------------  Constructors --------------------

    // Tag and constructor used internally to build from another block-gf-like object.
    struct impl_tag {};
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): x is only read to copy its data, not forwarded
    template <typename G> block_gf(impl_tag, G &&x) : name(x.name), _block_names(x.block_names()), _glist(factory<data_t>(x.data())) {} // NOLINT

    public:
    /**
     * @brief Copy constructor.
     * @param x Block Green's function to copy from.
     */
    block_gf(block_gf const &x) = default;

    /// Move constructor.
    block_gf(block_gf &&) = default;

    /**
     * @brief Construct from a list of block names and a list of Green's functions.
     *
     * @details The two lists must have matching sizes (throws a triqs::runtime_error otherwise).
     *
     * @param b Block names.
     * @param d Blocks (Green's functions).
     */
    block_gf(block_names_t b, data_t d) : _block_names(std::move(b)), _glist(std::move(d)) {
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

    /// Construct an empty block Green's function (with no blocks).
    block_gf() = default;

    /**
     * @brief Construct from a view of the same kind, making a deep copy of the data.
     * 
     * @tparam L Layout of the view.
     * @tparam Cnst Whether the view is const.
     * @param g View to copy from.
     */
    template <typename L, bool Cnst> block_gf(block_gf_view<Mesh, Target, L, Arity, Cnst> const &g) : block_gf(impl_tag{}, g) {}

    /**
     * @brief Construct from any object modeling the `BlockGreenFunction` concept with a compatible target.
     *
     * @details The block Green's function is default constructed and then assigned from `x`, which resizes it
     * accordingly.
     *
     * @tparam G A type modeling `BlockGreenFunction` with the same (or complex-promoted) target type and same arity.
     * @param x Object to construct from (e.g. another block Green's function or a lazy expression).
     */
    // TODO: We would like to refine this, G should have the same mesh, target, at least ...
    template <typename G>
    block_gf(G const &x)
      requires(BlockGreenFunction_v<G> and (std::is_same_v<get_target_t<G>, Target> or std::is_same_v<typename get_target_t<G>::complex_t, Target>))
       : block_gf() {
      static_assert(G::arity == Arity, "Impossible");
      *this = x;
    }

    /**
     * @brief Construct a one-index block_gf from a vector of Green's functions (block names default to "0", "1", ...).
     * @param V Blocks (Green's functions).
     */
    block_gf(data_t V)
      requires(Arity == 1)
       : _block_names(details::_make_block_names1(V.size())), _glist(std::move(V)) {}

    /**
     * @brief Construct a one-index block_gf of `n` default-constructed blocks.
     * @param n Number of blocks.
     */
    block_gf(int n)
      requires(Arity == 1)
       : block_gf(data_t(n)) {}

    /**
     * @brief Construct a one-index block_gf of `n` copies of a Green's function.
     * 
     * @param n Number of blocks.
     * @param g Green's function to copy into each block.
     */
    block_gf(int n, g_t const &g)
      requires(Arity == 1)
       : block_gf(data_t(n, g)) {}

    /**
     * @brief Construct a one-index block_gf from block names and one Green's function copied into every block.
     * 
     * @param b Block names.
     * @param g Green's function to copy into each block.
     */
    block_gf(block_names_t b, g_t const &g)
      requires(Arity == 1)
       : _block_names(std::move(b)), _glist(_block_names.size(), g) {}

    /**
     * @brief Construct a one-index block_gf with the given block names and default-constructed blocks.
     * @param b Block names.
     */
    block_gf(block_names_t b)
      requires(Arity == 1)
       : _block_names(std::move(b)), _glist(_block_names.size()) {}

    /**
     * @brief Construct a one-index block_gf from a mesh and a triqs::gfs::gf_struct_t.
     *
     * @details One block is created per entry of the structure, sized according to the block size (scalar-valued
     * targets ignore the size).
     *
     * @param m Mesh shared by all blocks.
     * @param gf_struct Block structure (block name and size for each block).
     */
    block_gf(Mesh const &m, gf_struct_t const &gf_struct)
      requires(Arity == 1)
    {

      for (auto const &[bl_name, bl_size] : gf_struct) {
        _block_names.push_back(bl_name);
        if constexpr (Target::rank == 0)
          _glist.emplace_back(m);
        else
          _glist.emplace_back(m, make_shape(bl_size, bl_size));
      }
    }

    /**
     * @brief Construct a one-index block_gf from a mesh and a vector of block sizes (block names default to "0", ...).
     * 
     * @tparam Int Integral type of the block sizes.
     * @param m Mesh shared by all blocks.
     * @param bl_sizes Size of each block.
     */
    template <typename Int>
    block_gf(Mesh const &m, std::vector<Int> const &bl_sizes)
      requires(Arity == 1 && std::is_integral_v<Int>)
    {

      for (auto const &[bl, bl_size] : itertools::enumerate(bl_sizes)) {
        _block_names.push_back(std::to_string(bl));
        if constexpr (Target::rank == 0)
          _glist.emplace_back(m);
        else
          _glist.emplace_back(m, make_shape(bl_size, bl_size));
      }
    }

    /**
     * @brief Construct a two-index block2_gf of `n` x `p` copies of a Green's function.
     * 
     * @param n Number of blocks along the first index.
     * @param p Number of blocks along the second index.
     * @param g Green's function to copy into each block.
     */
    block_gf(int n, int p, g_t const &g)
      requires(Arity == 2)
       : _block_names(details::_make_block_names2(n, p)), _glist(n, std::vector<g_t>(p, g)) {}

    /**
     * @brief Construct a two-index block2_gf from a matrix of Green's functions.
     * @param V Blocks (vector of vectors of Green's functions).
     */
    block_gf(data_t V)
      requires(Arity == 2)
       : _block_names(details::_make_block_names2(V.size(), V[0].size())), _glist(std::move(V)) {}

    // ---------------  Operator = --------------------

    /**
     * @brief Copy assignment.
     * 
     * @param rhs Block Green's function to assign from.
     * @return A reference to this block Green's function.
     */
    block_gf &operator=(block_gf const &rhs) = default;

    /**
     * @brief Move assignment.
     * 
     * @param rhs Block Green's function to move from.
     * @return A reference to this block Green's function.
     */
    block_gf &operator=(block_gf &&rhs) = default;

    /**
     * @brief Assign from any object modeling the `BlockGreenFunction` concept.
     *
     * @details The assignment resizes the blocks and block names to match `rhs`, invalidating all existing
     * pointers/views into them. `rhs` can be any object modeling the block Green's function concept, in particular a
     * lazy expression of block Green's functions.
     *
     * @tparam RHS Type of the right hand side; must model `BlockGreenFunction`.
     * @param rhs Object to assign from.
     * @return A reference to this block Green's function.
     */
    template <typename RHS>
      requires(BlockGreenFunction_v<RHS>)
    block_gf &operator=(RHS &&rhs) { // NOLINT(cppcoreguidelines-missing-std-forward): rhs is read block-wise, not forwarded
      if constexpr (Arity == 1) {
        _glist.resize(rhs.size());
        _block_names.resize(rhs.size());
        _assign_impl(rhs);
      } else {
        _block_names.resize(2);
        _glist.resize(rhs.size1());
        for (auto &g_bl : _glist) g_bl.resize(rhs.size2());
        _block_names[0].resize(rhs.size1());
        _block_names[1].resize(rhs.size2());
        _assign_impl(rhs);
      }
      return *this;
    }

    public:
    //----------------------------- print  -----------------------------

    /// Writing a block Green's function to an output stream is not supported.
    friend std::ostream &operator<<(std::ostream &out, block_gf const &) { return out << "block_gf"; }

    /// @cond
    // Friend declarations (hidden from doxygen; documented as free functions in block/mpi.hpp).
    template <BlockGf BG> friend void mpi_broadcast(BG &&, mpi::communicator c, int root);
    template <BlockGf BG1, BlockGf BG2> friend void mpi_reduce_into(BG1 const &, BG2 &&, mpi::communicator, int, bool, MPI_Op);
    /// @endcond

    // Common code for gf, gf_view, gf_const_view
#include "./_block_gf_view_common.hpp"
  };

} // namespace triqs::gfs
