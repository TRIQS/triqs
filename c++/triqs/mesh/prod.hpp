// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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
// Authors: Philipp Dumitrescu, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides a product mesh type.
 */

#pragma once

#include "./concepts.hpp"
#include "./utils.hpp"
#include "../utility/tuple_tools.hpp"

#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

// to silence some clangd warning and make gf independent of triqs::lattice. cf l 64. backward compat check.
namespace triqs::lattice {
  class brillouin_zone;
}

namespace triqs::mesh {

  /**
   * @addtogroup triqs-meshes-prod
   * @{
   */

  // Forward declaration.
  template <Mesh... Ms>
    requires(((n_variables<Ms> == 1) and ...) and (not(std::is_reference_v<Ms> or ...)))
  class prod;

  // Specialize some traits for the product mesh type.
  template <Mesh... Ms> static constexpr bool is_product<prod<Ms...>> = true;
  template <Mesh... Ms> static constexpr int n_variables<prod<Ms...>> = sizeof...(Ms);

  /**
   * @brief %Mesh point type for the triqs::mesh::prod mesh.
   * 
   * @details It inherits from `std::tuple` and in addition to the mesh points of the individual meshes, it stores
   * the corresponding indices in an index tuple \f$ \mathbf{n} \f$, the data indices in a data index tuple \f$
   * \mathbf{d} \f$, and the hash value of the parent mesh.
   */
  template <Mesh... Ms> class prod_mesh_point : public std::tuple<typename Ms::mesh_point_t...> {
    public:
    /// Parent mesh type.
    using mesh_t = prod<Ms...>;

    /// Index type of the parent mesh.
    using index_t = typename mesh_t::index_t;

    /// Data index type of the parent mesh.
    using data_index_t = typename mesh_t::data_index_t;

    /// Underlying tuple type of mesh points.
    using tuple_t = std::tuple<typename Ms::mesh_point_t...>;

    /// Default constructor leaves the mesh point uninitialized.
    prod_mesh_point() = default;

    /**
     * @brief Construct a mesh point from a tuple of mesh points.
     * @param mpt Tuple of mesh points.
     */
    prod_mesh_point(tuple_t mpt) : tuple_t(std::move(mpt)) {}

    /// Get the index tuple \f$ \mathbf{n} \f$ of the mesh point.
    [[nodiscard]] index_t index() const { return _index; }

    /// Get the data index tuple \f$ \mathbf{d} \f$ of the mesh point.
    [[nodiscard]] data_index_t data_index() const { return _data_index; }

    /// Get the hash value of the parent mesh.
    [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }

    /// Get the underlying tuple of mesh points.
    tuple_t const &as_tuple() const { return *this; }

    private:
    index_t _index           = std::apply([](auto &...ms) { return std::make_tuple(ms.index()...); }, as_tuple());
    data_index_t _data_index = std::apply([](auto &...ms) { return std::make_tuple(ms.data_index()...); }, as_tuple());
    uint64_t _mesh_hash      = std::apply([](auto &...ms) { return (ms.mesh_hash() + ...); }, as_tuple());
  };

  //
  template <typename P, typename C> auto make_mesh_range_prod(P const *, C const &m_components, uint64_t) {
    auto f             = [](auto &&...x) { return itertools::product(x...); };
    auto to_mesh_point = [](auto &&n_mp) { return typename P::mesh_point_t{n_mp}; };
    return itertools::transform(std::apply(f, m_components), to_mesh_point);
  }

