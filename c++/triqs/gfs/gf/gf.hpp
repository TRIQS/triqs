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
 * @brief Provides the Green's function class.
 */

#pragma once

#include "./defs.hpp"
#include "./targets.hpp"
#include "../../utility/concept_tools.hpp"
#include "../../utility/exceptions.hpp" // IWYU pragma: keep (used by ./_gf_view_common.hpp)
#include "../../utility/macros.hpp"     // IWYU pragma: keep (used by ./_gf_view_common.hpp)

#include <itertools/itertools.hpp>
#include <mpi/mpi.hpp>
#include <nda/nda.hpp>

#include <array>
#include <concepts>
#include <numeric> // IWYU pragma: keep (used by ./_gf_view_common.hpp)
#include <ostream> // IWYU pragma: keep (used by ./_gf_view_common.hpp)
#include <string>  // IWYU pragma: keep (used by ./_gf_view_common.hpp)
#include <tuple>   // IWYU pragma: keep (used by ./_gf_view_common.hpp)
#include <type_traits>
#include <utility>

namespace triqs::gfs {

  // Elevate certain types into the triqs::gfs namespace.
  using nda::C_layout;
  using nda::C_stride_layout;

  // Forward declarations.
  template <Mesh M, typename Target = matrix_valued, typename Layout = C_layout> class gf;
  template <Mesh M, typename Target = matrix_valued, typename Layout = C_stride_layout> class gf_view;
  template <Mesh M, typename Target = matrix_valued, typename Layout = C_stride_layout> class gf_const_view;

  /**
   * @addtogroup triqs-gfs-utils
   * @{
   */

  // Trait and concept tag marking a type as a Green's function.
  TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT(GreenFunction);

  /**
   * @brief Trait to detect whether a type is an instantiation of a given class template.
   *
   * @details The nested `value` member is `true` if and only if `T` is of the form `TMPLT<U...>` for some parameter
   * pack `U...`.
   *
   * @tparam TMPLT Class template to test against.
   * @tparam T Type to test.
   */
  template <template <typename...> class TMPLT, typename T> struct is_instantiation_of : std::false_type {};

  // Specialization of is_instantiation_of for a matching instantiation.
  template <template <typename...> class TMPLT, typename... U> struct is_instantiation_of<TMPLT, TMPLT<U...>> : std::true_type {};

  /**
   * @brief Variable template for triqs::gfs::is_instantiation_of, decaying the tested type first.
   *
   * @tparam gf Class template to test against.
   * @tparam T Type to test (cvref-qualifiers are stripped).
   */
  template <template <typename...> class gf, typename T>
  inline constexpr bool is_instantiation_of_v = is_instantiation_of<gf, std::decay_t<T>>::value;

  //template <typename G> using mesh_t_of = std::decay_t<decltype(std::declval<G>().mesh())>;

  /**
   * @brief Trait to check whether a type models the Green's function concept.
   *
   * @details It is `true` if `G` (after stripping cvref-qualifiers) is one of triqs::gfs::gf, triqs::gfs::gf_view or 
   * triqs::gfs::gf_const_view. 
   * 
   * When a second template argument `M` is supplied, it additionally requires the mesh type of `G` to be `M`.
   *
   * @deprecated In favour of the triqs::gfs::MemoryGf concept.
   *
   * @tparam G Type to test.
   * @tparam M Optional mesh type to require.
   */
  template <typename G, typename M = void> inline constexpr bool is_gf_v = false;

  // Specialization of is_gf_v for cvref-qualified types, which decays G first.
  template <typename G, typename M>
    requires(!std::is_same_v<G, std::remove_cvref_t<G>>)
  inline constexpr bool is_gf_v<G, M> = is_gf_v<std::remove_cvref_t<G>, M>;

  // Specialization of is_gf_v without a mesh constraint.
  template <typename G>
  inline constexpr bool is_gf_v<G, void> =
     is_instantiation_of_v<gf, G> or is_instantiation_of_v<gf_view, G> or is_instantiation_of_v<gf_const_view, G>;

  // Specialization of is_gf_v requiring the mesh type of G.
  template <typename G> inline constexpr bool is_gf_v<G, typename std::remove_cvref_t<G>::mesh_t> = is_gf_v<G, void>;

  /** @} */

