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
#include "./domains/product.hpp"
#include "./details/mesh_tools.hpp"
#include "./details/mesh_point.hpp"

#include "mesh_concepts.hpp"

namespace triqs::mesh {

  namespace detail {
    // a function that take the index for the mesh_point and let the rest pass through
    // used below in index_t constructor
    template <typename T> decltype(auto) get_index(T &&x) { return std::forward<T>(x); }
    template <typename M> auto get_index(typename M::mesh_point_t const &x) { return x.index(); }
    template <typename M> auto get_index(typename M::mesh_point_t &&x) { return x.index(); }
    template <typename M> auto get_index(typename M::mesh_point_t &x) { return x.index(); }
  } // namespace detail

  template <typename... Ms> class prod;
  // template <typename... Vs>  constexpr int get_n_variables(prod<Ms...>) { return sizeof...(Ms);}
  template <typename... Ms> struct get_n_variables<prod<Ms...>> { static const int value = sizeof...(Ms); };

  /** Cartesian product of mesh */
  // the mesh is simply a cartesian product
  template <typename... Ms> class prod : tag::product {

    // Backward compat helper.
    static_assert(not(std::is_reference_v<Ms> or ...), "Cannot create mesh product of references");
    static_assert(not(std::is_same_v<Ms, triqs::lattice::brillouin_zone> or ...),
                  "Since TRIQS 2.3, brillouin_zone is replaced by mesh::brzone as a mesh name. Cf Doc, changelog");

    public:
    using domain_t       = domain_product<typename Ms::domain_t...>;
    using m_tuple_t      = std::tuple<Ms...>;
    using m_pt_tuple_t   = std::tuple<typename Ms::mesh_point_t...>;
    using domain_pt_t    = typename domain_t::point_t;
    using linear_index_t = std::tuple<typename Ms::linear_index_t...>;

    static constexpr int dim = sizeof...(Ms);

    /// The index
    struct index_t {
      std::tuple<typename Ms::index_t...> _i;
      // construct with at least 2 arguments
      // The mesh point are replaced by their index.
      template <typename Arg0, typename Arg1, typename... Args>
      index_t(Arg0 &&arg0, Arg1 &&arg1, Args &&...args)
         : _i(detail::get_index(std::forward<Arg0>(arg0)), detail::get_index(std::forward<Arg1>(arg1)),
              detail::get_index(std::forward<Args>(args))...) {}
      // Construct the index_t from a tuple of indices
      index_t(std::tuple<typename Ms::index_t...> const &i) : _i(i) {}
      index_t(std::tuple<typename Ms::index_t...> &&i) : _i(i) {}

      index_t(index_t const &) = default;
      index_t(index_t &&)      = default;

      index_t &operator=(index_t const &idx) = default;
      index_t &operator=(index_t &&idx) = default;
    };

    struct mesh_point_t {
      using mesh_t = prod<Ms...>;
      mesh_t::m_pt_tuple_t components_;

      [[nodiscard]] auto components_tuple() const { return components_; }

      [[nodiscard]] auto index() const {
        return triqs::tuple::map([](auto const &mp) { return mp.index(); }, components_);
      }
      [[nodiscard]] auto value() const {
        return triqs::tuple::map([](auto const &mp) { return mp.value(); }, components_);
      }
      [[nodiscard]] auto linear_index() const {
        return triqs::tuple::map([](auto const &mp) { return mp.linear_index(); }, components_);
      }
      [[nodiscard]] auto mesh_hash() const {
        return triqs::tuple::map([](auto const &mp) { return mp.mesh_hash(); }, components_);
      }

      using cast_t = domain_pt_t;
      operator cast_t() const { return value; }

      template <int N> decltype(auto) get() { return std::get<N>(components_); }
      template <int N> decltype(auto) get() const { return std::get<N>(components_); }
    };

    // -------------------- Constructors -------------------

    prod() = default;
    prod(Ms const &...mesh) requires(sizeof...(Ms) > 0) : m_tuple(mesh...), _dom(mesh.domain()...), r_{make_mesh_prod_range<Ms...>(*this)} {}
    prod(std::tuple<Ms...> const &mesh_tpl)
       : m_tuple(mesh_tpl), _dom(triqs::tuple::map([](auto &&m) { return m.domain(); }, mesh_tpl)), r_{make_mesh_prod_range<Ms...>(*this)} {}
    template <typename... U>
    prod(std::tuple<U...> const &mesh_tpl)
       : m_tuple(mesh_tpl), _dom(triqs::tuple::map([](auto &&m) { return m.domain(); }, mesh_tpl)), r_{make_mesh_prod_range<Ms...>(*this)} {}
    prod(prod const &) = default;

    /// Mesh comparison
    bool operator==(prod const &m) const { return m_tuple == m.m_tuple; }
    bool operator!=(prod const &m) const { return !(operator==(m)); }

    // -------------------- Accessors (from concept) -------------------

    domain_t const &domain() const { return _dom; }

    /// size of the mesh is the product of size
    size_t size() const {
      return triqs::tuple::fold([](auto const &m, size_t R) { return R * m.size(); }, m_tuple, 1);
    }

    /// The sizes of all mesh components
    std::array<long, dim> size_of_components() const {
      std::array<long, dim> res;
      auto l = [&res](int i, auto const &m) mutable { res[i] = m.size(); };
      triqs::tuple::for_each_enumerate(m_tuple, l);
      return res;
    }

    /// Is the point of evaluation in the mesh. All components must be in the corresponding mesh.
    template <typename... Args> bool is_within_boundary(Args const &...args) const {
      return triqs::tuple::fold([](auto &m, auto &arg, bool r) { return r && (m.is_within_boundary(arg)); }, m_tuple, std::tie(args...), true);
    }
    template <typename... Args> bool is_within_boundary(index_t const &idx) const {
      return triqs::tuple::fold([](auto &m, auto &arg, bool r) { return r && (m.is_within_boundary(arg)); }, m_tuple, idx._i, true);
    }

    /// Conversions point <-> index <-> linear_index
    typename domain_t::point_t index_to_point(index_t const &idx) const {
      EXPECTS(is_within_boundary(idx));
      auto l = [](auto const &m, auto const &i) { return m.index_to_point(i); };
      return triqs::tuple::map_on_zip(l, m_tuple, idx._i);
    }

    /// The linear_index is the tuple of the linear_index of the components
    linear_index_t index_to_linear(index_t const &idx) const {
      EXPECTS(is_within_boundary(idx));
      auto l = [](auto const &m, auto const &i) { return m.index_to_linear(i); };
      return triqs::tuple::map_on_zip(l, m_tuple, idx._i);
    }

    // -------------------- Accessors (other) -------------------

    m_tuple_t const &components() const { return m_tuple; }
    m_tuple_t &components() { return m_tuple; }

    // for structured binding
    template <size_t pos> decltype(auto) get() { return std::get<pos>(m_tuple); }
    template <size_t pos> decltype(auto) get() const { return std::get<pos>(m_tuple); }

    ///
    linear_index_t mp_to_linear(m_pt_tuple_t const &mp) const {
      auto l = [](auto const &p) { return p.linear_index(); };
      return triqs::tuple::map(l, mp);
    }

    // Same but a variadic list of mesh_point_t
    template <typename... MP> size_t mesh_pt_components_to_linear(MP const &...mp) const {
      static_assert(std::is_same<std::tuple<MP...>, m_pt_tuple_t>::value, "Call incorrect ");
      return mp_to_linear(std::forward_as_tuple(mp...));
    }

    // -------------------- mesh_point -------------------

    /// Accessing a point of the mesh
    mesh_point_t operator[](index_t const &i) const { return mesh_point_t(*this, i); }
    mesh_point_t operator()(typename Ms::index_t... i) const { return (*this)[index_t{i...}]; }

    // -------------------------- Range & Iteration --------------------------

    auto begin() const { return r_.begin(); }
    auto end() const { return r_.end(); }
    auto cbegin() const { return r_.cbegin(); }
    auto cend() const { return r_.cend(); }

    // /// Iterating on all the points...
    // using const_iterator = mesh_pt_generator<prod>;
    // const_iterator begin() const { return const_iterator(this); }
    // const_iterator end() const { return const_iterator(this, true); }
    // const_iterator cbegin() const { return const_iterator(this); }
    // const_iterator cend() const { return const_iterator(this, true); }

    // -------------- Evaluation of a function on the grid --------------------------

    // not implemented, makes no sense
    long get_interpolation_data(long n) = delete;

    // -------------------- HDF5 -------------------

    static std::string hdf5_format() { return "MeshProduct"; }

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
      triqs::tuple::for_each(prod_mesh.m_tuple, [&sout](auto &m) { sout << "\n  -- " << m; });
      return sout;
    }