  /**
   * @brief Product mesh type for combining multiple meshes.
   *
   * @details A product mesh combines various triqs::mesh::Mesh types and satisfies the triqs::mesh::Mesh concept
   * itself. It is defined by the individual meshes in the product and it is iterated over in C-order, i.e. the last
   * mesh in the product is the fastest varying one, while the first mesh is the slowest. In the following, we use \f$
   * M = M_1 \times \dots \times M_k \f$ to denote a product mesh combining the \f$ k \f$ meshes \f$ M_1, \dots, M_k
   * \f$.
   *
   * A product mesh has the following properties:
   * - The number of meshes in the product is \f$ k \f$ and can be queried with triqs::mesh::n_variables.
   * - Its mesh points are tuples of size \f$ k \f$. The i<sup>th</sup> element in the tuple is a mesh point \f$ \in M_i
   * \f$.
   * - Each mesh point is identified by a unique tuple of indices \f$ \mathbf{n} = (n_1, n_2, \dots, n_k) \f$, where
   * \f$ n_i \f$ is a valid index of \f$ M_i \f$.
   * - The size of the mesh is \f$ N = N_1 \dots N_k \f$, where \f$ N_i \f$ is the size of \f$ M_i \f$.
   * - An index \f$ \mathbf{n} \f$ is mapped to the corresponding data index \f$ \mathbf{d}(\mathbf{n}) = (d_1(n_1),
   * \dots, d_k(n_k)) \f$, where \f$ d_i \f$ is a data index of \f$ M_i \f$. The inverse map is \f$ \mathbf{n}(
   * \mathbf{d}) = (n_1(d_1), \dots, n_k(d_k)) \f$.
   *
   * @ref triqs-gfs containers that are based on a product mesh store function values, coefficients, etc. depending on
   * the mesh types of its components. For example, a GF defined on a product mesh consisting of a triqs::mesh::brzone
   * and a triqs::mesh::dlr_imtime mesh stores the function values at the DLR interpolation nodes \f$ \tau_l \f$ for
   * every \f$ \mathbf{k} \f$-point. Function evaluation is done in general by calling
   * @ref triqs::mesh::evaluate(mesh::prod<Ms...> const &m, auto const &f, Xs const &...xs) "triqs::mesh::evaluate".
   *
   * @code
   * #include <fmt/ranges.h>
   * #include <triqs/mesh.hpp>
   *
   * #include <numbers>
   *
   * int main() {
   *   using std::numbers::pi;
   *
   *   // initialize a mesh on a bravais lattice with a = 1 and a 2x2x1 meshpoints
   *   auto bl   = triqs::mesh::bravais_lattice{nda::eye<double>(3)};
   *   auto bl_m = triqs::mesh::cyclat bl_m{bl, {2, 2, 1}};
   *
   *   // initialize a bosonic imaginary frequency mesh with 2 positive frequencies and beta = 10
   *   triqs::mesh::imfreq iw_m{10, triqs::mesh::Boson, 2};
   *
   *   // combine the two meshes into a product mesh
   *   auto m = triqs::mesh::prod{bl_m, iw_m};
   *
   *   // loop over all mesh points and print their index and data index tuples
   *   for (int i = 0; auto mp : m) { fmt::println("mesh point #{}: index = {}, data index = {}", i++, mp.index(), mp.data_index()); }
   * }
   * @endcode
   *
   * Output:
   *
   * ```
   * mesh point #0: index = ([0, 0, 0], -1), data index = (0, 0)
   * mesh point #1: index = ([0, 0, 0], 0), data index = (0, 1)
   * mesh point #2: index = ([0, 0, 0], 1), data index = (0, 2)
   * mesh point #3: index = ([0, 1, 0], -1), data index = (1, 0)
   * mesh point #4: index = ([0, 1, 0], 0), data index = (1, 1)
   * mesh point #5: index = ([0, 1, 0], 1), data index = (1, 2)
   * mesh point #6: index = ([1, 0, 0], -1), data index = (2, 0)
   * mesh point #7: index = ([1, 0, 0], 0), data index = (2, 1)
   * mesh point #8: index = ([1, 0, 0], 1), data index = (2, 2)
   * mesh point #9: index = ([1, 1, 0], -1), data index = (3, 0)
   * mesh point #10: index = ([1, 1, 0], 0), data index = (3, 1)
   * mesh point #11: index = ([1, 1, 0], 1), data index = (3, 2)
   * ```
   *
   * @tparam Ms triqs::mesh::Mesh types.
   */
  template <Mesh... Ms>
    requires(((n_variables<Ms> == 1) and ...) and (not(std::is_reference_v<Ms> or ...)))
  class prod : public std::tuple<Ms...> {
    public:
    /// Index type.
    using index_t = std::tuple<typename Ms::index_t...>;

    /// Data index type.
    using data_index_t = std::tuple<typename Ms::data_index_t...>;

    /// %Mesh point type.
    using mesh_point_t = prod_mesh_point<Ms...>;

    /// Underlying tuple type.
    using m_tuple_t = std::tuple<Ms...>;

    /// Default constructor calls the default constructor of all its components.
    prod() = default;