  /**
   * @ingroup triqs-gfs-concepts
   * @brief Concept checking that a type behaves like an in-memory Green's function.
   *
   * @details A type `G` models triqs::gfs::MemoryGf if 
   * 
   * - it owns a mesh of type `M` (a triqs::mesh::Mesh) accessible via `mesh()` and 
   * - a data array accessible via `data()` that models `nda::MemoryArray`. 
   * 
   * This is the constraint used throughout the Green's function algorithms.
   *
   * @tparam G Type to check.
   * @tparam M Expected mesh type.
   */
  template <typename G, typename M = typename std::remove_cvref_t<G>::mesh_t>
  concept MemoryGf = mesh::Mesh<M> and requires(G g) {
    { g.data() } -> nda::MemoryArray;
    requires std::same_as<std::decay_t<decltype(g.mesh())>, M>;
  };

  // Forward declarations with default arguments. The corresponding friend declarations inside `gf` and
  // the definitions in `./mpi.hpp` may then re-declare these templates without re-introducing defaults.
  template <MemoryGf G> void mpi_broadcast(G &&g, mpi::communicator c = {}, int root = 0);
  template <MemoryGf G1, MemoryGf G2>
  void mpi_reduce_into(G1 const &g_in, G2 &&g_out, mpi::communicator c = {}, int root = 0, bool all = false, MPI_Op op = MPI_SUM);

  /**
   * @addtogroup triqs-gfs-utils
   * @{
   */

  /// The target type of a Green's function type `G`.
  template <typename G> using get_target_t = typename std::decay_t<G>::target_t;

  /**
   * @brief Get the mesh of a Green's function, or its N-th component for a product mesh.
   *
   * @tparam N Index of the mesh component to return (default 0). Ignored for non-product meshes.
   * @tparam G The type of the Green's function.
   * @param g The Green's function.
   * @return A const reference to the (N-th component of the) mesh.
   */
  template <int N = 0, MemoryGf G> auto const &get_mesh(G const &g) {
    if constexpr (mesh::is_product<typename G::mesh_t>)
      return std::get<N>(g.mesh());
    else
      return g.mesh();
  }

  /// The type of value obtained when accessing a Green's function of type `G` with a mesh point.
  template <MemoryGf G> using target_value_t = decltype(std::declval<G>()[std::declval<typename G::mesh_point_t>()]);

  /** @} */

  // Tags used internally to select implementation-detail constructors.
  struct impl_tag {};
  struct impl_tag2 {};