    // ------------------------------------------------
    private:
    m_tuple_t m_tuple;
    domain_t _dom;
    make_mesh_range_prod_rtype<Ms...> r_;
  }; //end of class

  // ---------- Class template argument deduction rules (CTAD) -------------
  template <typename M1, typename M2, typename... Ms> prod(M1, M2, Ms...) -> prod<M1, M2, Ms...>;

  template <typename M1, typename M2, typename... Ms>
  prod(std::tuple<M1, M2, Ms...>) -> prod<std::decay_t<M1>, std::decay_t<M2>, std::decay_t<Ms>...>;

} // namespace triqs::mesh

/// std::get (mesh) return the component...
namespace std {

  // mesh as tuple
  // redundant with .get<pos>, but seems necessary.
  template <size_t pos, typename... Ms> decltype(auto) get(triqs::mesh::prod<Ms...> const &m) { return std::get<pos>(m.components()); }

  template <typename... Ms> class tuple_size<triqs::mesh::prod<Ms...>> {
    public:
    static const int value = sizeof...(Ms);
  };

  template <size_t N, typename... Ms> class tuple_element<N, triqs::mesh::prod<Ms...>> : public tuple_element<N, std::tuple<Ms...>> {};

  /*
   * // NON PRODUCT mesh, for generic code std::get<0> should work
  // redundant with .get<pos>, but seems necessary.
  template <size_t pos, typename M> auto const &get(M const &m) requires(models_mesh_concept<M>) {
    static_assert(pos == 0, "std::get<N>() of a non cartesiant product mesh for N>0");
    return m;
  }

  template <typename M> class tuple_size<M> {
    public:
    static const int value = 1;
  };

  template <size_t N, typename M> class tuple_element<N, M> { using type = M; };
*/