    /**
     * @brief Construct a product mesh with the given meshes \f$ M_1, \dots, M_k \f$.
     * @param ms Meshes to be combined into a product mesh \f$ M = M_1 \times \dots \times M_k \f$.
     */
    prod(Ms const &...ms)
      requires(sizeof...(Ms) > 0)
       : m_tuple_t{ms...}, hash_((ms.mesh_hash() + ...)) {}

    /**
     * @brief Construct a product mesh \f$ M = M_1 \times \dots \times M_k \f$ from the given tuple of meshes.
     * @param mt Tuple of meshes, i.e. \f$ (M_1, \dots, M_k) \f$.
     */
    template <typename... U>
    prod(std::tuple<U...> const &mt) : m_tuple_t{mt}, hash_(std::apply([](auto &...m) { return (m.mesh_hash() + ...); }, mt)) {}

    /// Equal-to comparison operator compares the tuple of meshes in the product.
    bool operator==(prod const &m) const { return as_tuple() == m.as_tuple(); }

    /// Not-equal-to comparison operator compares the tuple of meshes in the product.
    bool operator!=(prod const &m) const = default;

    /**
     * @brief Check if the given indices \f$ n_1, \dots, n_k \f$ are valid.
     *
     * @param ns Indices \f$ n_1, \dots, n_k \f$ to check.
     * @return True if \f$ 0 \leq n_i < N_i \; \forall i \f$, false otherwise.
     */
    template <typename... Args> [[nodiscard]] bool is_index_valid(Args const &...ns) const {
      return triqs::tuple::fold([](auto &m, auto &n, bool res) { return res && (m.is_index_valid(n)); }, as_tuple(), std::tie(ns...), true);
    }

    /**
     * @brief Check if given index tuple \f$ \mathbf{n} = (n_1, \dots, n_k) \f$ is valid.
     *
     * @param n Index tuple \f$ \mathbf{n} \f$ to check.
     * @return True if \f$ 0 \leq n_i < N_i \; \forall i \f$, false otherwise.
     */
    template <typename... Args> [[nodiscard]] bool is_index_valid(index_t const &n) const {
      return triqs::tuple::fold([](auto &m, auto &n, bool res) { return res && (m.is_index_valid(n)); }, as_tuple(), n, true);
    }

    /**
     * @brief Map an index tuple \f$ \mathbf{n} \f$ to its corresponding data index tuple \f$ \mathbf{d}(\mathbf{n})
     * \f$.
     *
     * @param n Index tuple \f$ \mathbf{n} = (n_1, \dots, n_k) \f$ to map.
     * @return Data index tuple \f$ \mathbf{d}(\mathbf{n}) = (d_1(n_1), \dots, d_k(n_k)) \f$.
     */
    [[nodiscard]] data_index_t to_data_index(index_t const &n) const {
      auto l = [](auto const &m, auto const &n_i) { return m.to_data_index(n_i); };
      return triqs::tuple::map_on_zip(l, *this, n);
    }

    /**
     * @brief Map a data index tuple \f$ \mathbf{d} \f$ to its corresponding index tuple \f$ \mathbf{n}(\mathbf{d})
     * \f$.
     *
     * @param d Data index tuple \f$ \mathbf{d} = (d_1, \dots, d_k) \f$ to map.
     * @return Index tuple \f$ \mathbf{n}(\mathbf{d}) = (n_1(d_1), \dots, n_k(d_k)) \f$.
     */
    [[nodiscard]] index_t to_index(data_index_t const &d) const {
      auto l = [](auto const &m, auto const &d_i) { return m.to_index(d_i); };
      return triqs::tuple::map_on_zip(l, *this, d);
    }

    /**
     * @brief Subscript operator to access a mesh point by its data index tuple \f$ \mathbf{d} \f$.
     *
     * @details Each data index \f$ d_i \f$ in the tuple is used to access the mesh point in the corresponding mesh 
     * component \f$ M_i \f$.
     *
     * @param d Data index tuple \f$ \mathbf{d} \f$ of the mesh point.
     * @return mesh_point_t containing the mesh points of the components.
     */
    [[nodiscard]] mesh_point_t operator[](data_index_t const &d) const {
      auto l = [](auto const &m, auto const &d_i) { return m[d_i]; };
      return triqs::tuple::map_on_zip(l, *this, d);
    }

