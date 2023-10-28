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

#pragma once
#include "utils.hpp"

namespace triqs::lattice { // to silence some warning of clangd and make gf independent of lattice. cf l 64. backward compat check.
  class brillouin_zone;
}

namespace triqs::mesh {

  template <Mesh... Ms>
    requires((n_variables<Ms> == 1) and ...)
  class prod;
  template <Mesh... Ms> static constexpr bool is_product<prod<Ms...>> = true;
  template <Mesh... Ms> static constexpr int n_variables<prod<Ms...>> = sizeof...(Ms);

  template <Mesh... Ms> struct prod_mesh_point : public std::tuple<typename Ms::mesh_point_t...> {
    using mesh_t       = prod<Ms...>;
    using index_t      = typename mesh_t::index_t;
    using data_index_t = typename mesh_t::data_index_t;
    using tuple_t      = std::tuple<typename Ms::mesh_point_t...>;

    private:
    index_t _index           = std::apply([](auto &...x) { return std::make_tuple(x.index()...); }, *this);
    data_index_t _data_index = std::apply([](auto &...x) { return std::make_tuple(x.data_index()...); }, *this);
    uint64_t _mesh_hash      = std::apply([](auto &...x) { return (x.mesh_hash() + ...); }, *this);

    public:
    prod_mesh_point() = default;
    prod_mesh_point(tuple_t mpt) : tuple_t(std::move(mpt)) {}

    /// The index of the mesh point
    [[nodiscard]] index_t index() const { return _index; }

    /// The data index of the mesh point
    [[nodiscard]] data_index_t data_index() const { return _data_index; }

    /// The Hash for the mesh configuration
    [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }

    tuple_t const &as_tuple() const { return *this; }
  };

  //
  template <typename P, typename C> auto make_mesh_range_prod([[maybe_unused]] P const *m, C const &m_components, [[maybe_unused]] uint64_t mhash) {
    auto f             = [](auto &&...x) { return itertools::product(x...); };
    auto to_mesh_point = [](auto &&n_mp) { return typename P::mesh_point_t{n_mp}; };
    return itertools::transform(std::apply(f, m_components), to_mesh_point);
  }

  /** Cartesian product of mesh */
  template <Mesh... Ms>
    requires((n_variables<Ms> == 1) and ...)
  class prod : public std::tuple<Ms...> {

    static constexpr int dim = sizeof...(Ms);
    // Backward compat helper.
    static_assert(not(std::is_reference_v<Ms> or ...), "Cannot create mesh product of references");
    static_assert(not(std::is_same_v<Ms, triqs::lattice::brillouin_zone> or ...),
                  "Since TRIQS 2.3, brillouin_zone is replaced by mesh::brzone as a mesh name. Cf Doc, changelog");

    public:
    using index_t      = std::tuple<typename Ms::index_t...>;
    using data_index_t = std::tuple<typename Ms::data_index_t...>;
    using mesh_point_t = prod_mesh_point<Ms...>;
    using m_tuple_t    = std::tuple<Ms...>;

    // -------------------- Constructors -------------------

    prod() = default;

    prod(Ms const &...mesh)
      requires(sizeof...(Ms) > 0)
       : m_tuple_t{mesh...} {}

    template <typename... U> prod(std::tuple<U...> const &mesh_tuple) : m_tuple_t{mesh_tuple} {}

    /// Mesh comparison
    bool operator==(prod const &m) const { return as_tuple() == m.as_tuple(); }
    bool operator!=(prod const &m) const = default;

    // -------------------- size -------------------

    /// size of the mesh is the product of size
    [[nodiscard]] long size() const {
      return triqs::tuple::fold([](auto const &m, size_t R) { return R * m.size(); }, as_tuple(), 1);
    }

    /// The sizes of all mesh components
    std::array<long, dim> size_of_components() const {
      std::array<long, dim> res;
      auto l = [&res](int i, auto const &m) mutable { res[i] = m.size(); };
      triqs::tuple::for_each_enumerate(as_tuple(), l);
      return res;
    }

    // -------------------- checks -------------------

    /// Is the point of evaluation in the mesh. All components must be in the corresponding mesh.
    template <typename... Args> bool is_index_valid(Args const &...args) const {
      return triqs::tuple::fold([](auto &m, auto &arg, bool r) { return r && (m.is_index_valid(arg)); }, as_tuple(), std::tie(args...), true);
    }
    template <typename... Args> bool is_index_valid(index_t const &index) const {
      return triqs::tuple::fold([](auto &m, auto &arg, bool r) { return r && (m.is_index_valid(arg)); }, as_tuple(), index, true);
    }

    // -------------------- to_data_index ------------------

    /// The data_index is the tuple of the data_index of the components
    data_index_t to_data_index(index_t const &index) const {
      auto l = [](auto const &m, auto const &idx) { return m.to_data_index(idx); };
      return triqs::tuple::map_on_zip(l, *this, index);
    }

