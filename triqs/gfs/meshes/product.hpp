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
#include "./mesh_tools.hpp"
#include "../domains/product.hpp"
#include <triqs/utility/tuple_tools.hpp>
#include <triqs/utility/mini_vector.hpp>
#include "./multivar_eval.hpp"

namespace triqs {
  namespace gfs {

    namespace detail {
      // a function that take the index for the mesh_point and let the rest pass through
      // used below in index_t constructor
      template <typename T> decltype(auto) get_index(T &&x) { return std::forward<T>(x); }
      template <typename T> auto get_index(mesh_point<T> const &x) { return x.index(); }
      template <typename T> auto get_index(mesh_point<T> &&x) { return x.index(); }
      template <typename T> auto get_index(mesh_point<T> &x) { return x.index(); }
    } // namespace detail

    template <typename... Vs> struct cartesian_product { using type = std::tuple<Vs...>; };

    // template <typename... Vs>  constexpr int get_n_variables(cartesian_product<Vs...>) { return sizeof...(Vs);}
    template <typename... Vs> struct get_n_variables<cartesian_product<Vs...>> { static const int value = sizeof...(Vs); };

    /** Cartesian product of meshes */
    // the mesh is simply a cartesian product
    template <typename... Vs> struct gf_mesh<cartesian_product<Vs...>> : tag::composite {
      using domain_t           = domain_product<typename gf_mesh<Vs>::domain_t...>;
      using m_tuple_t          = std::tuple<gf_mesh<Vs>...>;
      using ms_tuple_t         = std::tuple<Vs...>;
      using m_pt_tuple_t       = std::tuple<typename gf_mesh<Vs>::mesh_point_t...>;
      using domain_pt_t        = typename domain_t::point_t;
      using linear_index_t     = std::tuple<typename gf_mesh<Vs>::linear_index_t...>;
      using var_t              = cartesian_product<Vs...>;
      static constexpr int dim = sizeof...(Vs);

      /// The index
      struct index_t {
        std::tuple<typename gf_mesh<Vs>::index_t...> _i;
        // construct with at least 2 arguments
        // The mesh point are replaced by their index.
        template <typename Arg0, typename Arg1, typename... Args>
        index_t(Arg0 &&arg0, Arg1 &&arg1, Args &&... args)
           : _i(detail::get_index(std::forward<Arg0>(arg0)), detail::get_index(std::forward<Arg1>(arg1)),
                detail::get_index(std::forward<Args>(args))...) {}
        // Construct the index_t from a tuple of indices
        index_t(std::tuple<typename gf_mesh<Vs>::index_t...> const &i) : _i(i) {}
        index_t(std::tuple<typename gf_mesh<Vs>::index_t...> &&i) : _i(i) {}

        index_t(index_t const &) = default;
        index_t(index_t &&)      = default;

        index_t &operator=(index_t const &idx) = default;
        index_t &operator=(index_t &&idx) = default;
      };

      // -------------------- Constructors -------------------

      gf_mesh() = default;
      gf_mesh(gf_mesh<Vs> const &... meshes) : m_tuple(meshes...), _dom(meshes.domain()...) {}
      gf_mesh(std::tuple<gf_mesh<Vs>...> const &mesh_tpl)
         : m_tuple(mesh_tpl), _dom(triqs::tuple::map([](auto &&m) { return m.domain(); }, mesh_tpl)) {}
      gf_mesh(gf_mesh const &) = default;

      /// Mesh comparison
      bool operator==(gf_mesh const &m) const { return m_tuple == m.m_tuple; }
      bool operator!=(gf_mesh const &m) const { return !(operator==(m)); }

      // -------------------- Accessors (from concept) -------------------

      domain_t const &domain() const { return _dom; }

      /// size of the mesh is the product of size
      size_t size() const {
        return triqs::tuple::fold([](auto const &m, size_t R) { return R * m.size(); }, m_tuple, 1);
      }

      /// The sizes of all mesh components
      utility::mini_vector<size_t, dim> size_of_components() const {
        utility::mini_vector<size_t, dim> res;
        auto l = [&res](int i, auto const &m) mutable { res[i] = m.size(); };
        triqs::tuple::for_each_enumerate(m_tuple, l);
        return res;
      }

      /// Conversions point <-> index <-> linear_index
      typename domain_t::point_t index_to_point(index_t const &ind) const {
        domain_pt_t res;
        auto l = [](auto &p, auto const &m, auto const &i) { p = m.index_to_point(i); };
        triqs::tuple::map_on_zip(l, res, m_tuple, ind);
        return res;
      }

      /// The linear_index is the tuple of the linear_index of the components
      linear_index_t index_to_linear(index_t const &ind) const {
        auto l = [](auto const &m, auto const &i) { return m.index_to_linear(i); };
        return triqs::tuple::map_on_zip(l, m_tuple, ind._i);
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
      template <typename... MP> size_t mesh_pt_components_to_linear(MP const &... mp) const {
        static_assert(std::is_same<std::tuple<MP...>, m_pt_tuple_t>::value, "Call incorrect ");
        return mp_to_linear(std::forward_as_tuple(mp...));
      }

      // -------------------- mesh_point -------------------

      using mesh_point_t = mesh_point<gf_mesh>;

      /// Accessing a point of the mesh
      mesh_point_t operator[](index_t const &i) const { return mesh_point_t(*this, i); }
      mesh_point_t operator()(typename gf_mesh<Vs>::index_t... i) const { return (*this)[index_t{i...}]; }

      /// Iterating on all the points...
      using const_iterator = mesh_pt_generator<gf_mesh>;
      const_iterator begin() const { return const_iterator(this); }
      const_iterator end() const { return const_iterator(this, true); }
      const_iterator cbegin() const { return const_iterator(this); }
      const_iterator cend() const { return const_iterator(this, true); }

      // -------------- Evaluation of a function on the grid --------------------------

      /// Is the point of evaluation in the mesh. All components must be in the corresponding mesh.
      template <typename... Args> bool is_within_boundary(Args const &... args) const {
        return triqs::tuple::fold([](auto &m, auto &arg, bool r) { return r && (m.is_within_boundary(arg)); }, m_tuple, std::tie(args...), true);
      }

      // not implemented, makes no sense
      long get_interpolation_data(long n) = delete;

      private:
      template <typename F, size_t... Is, typename... Args> auto _impl_evaluate(std::index_sequence<Is...>, F const &f, Args &&... args) const {
        return multivar_eval(f, std::get<Is>(f.mesh().components()).get_interpolation_data(std::forward<Args>(args))...);
      }

      public:
      template <typename F, typename... Args> auto evaluate(F const &f, Args &&... args) const {
        return _impl_evaluate(std::index_sequence_for<Args...>{}, f, std::forward<Args>(args)...);
      }

      // -------------------- HDF5 -------------------

      static std::string hdf5_scheme() { return "MeshProduct"; }

      /// Write into HDF5
      friend void h5_write(h5::group fg, std::string subgroup_name, gf_mesh const &m) {
        h5::group gr = fg.create_group(subgroup_name);
        gr.write_hdf5_scheme(m);
        auto l = [gr](int N, auto const &m) { h5_write(gr, "MeshComponent" + std::to_string(N), m); };
        triqs::tuple::for_each_enumerate(m.components(), l);
      }

      /// Read from HDF5
      friend void h5_read(h5::group fg, std::string subgroup_name, gf_mesh &m) {
        h5::group gr = fg.open_group(subgroup_name);
        gr.assert_hdf5_scheme(m, true);
        auto l = [gr](int N, auto &m) { h5_read(gr, "MeshComponent" + std::to_string(N), m); };
        triqs::tuple::for_each_enumerate(m.components(), l);
      }

      // -------------------- boost serialization -------------------

      friend class boost::serialization::access;
      template <class Archive> void serialize(Archive &ar, const unsigned int version) {
        auto l = [&ar](int N, auto &m) { ar &TRIQS_MAKE_NVP("MeshComponent" + std::to_string(N), m); };
        triqs::tuple::for_each_enumerate(m_tuple, l);
      }

      // -------------------- print  -------------------

      friend std::ostream &operator<<(std::ostream &sout, gf_mesh const &prod_mesh) {
        sout << "Product Mesh";
        triqs::tuple::for_each(prod_mesh.m_tuple, [&sout](auto &m) { sout << "\n  -- " << m; });
        return sout;
      }

      // ------------------------------------------------
      private:
      m_tuple_t m_tuple;
      domain_t _dom;
    }; //end of class

    // ---------- Class template argument deduction rules -------------
    template <typename M1, typename M2, typename... Ms>
    gf_mesh(M1, M2, Ms...)->gf_mesh<cartesian_product<typename M1::var_t, typename M2::var_t, typename Ms::var_t...>>;

    template <typename M1, typename M2, typename... Ms>
    gf_mesh(std::tuple<M1, M2, Ms...>)->gf_mesh<cartesian_product<typename M1::var_t, typename M2::var_t, typename Ms::var_t...>>;

    // ------------------------------------------------
    /// The wrapper for the mesh point

    template <typename... Vs> struct mesh_point<gf_mesh<cartesian_product<Vs...>>> : tag::mesh_point {
      using mesh_t         = gf_mesh<cartesian_product<Vs...>>;
      using index_t        = typename mesh_t::index_t;
      using m_pt_tuple_t   = typename mesh_t::m_pt_tuple_t;
      using linear_index_t = typename mesh_t::linear_index_t;
      using domain_pt_t    = typename mesh_t::domain_pt_t;

      const mesh_t *m;
      m_pt_tuple_t _c;
      bool _atend;
      struct F1 {
        template <typename M> typename M::mesh_point_t operator()(M const &m) const { return {m}; }
      };

      public:
      mesh_point() = default;
      mesh_point(mesh_t const &m_, index_t index_)
         : m(&m_), _c(triqs::tuple::map_on_zip([](auto const &m, auto const &i) { return m[i]; }, m_.components(), index_._i)), _atend(false) {}
      mesh_point(mesh_t const &m_) : m(&m_), _c(triqs::tuple::map(F1(), m_.components())), _atend(false) {}
      m_pt_tuple_t const &components_tuple() const { return _c; }
      linear_index_t linear_index() const { return m->mp_to_linear(_c); }
      index_t index() const {
        return triqs::tuple::map([](auto const &mesh_pt) { return mesh_pt.index(); }, _c);
      }
      const mesh_t &mesh() const { return *m; }

      using cast_t = domain_pt_t;
      operator cast_t() const { return _c; }

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

      // std::get should work FIXME ? redondant
      template <int N> decltype(auto) get() { return std::get<N>(_c); }
      template <int N> decltype(auto) get() const { return std::get<N>(_c); }

    }; // end mesh_point

  } // namespace gfs
} // namespace triqs

/// std::get (mesh) return the component...
namespace std {