    /**
     * @brief Function call operator to access a mesh point by its index tuple \f$ \mathbf{n} \f$.
     *
     * @details Each index \f$ n_i \f$ in the tuple is used to access the mesh point in the corresponding mesh 
     * component \f$ M_i \f$.
     * 
     * @param n Index tuple \f$ \mathbf{n} \f$ of the mesh point.
     * @return mesh_point_t containing the mesh points of the components.
     */
    [[nodiscard]] mesh_point_t operator()(index_t const &n) const {
      auto l = [](auto const &m, auto const &n_i) { return m(n_i); };
      return triqs::tuple::map_on_zip(l, *this, n);
    }

    /// Get the hash value of the mesh.
    [[nodiscard]] uint64_t mesh_hash() const { return hash_; }

    /// Get the size \f$ N \f$ of the mesh, i.e. the product of the sizes of its components.
    [[nodiscard]] long size() const {
      return triqs::tuple::fold([](auto const &m, size_t n) { return n * m.size(); }, as_tuple(), 1);
    }

    /// Get the sizes of the components as a `std::array`, i.e. \f$ (N_1, \dots, N_k) \f$.
    [[nodiscard]] auto size_of_components() const {
      std::array<long, std::tuple_size_v<m_tuple_t>> res;
      auto l = [&res](int i, auto const &m) mutable { res[i] = m.size(); };
      triqs::tuple::for_each_enumerate(as_tuple(), l);
      return res;
    }

    // Get the underlying tuple of individual meshes, i.e. \f$ (M_1, \dots, M_k) \f$.
    [[nodiscard]] m_tuple_t const &components() const { return *this; }

    /// Get the underlying tuple of individual meshes, i.e. \f$ (M_1, \dots, M_k) \f$.
    [[nodiscard]] m_tuple_t &components() { return *this; }

    /// Get the underlying tuple of individual meshes, i.e. \f$ (M_1, \dots, M_k) \f$.
    [[nodiscard]] m_tuple_t const &as_tuple() const { return *this; }

    /// Get the underlying tuple of individual meshes, i.e. \f$ (M_1, \dots, M_k) \f$.
    [[nodiscard]] m_tuple_t &as_tuple() { return *this; }

    private:
    // Get a lazy range that generates the mesh points.
    [[nodiscard]] auto r_() const {
      auto f             = [](auto &&...ms) { return itertools::product(ms...); };
      auto to_mesh_point = [](auto &&n_mp) { return mesh_point_t{n_mp}; };
      return itertools::transform(std::apply(f, components()), to_mesh_point);
    }

    public:
    /// Get an iterator to the beginning of the mesh.
    [[nodiscard]] auto begin() const { return r_().begin(); }

    /// Get a const iterator to the beginning of the mesh.
    [[nodiscard]] auto cbegin() const { return r_().cbegin(); }

    /// Get an iterator to the end of the mesh.
    [[nodiscard]] auto end() const { return r_().end(); }

    /// Get a const iterator to the end of the mesh.
    [[nodiscard]] auto cend() const { return r_().cend(); }

    /**
     * @brief Write a triqs::mesh::prod mesh to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param m %Mesh to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, prod const &m) {
      sout << "Product Mesh";
      triqs::tuple::for_each(m.as_tuple(), [&sout](auto &mesh) { sout << "\n  -- " << mesh; });
      return sout;
    }

    /**
     * @brief Serialize the mesh to a generic archive.
     * @param ar Archive to serialize to.
     */
    void serialize(auto &ar) const { ar &as_tuple(); }

    /**
     * @brief Deserialize the mesh from a generic archive.
     * @param ar Archive to deserialize from.
     */
    void deserialize(auto &ar) { ar &as_tuple(); }

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "MeshProduct"; }

    /**
     * @brief Write a triqs::mesh::prod mesh to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be written.
     */
    friend void h5_write(h5::group g, std::string name, prod const &m) {
      h5::group gr = g.create_group(name);
      h5::write_hdf5_format(gr, m);
      auto l = [gr](int N, auto const &mc) { h5::write(gr, "MeshComponent" + std::to_string(N), mc); };
      triqs::tuple::for_each_enumerate(m.components(), l);
    }

    /**
     * @brief Read a triqs::mesh::prod mesh from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be read into.
     */
    friend void h5_read(h5::group g, std::string name, prod &m) {
      h5::group gr = g.open_group(name);
      h5::assert_hdf5_format(gr, m, true);
      auto l = [gr](int N, auto &mc) { h5::read(gr, "MeshComponent" + std::to_string(N), mc); };
      triqs::tuple::for_each_enumerate(m.components(), l);
    }

