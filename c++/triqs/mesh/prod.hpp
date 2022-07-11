/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2014 by O. Parcollet
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
#include "utils.hpp"

namespace triqs::mesh {

  template <Mesh... Ms> requires((n_variables<Ms> == 1) and ...) class prod;
  template <Mesh... Ms> static constexpr bool is_product<prod<Ms...>>   = true;
  template <Mesh... Ms> static constexpr int n_variables<prod<Ms...>> = sizeof...(Ms);

  template <Mesh... Ms>
  struct prod_mesh_point : public std::tuple<typename Ms::mesh_point_t...> {
    using mesh_t = prod<Ms...>;
    using idx_t    = typename mesh_t::idx_t;
    using datidx_t = typename mesh_t::datidx_t;
    using tuple_t  = std::tuple<typename Ms::mesh_point_t...>;

    idx_t idx          = std::apply([](auto &...x) { return std::make_tuple(x.idx...); }, *this);
    datidx_t datidx    = std::apply([](auto &...x) { return std::make_tuple(x.datidx...); }, *this);
    uint64_t mesh_hash = std::apply([](auto&...x){return (x.mesh_hash ^ ...);}, *this);

    tuple_t const &as_tuple() const { return *this; }
  };

  //
  template <typename P, typename C> auto make_mesh_range_prod(P const *m, C const &m_components, uint64_t mhash) {

    auto f             = [](auto &&...x) { return itertools::product(x...); };
    auto to_mesh_point = [](auto &&n_mp) { return typename P::mesh_point_t{n_mp}; };
    return itertools::transform(std::apply(f, m_components), to_mesh_point);

    //auto f = [](auto&&... x) { return itertools::enumerate(itertools::product(x...)); };
    //auto enumerated_mp_tuple_range = std::apply(f, m_components); // i.e. enumerate(product(m.components()...))
    //auto to_mesh_point = [h= mhash](auto && n_mp){ return typename P::mesh_point_t{ std::get<1>(n_mp), std::get<0>(n_mp), h};};
    //return enumerated_mp_tuple_range; //, to_mesh_point;
    //return itertools::product(std::get<0>(m_components),std::get<1>(m_components));
    //enumerated_mp_tuple_range.ee;
    //itertools::product(std::get<0>(m_components),std::get<1>(m_components)).eee;
    //return itertools::transform(itertools::product(std::get<0>(m_components),std::get<1>(m_components)), to_mesh_point);
  }

  /** Cartesian product of mesh */
  template <Mesh... Ms> requires((n_variables<Ms> == 1) and ...)
  class prod : public std::tuple<Ms...> {

    static constexpr int dim = sizeof...(Ms);
    // Backward compat helper.
    static_assert(not(std::is_reference_v<Ms> or ...), "Cannot create mesh product of references");
    static_assert(not(std::is_same_v<Ms, triqs::lattice::brillouin_zone> or ...),
		  "Since TRIQS 2.3, brillouin_zone is replaced by mesh::brzone as a mesh name. Cf Doc, changelog");

    public:
    using idx_t        = std::tuple<typename Ms::idx_t...>;
    using datidx_t     = std::tuple<typename Ms::datidx_t...>;
    using mesh_point_t = prod_mesh_point<Ms...>;
    using m_tuple_t    = std::tuple<Ms...>;

    // -------------------- Constructors -------------------

    prod() = default;

    prod(Ms const &...mesh) requires(sizeof...(Ms) > 0) : m_tuple_t{mesh...} {}

    template <typename... U> prod(std::tuple<U...> const &mesh_tuple) : m_tuple_t{mesh_tuple} {}

    /// Mesh comparison
    bool operator==(prod const &m) const { return as_tuple() == m.as_tuple(); }
    bool operator!=(prod const &m) const = default;

    // -------------------- -------------------

    /// size of the mesh is the product of size
    size_t size() const {
      return triqs::tuple::fold([](auto const &m, size_t R) { return R * m.size(); }, as_tuple(), 1);
    }

    /// The sizes of all mesh components
    std::array<long, dim> size_of_components() const {
      std::array<long, dim> res;
      auto l = [&res](int i, auto const &m) mutable { res[i] = m.size(); };
      triqs::tuple::for_each_enumerate( as_tuple(), l);
      return res;
    }

    /// Is the point of evaluation in the mesh. All components must be in the corresponding mesh.
    template <typename... Args> bool is_idx_valid(Args const &...args) const {
      return triqs::tuple::fold([](auto &m, auto &arg, bool r) { return r && (m.is_idx_valid(arg)); }, as_tuple(), std::tie(args...), true);
    }
    template <typename... Args> bool is_idx_valid(idx_t const &index) const {
      return triqs::tuple::fold([](auto &m, auto &arg, bool r) { return r && (m.is_idx_valid(arg)); }, as_tuple(), index, true);
    }

