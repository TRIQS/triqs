/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2013 by O. Parcollet
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
#include "./mesh_tools.hpp"
#include "../domains/product.hpp"
#include <triqs/utility/tuple_tools.hpp>
#include <triqs/utility/mini_vector.hpp>
#include <triqs/utility/c14.hpp>
namespace triqs {
namespace gfs {

 /** Cartesian product of meshes */
 template <typename... Meshes> struct mesh_product : tag::composite {
  using domain_t = domain_product<typename Meshes::domain_t...>;
  using index_t = std::c14::tuple<typename Meshes::index_t...>;
  using m_tuple_t = std::tuple<Meshes...>;
  using m_pt_tuple_t = std::tuple<typename Meshes::mesh_point_t...>;
  using domain_pt_t = typename domain_t::point_t;

  static constexpr int dim = sizeof...(Meshes);

  mesh_product() {}
  mesh_product(Meshes const &... meshes) : m_tuple(meshes...), _dom(meshes.domain()...) {}

  domain_t const &domain() const { return _dom; }
  m_tuple_t const &components() const { return m_tuple; }
  m_tuple_t &components() { return m_tuple; }

  /// size of the mesh is the product of size
  size_t size() const {
   return triqs::tuple::fold([](auto const &m, size_t R) { return R * m.size(); }, m_tuple, 1);
  }

  /// Conversions point <-> index <-> linear_index
  typename domain_t::point_t index_to_point(index_t const &ind) const {
   domain_pt_t res;
   auto l = [](auto &p, auto const &m, auto const &i) { p = m.index_to_point(i); };
   triqs::tuple::map_on_zip(l, res, m_tuple, ind);
   return res;
  }

  /// Flattening index to linear :  index[0] + component[0].size * (index[1] + component[1].size* (index[2] + ....))
  size_t index_to_linear(index_t const &ii) const {
   auto l = [](auto const &m, auto const &i, size_t R) { return m.index_to_linear(i) + R * m.size(); };
   return triqs::tuple::fold(l, reverse(m_tuple), reverse(ii), size_t(0));
  }
  
  /// Flattening index to linear :  index[0] + component[0].size * (index[1] + component[1].size* (index[2] + ....))
  size_t mp_to_linear(m_pt_tuple_t const &mp) const {
   auto l = [](auto const &m, auto const &p, size_t R) { return p.linear_index() + R * m.size(); };
   return triqs::tuple::fold(l, reverse(m_tuple), reverse(mp), size_t(0));
  }

  utility::mini_vector<size_t, dim> shape() const {
   utility::mini_vector<size_t, dim> res;
   auto l = [&res](int i, auto const &m) mutable { res[i] = m.size(); };
   triqs::tuple::for_each_enumerate(m_tuple, l);
   return res;
  }

  // Same but a variadic list of mesh_point_t
  template <typename... MP> size_t mesh_pt_components_to_linear(MP const &... mp) const {
   static_assert(std::is_same<std::tuple<MP...>, m_pt_tuple_t>::value, "Call incorrect ");
   // static_assert(std::is_same< std::tuple<typename std::remove_cv<typename std::remove_reference<MP>::type>::type...>,
   // m_pt_tuple_t>::value, "Call incorrect ");
   return mp_to_linear(std::forward_as_tuple(mp...));
  } // speed test ? or make a variadic fold...

  /// The wrapper for the mesh point
  class mesh_point_t : tag::mesh_point {
   const mesh_product *m;
   m_pt_tuple_t _c;
   bool _atend;
   struct F1 {
    template <typename M> typename M::mesh_point_t operator()(M const &m) const { return {m}; }
   };

   public:
   mesh_point_t() = default;
   mesh_point_t(mesh_product const &m_, index_t index_)
      : m(&m_)
      , _c(triqs::tuple::map_on_zip([](auto const & m, auto const & i) { return m[i]; }, m_.m_tuple, index_))
      , _atend(false) {}
   mesh_point_t(mesh_product const &m_) : m(&m_), _c(triqs::tuple::map(F1(), m_.m_tuple)), _atend(false) {}
   m_pt_tuple_t const &components_tuple() const { return _c; }
   size_t linear_index() const { return m->mp_to_linear(_c); }
   const mesh_product *mesh() const { return m; }

   using cast_t = domain_pt_t;
   operator cast_t() const { return m->index_to_point(index); }

   // index[0] +=1; if index[0]==m.component[0].size() { index[0]=0; index[1] +=1; if  ....}  and so on until dim
   void advance() {
    auto l = [](auto &p, bool done) {
     if (done) return true;
     p.advance();
     if (!p.at_end()) return true;
     p.reset();
     return false;
    };
    _atend = !(triqs::tuple::fold(l, _c, false));
   }

   // index_t index() const { return _index;} // not implemented yet
   bool at_end() const { return _atend; }

   void reset() {
    _atend = false;
    triqs::tuple::for_each(_c, [](auto &m) { m.reset(); });
   }
  }; // end mesh_point_t

  /// Accessing a point of the mesh
  mesh_point_t operator[](index_t i) const { return mesh_point_t(*this, i); }
  mesh_point_t operator()(typename Meshes::index_t... i) const { return (*this)[std::make_tuple(i...)]; }

  /// Iterating on all the points...
  using const_iterator = mesh_pt_generator<mesh_product>;
  const_iterator begin() const { return const_iterator(this); }
  const_iterator end() const { return const_iterator(this, true); }
  const_iterator cbegin() const { return const_iterator(this); }
  const_iterator cend() const { return const_iterator(this, true); }

  /// Mesh comparison
  friend bool operator==(mesh_product const &M1, mesh_product const &M2) { return M1.m_tuple == M2.m_tuple; }

  /// Write into HDF5
  friend void h5_write(h5::group fg, std::string subgroup_name, mesh_product const &m) {
   h5::group gr = fg.create_group(subgroup_name);
   auto l = [gr](int N, auto const &m) { h5_write(gr, "MeshComponent" + std::to_string(N), m); };
   triqs::tuple::for_each_enumerate(m.components(), l);
  }

  /// Read from HDF5
  friend void h5_read(h5::group fg, std::string subgroup_name, mesh_product &m) {
   h5::group gr = fg.open_group(subgroup_name);
   auto l = [gr](int N, auto &m) { h5_read(gr, "MeshComponent" + std::to_string(N), m); };
   triqs::tuple::for_each_enumerate(m.components(), l);
  }

  /// BOOST Serialization
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) {
   auto l = [&ar](int N, auto &m) { ar &TRIQS_MAKE_NVP("MeshComponent" + std::to_string(N), m); };
   triqs::tuple::for_each_enumerate(m_tuple, l);
  }