  // mesh as tuple
  // redondant with .get<pos>, but seems necessary.
  template <size_t pos, typename... Vs> decltype(auto) get(triqs::gfs::gf_mesh<triqs::gfs::cartesian_product<Vs...>> const &m) {
    return std::get<pos>(m.components());
  }

  template <typename... Vs> class tuple_size<triqs::gfs::gf_mesh<triqs::gfs::cartesian_product<Vs...>>> {
    public:
    static const int value = sizeof...(Vs);
  };

  template <size_t N, typename... Vs>
  class tuple_element<N, triqs::gfs::gf_mesh<triqs::gfs::cartesian_product<Vs...>>>
     : public tuple_element<N, std::tuple<typename triqs::gfs::gf_mesh<Vs>...>> {};

  // NON PRODUCT mesh, for generic code std::get<0> should work
  // redondant with .get<pos>, but seems necessary.
  template <size_t pos, typename M> auto const &get(triqs::gfs::gf_mesh<M> const &m) {
    static_assert(pos == 0, "std::get<N>() of a non cartesiant product mesh for N>0");
    return m;
  }

  template <typename M> class tuple_size<triqs::gfs::gf_mesh<M>> {
    public:
    static const int value = 1;
  };

  template <size_t N, typename M> class tuple_element<N, triqs::gfs::gf_mesh<M>> { using type = triqs::gfs::gf_mesh<M>; };

