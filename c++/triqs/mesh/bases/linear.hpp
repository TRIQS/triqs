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
#include <ranges>
#include <stdexcept>
#include "../utils.hpp"

namespace triqs::mesh {

  template <typename Mesh, typename Value> class linear {
    static_assert(std::totally_ordered<Value>);

    public:
    using value_t  = Value;
    using idx_t    = long;
    using datidx_t = long;

    // --  data
    protected:
    long L;
    value_t xmin, xmax, del;
    double del_inv;
    size_t mesh_hash_ = 0;

    // -------------------- Constructors -------------------
    public:
    linear(value_t a = 0, value_t b = 1, long n_pts = 2)
       : L(n_pts),
         xmin(a),
         xmax(b),
         del(L == 1 ? 0. : (b - a) / (L - 1)),
         del_inv{del == 0.0 ? std::numeric_limits<double>::infinity() : 1. / del},
         mesh_hash_(hash(L, xmin, xmax)) {
      EXPECTS(a <= b);
    }

    // -------------------- Comparison -------------------

    /// Mesh comparison
    bool operator==(linear const &) const = default;
    bool operator!=(linear const &) const = default;

    // --------------------  Mesh Point -------------------

    struct mesh_point_t {
      using mesh_t       = Mesh;
      long idx           = 0;
      long datidx        = 0;
      uint64_t mesh_hash = 0;
      value_t val        = {};
      value_t value() const { return val; }
      operator value_t() const { return val; }

      // OPFIXME : Commented the first requires (as in the godbolt)
      // otherwise, the mp + mp is not covered.
      // https://godbolt.org/z/xoYP3vTW4

      // OPFIXME : this is the requires of the first
      //requires(not std::is_same_v<decltype(y), mesh_point_t const &>)                                                                                  \

#define IMPL_OP(OP)                                                                                                                                  \
  friend auto operator OP(mesh_point_t const &mp, auto const &y) { return mp.val OP y; }                                                             \
  friend auto operator OP(auto const &x, mesh_point_t const &mp)                                                                                     \
    requires(not std::is_same_v<decltype(x), mesh_point_t const &>)                                                                                  \
  {                                                                                                                                                  \
    return x OP mp.val;                                                                                                                              \
  }
      IMPL_OP(+);
      IMPL_OP(-);
      IMPL_OP(*);
      IMPL_OP(/);
#undef IMPL_OP
    };

    // -------------------- idx checks and conversions -------------------

    [[nodiscard]] bool is_idx_valid(idx_t idx) const noexcept { return 0 <= idx and idx < L; }
    [[nodiscard]] bool is_value_valid(value_t val) const noexcept { return xmin <= val and val <= xmax; }

    // -------------------- to_datidx ------------------

    [[nodiscard]] datidx_t to_datidx(idx_t idx) const noexcept {
      EXPECTS(is_idx_valid(idx));
      return idx;
    }

    [[nodiscard]] datidx_t to_datidx(mesh_point_t const &mp) const noexcept {
      EXPECTS(mesh_hash_ == mp.mesh_hash);
      return mp.datidx;
    }

    [[nodiscard]] idx_t to_datidx(closest_mesh_point_t<value_t> const &cmp) const noexcept {
      EXPECTS(is_value_valid(cmp.value));
      return to_datidx(to_idx(cmp));
    }

    // ------------------ to_idx -------------------

    [[nodiscard]] idx_t to_idx(datidx_t datidx) const noexcept {
      EXPECTS(is_idx_valid(datidx));
      return datidx;
    }

    [[nodiscard]] idx_t to_idx(closest_mesh_point_t<value_t> const &cmp) const noexcept {
      EXPECTS(is_value_valid(cmp.value));
      return idx_t((cmp.value - xmin) * del_inv + 0.5);
    }

    // ------------------ operator[] -------------------

    [[nodiscard]] mesh_point_t operator[](long datidx) const { return (*this)(datidx); }

    [[nodiscard]] mesh_point_t operator[](closest_mesh_point_t<value_t> const &cmp) const { return (*this)[this->to_datidx(cmp)]; }

    // ------------------ operator()-------------------

    [[nodiscard]] mesh_point_t operator()(idx_t idx) const {
      EXPECTS(is_idx_valid(idx));
      return {idx, idx, mesh_hash_, to_value(idx)};
    }

    [[nodiscard]] value_t to_value(idx_t idx) const noexcept {
      EXPECTS(is_idx_valid(idx));
      if (L == 1) return xmin;
      double wr  = double(idx) / (L - 1);
      double res = xmin * (1 - wr) + xmax * wr;
      return res;
    }

    // -------------------- Accessors -------------------

    // Hash
    [[nodiscard]] uint64_t mesh_hash() const noexcept { return mesh_hash_; }

    /// Size (linear) of the mesh of the window
    [[nodiscard]] auto size() const noexcept { return L; }

    /// Step of the mesh
    [[nodiscard]] value_t delta() const noexcept { return del; }

    /// Inverse of the step of the mesh
    [[nodiscard]] value_t delta_inv() const noexcept { return del_inv; }

    // -------------------------- Range & Iteration --------------------------

    private:
    [[nodiscard]] auto r_() const {
      return itertools::transform(range(size()), [this](long i) { return (*this)[i]; });
    }

    public:
    [[nodiscard]] auto begin() const { return r_().begin(); }
    [[nodiscard]] auto cbegin() const { return r_().cbegin(); }
    [[nodiscard]] auto end() const { return r_().end(); }
    [[nodiscard]] auto cend() const { return r_().cend(); }

    // -------------------- print  -------------------

    friend std::ostream &operator<<(std::ostream &sout, linear const &m) { return sout << "linear mesh of size " << m.l; }

    //  -------------------------- HDF5  --------------------------

    /// Write into HDF5
    friend void h5_write_impl(h5::group fg, std::string const &subgroup_name, linear const &m, const char *format) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format_as_string(gr, format);

      h5::write(gr, "min", m.xmin);
      h5::write(gr, "max", m.xmax);
      h5::write(gr, "size", m.size());
    }

    /// Read from HDF5
    friend void h5_read_impl(h5::group fg, std::string const &subgroup_name, linear &m, const char *format_expected) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format_as_string(gr, format_expected, true);

      auto a = h5::read<value_t>(gr, "min");
      auto b = h5::read<value_t>(gr, "max");
      auto L = h5::read<long>(gr, "size");
      m      = linear(a, b, L);
    }

    // ------------------------- Evaluate -----------------------------

    friend decltype(auto) evaluate(linear const &m, auto const &f, double x) {
      EXPECTS(m.is_value_valid(x) and m.size() > 1);
      x        = std::max(x, m.xmin);
      double a = (x - m.xmin) * m.delta_inv();
      long i   = std::min(static_cast<long>(a), m.size() - 2);
      double w = std::min(a - i, 1.0); //NOLINT
      return (1 - w) * f(i) + w * f(i + 1);
    }
  };

} // namespace triqs::mesh