    /// The datidx is the tuple of the datidx of the components
    datidx_t to_datidx(idx_t const &index) const {
      EXPECTS(is_idx_valid(index));
      auto l = [](auto const &m, auto const &idx) { return m.to_datidx(idx); };
      return triqs::tuple::map_on_zip(l, *this, index);
    }

    /// The datidx is the tuple of the datidx of the components
    datidx_t to_datidx(mesh_point_t const &mesh_point) const {
      auto l = [](auto const &m, auto const &mp) { return mp.datidx; };
      return triqs::tuple::map_on_zip(l, *this, mesh_point);
    }

    /// The index is the tuple of the index of the components
    idx_t to_idx(datidx_t const &datidx) const {
      auto l = [](auto const &m, auto const &didx) { return m.to_idx(didx); };
      return triqs::tuple::map_on_zip(l, *this, datidx);
    }

    // -------------------- Accessors (other) -------------------

    m_tuple_t const &components() const { return *this; }
    m_tuple_t &components() { return *this; }

    m_tuple_t const &as_tuple() const { return *this; }
    m_tuple_t &as_tuple() { return *this; }

    ///
    //long mp_to_linear(m_pt_tuple_t const &mp) const {
    //auto l = [](auto const &p) { return p.datidx(); };
    //return triqs::tuple::map(l, mp);
    //}

    // Same but a variadic list of mesh_point_t
    //template <typename... MP> size_t mesh_pt_components_to_linear(MP const &...mp) const {
    //static_assert(std::is_same<std::tuple<MP...>, m_pt_tuple_t>::value, "Call incorrect ");
    //return mp_to_linear(std::forward_as_tuple(mp...));
    //}

    // -------------------- mesh_point -------------------

    mesh_point_t operator[](datidx_t const &datidx) const {
      auto l = [](auto const &m, auto const &didx) { return m[didx]; };
      return triqs::tuple::map_on_zip(l, *this, datidx);
    }

    mesh_point_t operator()(idx_t const &idx) const {
      auto l = [](auto const &m, auto const &i) { return m(i); };
      return triqs::tuple::map_on_zip(l, *this, idx);
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

    /*   public:*/

    /*[[nodiscard]] auto begin() const {*/
    /*r_ = make_range(*this);*/
    /*return r_.begin();*/
    /*}*/
    /*[[nodiscard]] auto cbegin() const {*/
    /*r_ = make_range(*this);*/
    /*return r_.cbegin();*/
    /*}*/
    /*[[nodiscard]] auto end() const { return r_.end(); }*/
    /*[[nodiscard]] auto cend() const { return r_.cend(); }*/

    // -------------------- HDF5 -------------------

    [[nodiscard]] static std::string hdf5_format() { return "MeshProduct"; }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string subgroup_name, prod const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, m);
      auto l = [gr](int N, auto const &m) { h5_write(gr, "MeshComponent" + std::to_string(N), m); };
      triqs::tuple::for_each_enumerate(m.components(), l);
    }

    /// Read from HDF5
    friend void h5_read(h5::group fg, std::string subgroup_name, prod &m) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format(gr, m, true);
      auto l = [gr](int N, auto &m) { h5_read(gr, "MeshComponent" + std::to_string(N), m); };
      triqs::tuple::for_each_enumerate(m.components(), l);
    }

    // -------------------- print  -------------------

    friend std::ostream &operator<<(std::ostream &sout, prod const &prod_mesh) {
      sout << "Product Mesh";
      triqs::tuple::for_each(prod_mesh. as_tuple(), [&sout](auto &m) { sout << "\n  -- " << m; });
      return sout;
    }

    // ------------------------------------------------
    private:
    uint64_t hash_;
  }; //end of class

  // ---------- Class template argument deduction rules (CTAD) -------------
  template <typename M1, typename M2, typename... Ms> prod(M1, M2, Ms...) -> prod<M1, M2, Ms...>;

  template <typename M1, typename M2, typename... Ms>
  prod(std::tuple<M1, M2, Ms...>) -> prod<std::decay_t<M1>, std::decay_t<M2>, std::decay_t<Ms>...>;

} // namespace triqs::mesh

template <typename... Ms> struct std::tuple_size<triqs::mesh::prod<Ms...>> : public std::integral_constant<size_t, sizeof...(Ms)> {};
template <typename... Ms> struct std::tuple_size<triqs::mesh::prod_mesh_point<Ms...>> : public std::integral_constant<size_t, sizeof...(Ms)> {};

template <size_t N, typename... Ms> struct std::tuple_element<N, triqs::mesh::prod<Ms...>> : public std::tuple_element<N, std::tuple<Ms...>> {};
template <size_t N, typename... Ms> struct std::tuple_element<N, triqs::mesh::prod_mesh_point<Ms...>> : public std::tuple_element<N, std::tuple<typename Ms::mesh_point_t...>> {};

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
