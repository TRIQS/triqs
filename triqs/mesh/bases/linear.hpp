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
#include "../details/mesh_tools.hpp"
namespace triqs::mesh {
 
  /**
   * Fit the two closest points for x on [x_min, x_max], with a linear weight w
   * @param x : the point
   * @param i_max : maximum index
   * @param x_min : the window starts. It ends at x_min + i_max* delta_x
   * @param delta_x
   *
   * Throws if x is not in the window
   * */
  inline interpol_data_lin_t<long, 2> interpolate_on_segment(double x, double x_min, double delta_x, long imax) {
    double a = (x - x_min) / delta_x;
    long i   = std::floor(a);
    bool in  = (i >= 0) && (i < imax);
    double w = a - i;
    // We include both x_min and x_max and account
    // for a small rounding error margin of 1e-8 for w
    if (i == imax) {
      --i;
      in = (std::abs(w) < 1.e-8);
      w  = 1.0;
    }
    if (i == -1) {
      i  = 0;
      in = (std::abs(1 - w) < 1.e-8);
      w  = 0.0;
    }
    if (!in) TRIQS_RUNTIME_ERROR << "out of window x= " << x << " xmin = " << x_min << " xmax = " << x_min + imax * delta_x;
    return {{i, i + 1}, {1 - w, w}};
  }
  //-----------------------------------------------------------------------

  template <typename Domain> struct linear_mesh : triqs::mesh::tag::mesh {

    using domain_t       = Domain;
    using index_t        = long;
    using linear_index_t = long;
    using domain_pt_t    = typename domain_t::point_t;

    static_assert(!std::is_base_of<std::complex<double>, domain_pt_t>::value, "Internal error : cannot use Linear Mesh in this case");

    // -------------------- Constructors -------------------

    explicit linear_mesh(domain_t dom, double a, double b, long n_pts) : _dom(std::move(dom)), L(n_pts), xmin(a), xmax(b), del((b - a) / (L - 1)) {}

    linear_mesh() : linear_mesh(domain_t{}, 0, 1, 2) {}

    /// Mesh comparison
    bool operator==(linear_mesh const &M) const {
      return ((_dom == M._dom) && (size() == M.size()) && (std::abs(xmin - M.xmin) < 1.e-15) && (std::abs(xmax - M.xmax) < 1.e-15));
    }
    bool operator!=(linear_mesh const &M) const { return !(operator==(M)); }

    // -------------------- Accessors (other) -------------------

    /// Step of the mesh
    double delta() const { return del; }

    /// Min of the mesh
    double x_min() const { return xmin; }

    /// Max of the mesh
    double x_max() const { return xmax; }

    // -------------------- Accessors (from concept) -------------------

    /// The corresponding domain
    domain_t const &domain() const { return _dom; }

    /// Size (linear) of the mesh of the window
    long size() const { return L; }

    /// Is the point in mesh ?
    static constexpr bool is_within_boundary(all_t) { return true; }
    bool is_within_boundary(double x) const { return ((x >= x_min()) && (x <= x_max())); }
    bool is_within_boundary(index_t idx) const { return ((idx >= 0) && (idx < L)); }

    /// From an index of a point in the mesh, returns the corresponding point in the domain
    domain_pt_t index_to_point(index_t idx) const {
      EXPECTS(is_within_boundary(idx));
      return xmin + idx * del;
    }

    /// Flatten the index in the positive linear index for memory storage (almost trivial here).
    long index_to_linear(index_t idx) const {
      EXPECTS(is_within_boundary(idx));
      return idx;
    }

    // -------------------- mesh_point -------------------

    /// Type of the mesh point
    using mesh_point_t = mesh_point<linear_mesh>;

    /// Accessing a point of the mesh
    mesh_point_t operator[](index_t i) const { return {*this, i}; }

    /// Iterating on all the points...
    using const_iterator = mesh_pt_generator<linear_mesh>;
    const_iterator begin() const { return const_iterator(this); }
    const_iterator end() const { return const_iterator(this, true); }
    const_iterator cbegin() const { return const_iterator(this); }
    const_iterator cend() const { return const_iterator(this, true); }

    // -------------- Evaluation of a function on the grid --------------------------

    interpol_data_lin_t<index_t, 2> get_interpolation_data(double x) const { return interpolate_on_segment(x, x_min(), delta(), long(size()) - 1); }

    // -------------- HDF5  --------------------------
    /// Write into HDF5
    friend void h5_write_impl(h5::group fg, std::string const &subgroup_name, linear_mesh const &m, const char *_type) {
      h5::group gr = fg.create_group(subgroup_name);
      gr.write_hdf5_scheme_as_string(_type);
      h5_write(gr, "domain", m.domain());
      h5_write(gr, "min", m.xmin);
      h5_write(gr, "max", m.xmax);
      h5_write(gr, "size", long(m.size()));
    }

    /// Read from HDF5
    friend void h5_read_impl(h5::group fg, std::string const &subgroup_name, linear_mesh &m, const char *tag_expected) {
      h5::group gr = fg.open_group(subgroup_name);
      gr.assert_hdf5_scheme_as_string(tag_expected, true);
      typename linear_mesh::domain_t dom;
      double a, b;
      long L;
      h5_read(gr, "domain", dom);
      h5_read(gr, "min", a);
      h5_read(gr, "max", b);
      h5_read(gr, "size", L);
      m = linear_mesh(std::move(dom), a, b, L);
    }

    // -------------------- boost serialization -------------------

    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, const unsigned int version) {
      ar &TRIQS_MAKE_NVP("domain", _dom);
      ar &TRIQS_MAKE_NVP("xmin", xmin);
      ar &TRIQS_MAKE_NVP("xmax", xmax);
      ar &TRIQS_MAKE_NVP("del", del);
      ar &TRIQS_MAKE_NVP("size", L);
    }

    // -------------------- print  -------------------

    friend std::ostream &operator<<(std::ostream &sout, linear_mesh const &m) { return sout << "Linear Mesh of size " << m.L; }

    // ------------------------------------------------
    private:
    domain_t _dom;
    long L;
    double xmin, xmax, del;
  };

  // ---------------------------------------------------------------------------
  //                     The mesh point
  // ---------------------------------------------------------------------------

  template <typename Domain>
  struct mesh_point<linear_mesh<Domain>> : public utility::arithmetic_ops_by_cast<mesh_point<linear_mesh<Domain>>, typename Domain::point_t> {
    using mesh_t  = linear_mesh<Domain>;
    using index_t = typename mesh_t::index_t;
    mesh_t const *m;
    index_t _index;

    public:
    mesh_point() : m(nullptr) {}
    mesh_point(mesh_t const &mesh, index_t const &index_) : m(&mesh), _index(index_) {}
    mesh_point(mesh_t const &mesh) : mesh_point(mesh, 0) {}
    void advance() { ++_index; }
    using cast_t = typename Domain::point_t;
    operator cast_t() const { return m->index_to_point(_index); }
    long linear_index() const { return _index; }
    long index() const { return _index; }
    bool at_end() const { return (_index == m->size()); }
    void reset() { _index = 0; }
    mesh_t const &mesh() const { return *m; }
  };

} // namespace triqs::mesh
