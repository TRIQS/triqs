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
#include "../mesh_concepts.hpp"
#include "../details/mesh_tools.hpp"
#include "../details/mesh_point.hpp"
#include <ranges>
#include <stdexcept>
namespace triqs::mesh {

  template <Domain D>
  requires std::totally_ordered<typename D::point_t> // Addable and dividable
  class linear_mesh {
    public:
    using domain_t    = D;
    using domain_pt_t = typename D::point_t; // Syntactic sugar

    // linear_index and index are identical for this mesh
    using index_t        = long;
    using linear_index_t = long;

    struct mesh_point_t : public mesh_point_impl<linear_mesh<D>> {
      template <typename... T> mesh_point_t(T &&...x) : mesh_point_impl<linear_mesh<D>>(std::forward<T>(x)...) {}
    };

    // -------------------- Index Bijection -------------------

    [[nodiscard]] linear_index_t index_to_linear(index_t const &index) const {
      EXPECTS(is_within_boundary(index));
      return index;
    }
    [[nodiscard]] index_t linear_to_index(linear_index_t const &liner_index) const { return liner_index; }

    // -------------------- Mesh Implemenation & Mesh Point Access -------------------

    /// From an index of a point in the mesh, returns the corresponding point in the domain
    [[nodiscard]] domain_pt_t index_to_point(index_t idx) const {
      EXPECTS(is_within_boundary(idx));
      if (L == 1) return xmin;
      double wr = double(idx) / (L - 1);
      double res = xmin * (1 - wr) + xmax * wr;
      ASSERT(is_within_boundary(res));
      return res;
    }

    [[nodiscard]] mesh_point_t operator[](index_t i) const { return {i, index_to_point(i), i, mesh_hash_}; }
    [[nodiscard]] mesh_point_t linear_to_mesh_pt(linear_index_t const &linear_index) const { return operator[](linear_to_index(linear_index)); }

    // -------------------- Constructors -------------------

    explicit linear_mesh(D dom, domain_pt_t a, domain_pt_t b, long n_pts)
       : _dom(std::move(dom)), L(n_pts), xmin(a), xmax(b), del(L == 1 ? 0. : (b - a) / (L - 1)), del_inv{del == 0.0 ? std::numeric_limits<double>::infinity() : 1. / del}, r_{make_mesh_range(*this)} {
      EXPECTS(a<=b);
    }

    linear_mesh() : linear_mesh(D{}, 0, 1, 2) {}

    // -------------------- Comparison -------------------

    /// Mesh comparison
    bool operator==(linear_mesh const &m) const {
      // Not default since we can't directly compare itertool range
      return (_dom == m._dom) && (size() == m.size()) && (xmin == m.xmin) && (xmax == m.xmax);
    }
    bool operator!=(linear_mesh const &) const = default;

    // -------------------- Accessors -------------------

    /// Domain on which mesh is defined
    auto const &domain() const { return _dom; }

    // Hash
    [[nodiscard]] size_t mesh_hash() const { return mesh_hash_; }

    /// Size (linear) of the mesh of the window
    [[nodiscard]] auto size() const { return L; }

    /// Step of the mesh
    [[nodiscard]] domain_pt_t delta() const { return del; }

    /// Inverse of the step of the mesh
    [[nodiscard]] domain_pt_t delta_inv() const { return del_inv; }

    /// Min of the mesh
    [[nodiscard]] domain_pt_t x_min() const { return xmin; }

    /// Max of the mesh
    [[nodiscard]] domain_pt_t x_max() const { return xmax; }

    // -------------------------- Other --------------------------

    /// Is the point in mesh ?
    static constexpr bool is_within_boundary(all_t) { return true; }                                  // ????? is index valid
    [[nodiscard]] bool is_within_boundary(domain_pt_t x) const { return (x >= xmin) && (x <= xmax); } // FIXME NAME: is within boundary
    [[nodiscard]] bool is_within_boundary(index_t idx) const { return (idx >= 0) && (idx < L); }      // FIXME NAME: is index valid

    // -------------------------- Range & Iteration --------------------------

    auto begin() const {
      r_ = make_mesh_range(*this);
      return r_.begin();
    }
    auto end() const { return r_.end(); }
    auto cbegin() const {
      r_ = make_mesh_range(*this);
      return r_.cbegin();
    }
    auto cend() const { return r_.cend(); }

    //  -------------------------- HDF5  --------------------------
    /// Write into HDF5
    friend void h5_write_impl(h5::group fg, std::string const &subgroup_name, linear_mesh const &m, const char *_type) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format_as_string(gr, _type);
      h5_write(gr, "domain", m.domain());
      h5_write(gr, "min", m.xmin);
      h5_write(gr, "max", m.xmax);
      h5_write(gr, "size", m.size());
    }

    /// Read from HDF5
    friend void h5_read_impl(h5::group fg, std::string const &subgroup_name, linear_mesh &m, const char *tag_expected) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format_as_string(gr, tag_expected, true);
      domain_t dom;
      domain_pt_t a, b;
      long L;
      h5_read(gr, "domain", dom);
      h5_read(gr, "min", a);
      h5_read(gr, "max", b);
      h5_read(gr, "size", L);
      m = linear_mesh(std::move(dom), a, b, L);
    }

    // -------------------- print  -------------------

    friend std::ostream &operator<<(std::ostream &sout, linear_mesh const &m) { return sout << "Linear Mesh of size " << m.L; }

    // ------------------------------------------------
    private:
    D _dom;
    long L;
    domain_pt_t xmin, xmax, del, del_inv;
    mutable make_mesh_range_rtype<linear_mesh> r_;
    size_t mesh_hash_ = 0;
  };

  //-----------------------------------------------------------------------

  /**
   * Fit the two closest points for x on [x_min, x_max], with a linear weight w
   * @param x : the point
   * @param i_max : maximum index
   * @param x_min : the window starts. It ends at x_min + i_max* delta_x
   * @param delta_x
   *
   * 
   * Throws if x is not in the window
   * */
  inline std::array<std::pair<long, double>, 2> interpolate_on_segment(double x, double x_min, double delta_x, double delta_x_inv, long imax) noexcept {
    EXPECTS(x_min <= x and x <= x_min + imax * delta_x);
    x = std::max(x, x_min);
    double a = (x - x_min) * delta_x_inv;
    long i   = std::min(static_cast<long>(a), imax - 1);
    double w = std::min(a - i, 1.0);
    return {std::make_pair(i, 1 - w), std::make_pair(i + 1, w)};
  }

  template <typename Domain> inline std::array<std::pair<long, double>, 2> get_interpolation_data(linear_mesh<Domain> const &m, double x) {
    return interpolate_on_segment(x, m.x_min(), m.delta(), m.delta_inv(), long(m.size()) - 1);
  }

  //-----------------------------------------------------------------------

} // namespace triqs::mesh