  friend std::ostream &operator<<(std::ostream &sout, mesh_product const &m) { return sout << "Product Mesh"; }

  private:
  m_tuple_t m_tuple;
  domain_t _dom;
 };

 template <int pos, typename P> decltype(auto) get_index(P const &p) {return std::get<pos>(p.components_tuple()).index();}

 template <int pos, typename P> decltype(auto) get_point(P const &p) {
  return std::get<pos>(p.mesh()->components()).index_to_point(std::get<pos>(p.components_tuple()).index());
 }

 template <int pos, typename P> decltype(auto) get_component(P const &p) { return std::get<pos>(p.components_tuple()); }

 // Given a composite mesh m , and a linear array of storage A
 // reinterpret_linear_array(m,A) returns a d-dimensionnal view of the array
 // with indices egal to the indices of the components of the mesh.
 // Very useful for slicing, currying functions.
 template <typename... Meshes, typename T, ull_t OptionsFlags, ull_t To, int R, bool B, bool C>
 arrays::array_view<T, sizeof...(Meshes) + R - 1, OptionsFlags, To, true, C>
 reinterpret_linear_array(mesh_product<Meshes...> const &m, arrays::array_view<T, R, OptionsFlags, To, B, C> A) {
  return {{join(m.shape(), get_shape(A).front_pop())}, A.storage()};
 }
}
}
