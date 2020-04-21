/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
#include "../domains/discrete.hpp"

namespace triqs {
  namespace gfs {

    template <typename Domain> struct discrete {};

    template <typename Domain> struct gf_mesh<discrete<Domain>> {

      using domain_t       = Domain;
      using index_t        = long;
      using linear_index_t = long;
      using mesh_point_t   = mesh_point<gf_mesh<discrete<Domain>>>;
      using var_t          = discrete<Domain>;

      // -------------------- Constructors -------------------

      gf_mesh(domain_t dom) : _dom(std::move(dom)) {}
      gf_mesh() = default;

      bool operator==(gf_mesh const &M) const { return (_dom == M._dom); }
      bool operator!=(gf_mesh const &M) const { return !(operator==(M)); }

      // -------------------- Accessors (from concept) -------------------

      domain_t const &domain() const { return _dom; }
      size_t size() const { return _dom.size(); }

      ///
      utility::mini_vector<size_t, 1> size_of_components() const { return {size()}; }

      /// Is the point in the mesh ?
      static constexpr bool is_within_boundary(all_t) { return true; }
      bool is_within_boundary(index_t idx) const { return ((idx >= 0) && (idx < size())); }

      /// Conversions point <-> index <-> discrete_index
      long index_to_point(index_t idx) const {
        EXPECTS(is_within_boundary(idx));
        return idx;
      }
      long index_to_linear(index_t idx) const {
        EXPECTS(is_within_boundary(idx));
        return idx;
      }

      // -------------------- mesh_point -------------------

      /// Accessing a point of the mesh
      mesh_point_t operator[](index_t i) const { return mesh_point_t(*this, i); }
      mesh_point_t operator[](std::string const &s) const { return mesh_point_t(*this, _dom.index_from_name(s)); }

      /// Iterating on all the points...
      using const_iterator = mesh_pt_generator<gf_mesh>;
      const_iterator begin() const { return const_iterator(this); }
      const_iterator end() const { return const_iterator(this, true); }
      const_iterator cbegin() const { return const_iterator(this); }
      const_iterator cend() const { return const_iterator(this, true); }

      // -------------- Evaluation of a function on the grid --------------------------

      long get_interpolation_data(long n) const { return n; }
      template <typename F> auto evaluate(F const &f, long n) const { return f[n]; }

      // -------------------- MPI -------------------

      // -------------------- HDF5 -------------------

      // FIXME : CLEAN THIS MESS
      /// Write into HDF5
      friend void h5_write_impl(h5::group fg, std::string subgroup_name, gf_mesh const &m, const char *_type) {
        h5::group gr = fg.create_group(subgroup_name);
        gr.write_hdf5_scheme_as_string(_type);
        h5_write(gr, "domain", m.domain());
      }

      /// Read from HDF5
      friend void h5_read_impl(h5::group fg, std::string subgroup_name, gf_mesh &m, const char *tag_expected) {
        h5::group gr = fg.open_group(subgroup_name);
        gr.assert_hdf5_scheme_as_string(tag_expected, true);
        typename gf_mesh::domain_t dom;
        h5_read(gr, "domain", dom);
        m = gf_mesh(std::move(dom));
      }

      friend void h5_write(h5::group fg, std::string const &subgroup_name, gf_mesh const &m) { h5_write_impl(fg, subgroup_name, m, "MeshDiscrete"); }

      friend void h5_read(h5::group fg, std::string const &subgroup_name, gf_mesh &m) { h5_read_impl(fg, subgroup_name, m, "MeshDiscrete"); }

      // -------------------- boost serialization -------------------

      friend class boost::serialization::access;
      template <class Archive> void serialize(Archive &ar, const unsigned int version) { ar &_dom; }

      // -------------------- print  -------------------

      friend std::ostream &operator<<(std::ostream &sout, gf_mesh const &m) { return sout << "Discrete Mesh"; }

      // ------------------------------------------------
      private:
      domain_t _dom;
    };

    template <typename Domain>
    struct mesh_point<gf_mesh<discrete<Domain>>> : tag::mesh_point,
                                                   public utility::arithmetic_ops_by_cast<mesh_point<gf_mesh<discrete<Domain>>>, long> {
      using discrete_mesh_t = gf_mesh<discrete<Domain>>;
      using index_t         = typename discrete_mesh_t::index_t;
      discrete_mesh_t const *m;
      index_t _index;

      public:
      mesh_point()                   = default;
      mesh_point(mesh_point const &) = default;
      mesh_point(discrete_mesh_t const &mesh, index_t const &index_) : m(&mesh), _index(index_) {}
      mesh_point(discrete_mesh_t const &mesh) : mesh_point(mesh, 0) {}
      void advance() { ++_index; }
      using cast_t = long;
      operator cast_t() const { return m->index_to_point(_index); }
      long linear_index() const { return _index; }
      long index() const { return _index; }
      bool at_end() const { return (_index == m->size()); }
      void reset() { _index = 0; }
      discrete_mesh_t const &mesh() const { return *m; }
    };

  } // namespace gfs
} // namespace triqs