  // mesh_point as tuple
  template <int pos, typename... Vs> decltype(auto) get(triqs::gfs::mesh_point<triqs::gfs::gf_mesh<triqs::gfs::cartesian_product<Vs...>>> const &m) {
    return std::get<pos>(m.components_tuple());
  }

  template <typename... Vs> class tuple_size<triqs::gfs::mesh_point<triqs::gfs::gf_mesh<triqs::gfs::cartesian_product<Vs...>>>> {
    public:
    static const int value = sizeof...(Vs);
  };

  template <size_t N, typename... Vs>
  class tuple_element<N, triqs::gfs::mesh_point<triqs::gfs::gf_mesh<triqs::gfs::cartesian_product<Vs...>>>>
     : public tuple_element<N, std::tuple<typename triqs::gfs::gf_mesh<Vs>::mesh_point_t...>> {};

} // namespace std

// ----- product of mesh ---------------
namespace triqs::gfs {
  template <typename A, typename B, size_t... Is, size_t... Js>
  auto _prod_impl(std::index_sequence<Is...>, std::index_sequence<Js...>, gf_mesh<A> const &a, gf_mesh<B> const &b) {
    return gf_mesh{std::get<Is>(a)..., std::get<Js>(b)...};
  }
  template <typename A, typename B> auto operator*(gf_mesh<A> const &a, gf_mesh<B> const &b) {
    return _prod_impl(std::make_index_sequence<get_n_variables<A>::value>{}, std::make_index_sequence<get_n_variables<B>::value>{}, a, b);
  }
} // namespace triqs::gfs