  template <triqs::mesh::MeshPoint MP> class tuple_size<MP> : public std::integral_constant<size_t, tuple_size<typename MP::mesh_t>::value> {};

  // mesh_point as tuple
  template <int pos, typename... Ms> decltype(auto) get(typename triqs::mesh::prod<Ms...>::mesh_point_t const &m) {
    return std::get<pos>(m.components_tuple());
  }

  template <size_t N, triqs::mesh::MeshPoint MP> class tuple_element<N, MP> {
    public:
    using type = typename tuple_element<N, typename MP::mesh_t>::type::mesh_point_t;
  };

} // namespace std

// ----- product of mesh ---------------
namespace triqs::mesh {

  template <typename... M1, typename... M2> auto operator*(prod<M1...> const &m1, prod<M2...> const &m2) {
    return prod<M1..., M2...>{std::tuple_cat(m1.components(), m2.components())};
  }

  template <typename M1, typename... M2> auto operator*(M1 const &m1, prod<M2...> const &m2) {
    return prod<M1, M2...>{std::tuple_cat(std::make_tuple(m1), m2.components())};
  }

  template <typename... M1, typename M2> auto operator*(prod<M1...> const &m1, M2 const &m2) {
    return prod<M1..., M2>{std::tuple_cat(m1.components(), std::make_tuple(m2))};
  }

  template <typename M1, typename M2>
     auto operator*(M1 const &m1, M2 const &m2) //
     requires(models_mesh_concept_v<M1> or Mesh<M1>)
     and (models_mesh_concept_v<M2> or Mesh<M2>) {
    return prod<M1, M2>{m1, m2};
  }
} // namespace triqs::mesh