    private:
    uint64_t hash_ = 0;
  };

  // Class template argument deduction rules (CTAD).
  template <typename M1, typename M2, typename... Ms> prod(M1, M2, Ms...) -> prod<M1, M2, Ms...>;

  template <typename M1, typename M2, typename... Ms>
  prod(std::tuple<M1, M2, Ms...>) -> prod<std::decay_t<M1>, std::decay_t<M2>, std::decay_t<Ms>...>;

  /**
   * @brief Multiplication operator for two product meshes.
   *
   * @tparam Ls %Mesh types of the left-hand side product mesh.
   * @tparam Rs %Mesh types of the right-hand side product mesh.
   * @param m Left-hand side product mesh, i.e. \f$ M_1 \times \dots \times M_k \f$.
   * @param n Right-hand side product mesh, i.e. \f$ N_1 \times \dots \times N_l \f$.
   * @return A product mesh containing the components of both meshes, i.e. \f$ M_1 \times \dots \times M_k \times N_1 
   * \times \dots \times N_l \f$.
   */
  template <Mesh... Ls, Mesh... Rs> auto operator*(prod<Ls...> const &m, prod<Rs...> const &n) {
    return prod<Ls..., Rs...>{std::tuple_cat(m.components(), n.components())};
  }

  /**
   * @brief Multiplication operator for a single mesh and a product mesh.
   *
   * @tparam L %Mesh type of the left-hand side single mesh.
   * @tparam Rs %Mesh types of the right-hand side product mesh.
   * @param n Left-hand side single mesh \f$ N \f$.
   * @param m Right-hand side product mesh \f$ M = M_1 \times \dots \times M_k \f$.
   * @return A product mesh containing the single mesh as well as the components of the product meshes, i.e. \f$ 
   * N \times M_1 \times \dots \times M_k \f$.
   */
  template <Mesh L, Mesh... Rs> auto operator*(L const &n, prod<Rs...> const &m) {
    return prod<L, Rs...>{std::tuple_cat(std::make_tuple(n), m.components())};
  }

  /**
   * @brief Multiplication operator for a product mesh and a single mesh.
   *
   * @tparam Ls %Mesh types of the left-hand side product mesh.
   * @tparam R %Mesh type of the right-hand side single mesh.
   * @param m Left-hand side product mesh \f$ M = M_1 \times \dots \times M_k \f$.
   * @param n Right-hand side single mesh \f$ N \f$.
   * @return A product mesh containing the components of the product meshes as well as the single mesh, i.e. \f$ M_1 
   * \times \dots \times M_k \times N \f$.
   */
  template <Mesh... Ls, Mesh R> auto operator*(prod<Ls...> const &m, R const &n) {
    return prod<Ls..., R>{std::tuple_cat(m.components(), std::make_tuple(n))};
  }

  /**
   * @brief Multiplication operator for two single meshes.
   *
   * @tparam L %Mesh type of the left-hand side mesh.
   * @tparam R %Mesh type of the right-hand side mesh.
   * @param m Left-hand side mesh \f$ M \f$.
   * @param n Right-hand side mesh \f$ N \f$.
   * @return A product mesh containing both meshes as components, i.e. \f$ M \times N \f$.
   */
  template <Mesh L, Mesh R> auto operator*(L const &m, R const &n) { return prod<L, R>{m, n}; }

  /** @} */

} // namespace triqs::mesh

/// Specialize `std::tuple_size` for triqs::mesh::prod types.
template <typename... Ms> struct std::tuple_size<triqs::mesh::prod<Ms...>> : public std::integral_constant<size_t, sizeof...(Ms)> {};

/// Specialize `std::tuple_size` for triqs::mesh::prod_mesh_point types.
template <typename... Ms> struct std::tuple_size<triqs::mesh::prod_mesh_point<Ms...>> : public std::integral_constant<size_t, sizeof...(Ms)> {};

/// Specialize `std::tuple_element` for triqs::mesh::prod types.
template <size_t N, typename... Ms> struct std::tuple_element<N, triqs::mesh::prod<Ms...>> : public std::tuple_element<N, std::tuple<Ms...>> {};

/// Specialize `std::tuple_element` for triqs::mesh::prod_mesh_point types.
template <size_t N, typename... Ms>
struct std::tuple_element<N, triqs::mesh::prod_mesh_point<Ms...>> : public std::tuple_element<N, std::tuple<typename Ms::mesh_point_t...>> {};