    // ------------------ to_index -------------------

    /// The index is the tuple of the index of the components
    index_t to_index(data_index_t const &data_index) const {
      auto l = [](auto const &m, auto const &didx) { return m.to_index(didx); };
      return triqs::tuple::map_on_zip(l, *this, data_index);
    }

    // -------------------- Accessors (other) -------------------

    m_tuple_t const &components() const { return *this; }
    m_tuple_t &components() { return *this; }

    m_tuple_t const &as_tuple() const { return *this; }
    m_tuple_t &as_tuple() { return *this; }

    // -------------------- mesh_point -------------------

    mesh_point_t operator[](data_index_t const &data_index) const {
      auto l = [](auto const &m, auto const &dindex) { return m[dindex]; };
      return triqs::tuple::map_on_zip(l, *this, data_index);
    }

    mesh_point_t operator()(index_t const &index) const {
      auto l = [](auto const &m, auto const &i) { return m(i); };
      return triqs::tuple::map_on_zip(l, *this, index);
    }

    // -------------------------- Range & Iteration --------------------------

    // The mesh is a range of mesh_points.
    private:
    [[nodiscard]] auto r_() const {
      auto f             = [](auto &&...x) { return itertools::product(x...); };
      auto to_mesh_point = [](auto &&n_mp) { return mesh_point_t{n_mp}; };
      return itertools::transform(std::apply(f, components()), to_mesh_point);
    }

    public:
    [[nodiscard]] auto begin() const { return r_().begin(); }
    [[nodiscard]] auto cbegin() const { return r_().cbegin(); }
    [[nodiscard]] auto end() const { return r_().end(); }
    [[nodiscard]] auto cend() const { return r_().cend(); }

    // -------------------- HDF5 -------------------

    [[nodiscard]] static std::string hdf5_format() { return "MeshProduct"; }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string subgroup_name, prod const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, m);
      auto l = [gr](int N, auto const &me) { h5_write(gr, "MeshComponent" + std::to_string(N), me); };
      triqs::tuple::for_each_enumerate(m.components(), l);
    }

    /// Read from HDF5
    friend void h5_read(h5::group fg, std::string subgroup_name, prod &m) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format(gr, m, true);
      auto l = [gr](int N, auto &me) { h5_read(gr, "MeshComponent" + std::to_string(N), me); };
      triqs::tuple::for_each_enumerate(m.components(), l);
    }

    // -------------------- print  -------------------

    friend std::ostream &operator<<(std::ostream &sout, prod const &prod_mesh) {
      sout << "Product Mesh";
      triqs::tuple::for_each(prod_mesh.as_tuple(), [&sout](auto &m) { sout << "\n  -- " << m; });
      return sout;
    }

    // ------------------------------------------------
    private:
    uint64_t hash_ = 0;
  }; //end of class

  // ---------- Class template argument deduction rules (CTAD) -------------
  template <typename M1, typename M2, typename... Ms> prod(M1, M2, Ms...) -> prod<M1, M2, Ms...>;

  template <typename M1, typename M2, typename... Ms>
  prod(std::tuple<M1, M2, Ms...>) -> prod<std::decay_t<M1>, std::decay_t<M2>, std::decay_t<Ms>...>;

} // namespace triqs::mesh

template <typename... Ms> struct std::tuple_size<triqs::mesh::prod<Ms...>> : public std::integral_constant<size_t, sizeof...(Ms)> {};
template <typename... Ms> struct std::tuple_size<triqs::mesh::prod_mesh_point<Ms...>> : public std::integral_constant<size_t, sizeof...(Ms)> {};

template <size_t N, typename... Ms> struct std::tuple_element<N, triqs::mesh::prod<Ms...>> : public std::tuple_element<N, std::tuple<Ms...>> {};
template <size_t N, typename... Ms>
struct std::tuple_element<N, triqs::mesh::prod_mesh_point<Ms...>> : public std::tuple_element<N, std::tuple<typename Ms::mesh_point_t...>> {};

// ----- product of mesh ---------------
namespace triqs::mesh {

  // Clean return type
  template <Mesh... M1, Mesh... M2> auto operator*(prod<M1...> const &m1, prod<M2...> const &m2) {
    return prod<M1..., M2...>{std::tuple_cat(m1.components(), m2.components())};
  }

  template <Mesh M1, Mesh... M2> auto operator*(M1 const &m1, prod<M2...> const &m2) {
    return prod<M1, M2...>{std::tuple_cat(std::make_tuple(m1), m2.components())};
  }

  template <Mesh... M1, Mesh M2> auto operator*(prod<M1...> const &m1, M2 const &m2) {
    return prod<M1..., M2>{std::tuple_cat(m1.components(), std::make_tuple(m2))};
  }

  template <Mesh M1, Mesh M2> auto operator*(M1 const &m1, M2 const &m2) { return prod<M1, M2>{m1, m2}; }

} // namespace triqs::mesh