  /**
   * @ingroup triqs-gfs-containers
   * @brief The owning Green's function container.
   *
   * @details triqs::gfs::gf is the central, value-semantic container of the Green's function module. It owns a mesh
   * and a data array, and represents a function defined on the mesh whose value at each mesh point is fixed by the
   * `Target` type (see triqs::gfs::scalar_valued, triqs::gfs::matrix_valued, triqs::gfs::tensor_valued).
   *
   * The data array has rank `arity + Target::rank`, where `arity` is the number of mesh dimensions. The function can
   * be accessed at a mesh point with `operator[]` and evaluated at an arbitrary point with `operator()` (using the
   * interpolation scheme of the mesh). 
   * 
   * Non-owning views are provided by triqs::gfs::gf_view and triqs::gfs::gf_const_view.
   *
   * @tparam M Mesh type, modeling triqs::mesh::Mesh; fixes the domain of definition.
   * @tparam Target Target type; fixes the value stored at each mesh point.
   * @tparam Layout Memory layout policy of the data array.
   */
  template <Mesh M, typename Target, typename Layout> class gf : TRIQS_CONCEPT_TAG_NAME(GreenFunction) {

    static_assert(not std::is_same_v<M, triqs::lattice::brillouin_zone>,
                  "Since TRIQS 2.3, brillouin_zone is replaced by mesh::brzone as a mesh name. Cf Doc, changelog");

    using this_t = gf<M, Target, Layout>; // used in common code

    public:
    /// Whether this type is a view (always `false` for triqs::gfs::gf).
    static constexpr bool is_view = false;

    /// Whether this type is a const view (always `false` for triqs::gfs::gf).
    static constexpr bool is_const = false;

    /// Associated mutable view type.
    using mutable_view_type = gf_view<M, Target, typename Layout::with_lowest_guarantee_t>;

    /// Associated const view type.
    using const_view_type = gf_const_view<M, Target, typename Layout::with_lowest_guarantee_t>;

    /// Associated (non const) view type.
    using view_type = gf_view<M, Target, typename Layout::with_lowest_guarantee_t>;

    /// Associated regular (owning) type.
    using regular_type = gf<M, Target, Layout>;

    /// Associated real-valued type.
    using real_t = gf<M, typename Target::real_t, Layout>;

    /// Associated complex-valued type.
    using complex_t = gf<M, typename Target::complex_t, Layout>;

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

    /// Rank of the data array representing the function.
    static constexpr int data_rank = arity + Target::rank;

    /// Type of the data array.
    using data_t = nda::basic_array<scalar_t, data_rank, Layout, 'A', nda::heap<>>;

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
     * @brief Get the data array.
     * @return A reference to the data array.
     */
    data_t &data() & { return _data; }

    /**
     * @brief Get the data array (const overload).
     * @return A const reference to the data array.
     */
    data_t const &data() const & { return _data; }

    /**
     * @brief Get the data array (rvalue overload).
     * @return The data array, moved out of the (expiring) Green's function.
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
     * @return A triqs::gfs::gf::target_and_shape_t bundling the target type and shape (the mesh dimensions are dropped).
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
    /// Construct an empty Green's function (with an empty mesh and data array).
    gf() = default; // {}

    /**
     * @brief Copy constructor.
     * @param x Green's function to copy from.
     */
    gf(gf const &x) = default;

    /// Move constructor.
    gf(gf &&) = default;

    private:
    void swap_impl(gf &b) noexcept {
      using std::swap;
      swap(this->_mesh, b._mesh);
      swap(this->_data, b._data);
    }

    private:
    // Compute the shape of the data array from the mesh and the target shape.
    static auto make_data_shape(mesh_t const &m, target_shape_t const &shape) {
      if constexpr (mesh::is_product<mesh_t>)
        return stdutil::join(m.size_of_components(), shape);
      else
        return stdutil::front_append(shape, m.size());
    }

    public:
    /**
     * @brief Construct from a mesh and a data array.
     *
     * @note The mesh and data array are passed by value and moved into the Green's function.
     *
     * @param m Mesh.
     * @param dat Data array.
     */
    gf(mesh_t m, data_t dat) : _mesh(std::move(m)), _data(std::move(dat)) {}

    /**
     * @brief Construct from a mesh and a target shape.
     *
     * @details The data array is allocated to match the mesh and the requested target shape, and value-initialized.
     *
     * @param m Mesh.
     * @param shape Target shape (defaults to an empty shape, appropriate for scalar targets).
     */
    gf(mesh_t m, target_shape_t shape = {}) : _mesh(std::move(m)), _data(make_data_shape(_mesh, shape)) {}

    /**
     * @brief Construct from a view, making a deep copy of the data.
     * @param g View to copy from.
     */
    explicit gf(gf_view<M, Target> const &g) : _mesh(g.mesh()), _data(g.data()) {}

    /**
     * @brief Construct from a const view, making a deep copy of the data.
     * @param g Const view to copy from.
     */
    explicit gf(gf_const_view<M, Target> const &g) : _mesh(g.mesh()), _data(g.data()) {}

    /**
     * @brief Construct from any object modeling the `GreenFunction` concept with the same mesh type.
     *
     * @details The Green's function is default constructed and then assigned from `g`, which resizes the mesh and data
     * accordingly. The constructor is explicit to avoid unintended conversions.
     *
     * @tparam G A type modeling `GreenFunction` with the same mesh type.
     * @param g Object to construct from (e.g. another Green's function or a lazy expression).
     */
    template <typename G>
    explicit gf(G const &g)
      requires(GreenFunction<G>::value and std::is_same_v<mesh_t, typename G::mesh_t>)
       : gf() {
      *this = g;
    } // explicit is very important here.
    // TODO: We would like to refine this, G should have the same mesh, target, at least ...

    // ---------------  Operator = --------------------

    /**
     * @brief Copy assignment.
     * @param rhs Green's function to assign from.
     * @return A reference to this Green's function.
     */
    gf &operator=(gf const &rhs) = default;

    /**
     * @brief Move assignment.
     * @param rhs Green's function to move from.
     * @return A reference to this Green's function.
     */
    gf &operator=(gf &&rhs) noexcept {
      this->swap_impl(rhs);
      return *this;
    }

    /**
     * @brief Assign from any object modeling the `GreenFunction` concept.
     *
     * @details The assignment resizes the mesh and the data to match `rhs`, invalidating all existing pointers/views
     * into them. `rhs` can be any object modeling the Green's function concept, in particular a lazy expression of
     * Green's functions.
     *
     * @tparam RHS Type of the right hand side; must model `GreenFunction`.
     * @param rhs Object to assign from.
     * @return A reference to this Green's function.
     */
    template <typename RHS>
    gf &operator=(RHS &&rhs) // NOLINT
      requires(GreenFunction<RHS>::value and not std::is_same_v<std::decay_t<RHS>, gf>)
    {
      _mesh = rhs.mesh();
      _data.resize(rhs.data_shape());
      for (auto w : _mesh) (*this)[w] = rhs[w];
      return *this;
    }

    // other = late, cf MPI

    public:
    // ------------- apply_on_data -----------------------------

    /**
     * @brief Build a view of a Green's function whose data is the result of applying a function to the data array.
     *
     * @details The target type of the returned view is deduced from the rank of the transformed array via
     * triqs::gfs::target_from_array.
     *
     * @tparam Fdata Type of the callable applied to the data array.
     * @param fd Callable taking the data array and returning a transformed array.
     * @return A triqs::gfs::gf_view onto the transformed data, sharing this Green's function's mesh.
     */
    template <typename Fdata> auto apply_on_data(Fdata &&fd) {
      auto d2    = std::forward<Fdata>(fd)(_data);
      using t2   = target_from_array<decltype(d2), arity>;
      using gv_t = gf_view<M, t2>;
      return gv_t{mesh(), d2};
    }

    /**
     * @brief Build a const view of a Green's function whose data is the result of applying a function to the data array.
     *
     * @details Const overload of triqs::gfs::gf::apply_on_data. The target type of the returned view is deduced from
     * the rank of the transformed array via triqs::gfs::target_from_array.
     *
     * @tparam Fdata Type of the callable applied to the data array.
     * @param fd Callable taking the data array and returning a transformed array.
     * @return A triqs::gfs::gf_const_view onto the transformed data, sharing this Green's function's mesh.
     */
    template <typename Fdata> auto apply_on_data(Fdata &&fd) const {
      auto d2    = std::forward<Fdata>(fd)(_data);
      using t2   = target_from_array<decltype(d2), arity>;
      using gv_t = gf_const_view<M, t2>;
      return gv_t{mesh(), d2};
    }

    // Friend declarations (hidden from doxygen; documented as free functions in gf/mpi.hpp).
    /// @cond
    template <MemoryGf G> friend void mpi_broadcast(G &&, mpi::communicator, int root);
    template <MemoryGf G1, MemoryGf G2> friend void mpi_reduce_into(G1 const &, G2 &&, mpi::communicator, int, bool, MPI_Op);
    /// @endcond

    // Common code for gf, gf_view, gf_const_view
#include "./_gf_view_common.hpp"
  };

  /*------------------------------------------------------------------------
   *   Deduction guides
   *-----------------------------------------------------------------------*/

  /// Deduce a triqs::gfs::gf type from a mesh and a data array.
  template <Mesh M, nda::MemoryArray DataArray> gf(M, DataArray) -> gf<M, target_from_array<DataArray, n_variables<M>>>;

  /// Deduce a triqs::gfs::gf type from a mesh and a target shape (complex valued, target rank `R`).
  template <Mesh M, std::integral I, size_t R> gf(M, std::array<I, R>) -> gf<M, typename _target_from_type_rank<dcomplex, R>::type>;

  /// Deduce a (scalar-valued) triqs::gfs::gf type from a mesh alone.
  template <Mesh M> gf(M) -> gf<M, scalar_valued>;

  // Forward declare gf_expr
  template <typename Tag, typename L, typename R> struct gf_expr;

  /// Deduce a triqs::gfs::gf type from an rvalue Green's function expression.
  template <typename Tag, typename L, typename R>
  gf(gf_expr<Tag, L, R> &&) -> gf<typename gf_expr<Tag, L, R>::mesh_t, typename gf_expr<Tag, L, R>::target_t>;

  /// Deduce a triqs::gfs::gf type from a const Green's function expression.
  template <typename Tag, typename L, typename R>
  gf(gf_expr<Tag, L, R> const &) -> gf<typename gf_expr<Tag, L, R>::mesh_t, typename gf_expr<Tag, L, R>::target_t>;

} // namespace triqs::gfs
