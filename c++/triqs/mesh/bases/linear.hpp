// Copyright (c) 2013-2015 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2015 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2019-2023 Simons Foundation
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
// Authors: Philipp Dumitrescu, Michel Ferrero, Henri Menke, Olivier Parcollet, Nils Wentzell

#pragma once
#include <ranges>
#include <stdexcept>
#include "../utils.hpp"

namespace triqs::mesh::details {

  // Implements linear spaced meshes as MeshWithValues by CTRP on Mesh
  // Value is value_t of MeshWithValues concept.
  // Implementation only. Not user
  template <typename Mesh, typename Value> class linear {
    static_assert(std::totally_ordered<Value>);

    public:
    using value_t      = Value;
    using index_t      = long;
    using data_index_t = long;

    // --  data
    protected:
    long L;
    value_t xmin, xmax, delta_x;
    double delta_x_inv;
    size_t _mesh_hash = 0;

    // -------------------- Constructors -------------------

    linear(value_t a = 0, value_t b = 1, long n_pts = 2)
       : L(n_pts),
         xmin(a),
         xmax(b),
         delta_x(L == 1 ? 0. : (b - a) / (L - 1)),
         delta_x_inv{delta_x == 0.0 ? std::numeric_limits<double>::infinity() : 1. / delta_x},
         _mesh_hash(hash(L, xmin, xmax)) {
      EXPECTS(a <= b);
    }

    public:
    // -------------------- Comparison -------------------

    /// Mesh comparison
    bool operator==(linear const &) const = default;
    bool operator!=(linear const &) const = default;

    // --------------------  Mesh Point -------------------

    struct mesh_point_t {
      using mesh_t = Mesh;

      public:
      long _index         = 0;
      long _data_index    = 0;
      uint64_t _mesh_hash = 0;
      value_t _value      = {};

      public:
      mesh_point_t() = default;
      mesh_point_t(long index, long data_index, uint64_t mesh_hash, double value)
         : _index(index), _data_index(data_index), _mesh_hash(mesh_hash), _value(value) {}

      /// The index of the mesh point
      [[nodiscard]] long index() const { return _index; }

      /// The data index of the mesh point
      [[nodiscard]] long data_index() const { return _data_index; }

      /// The value of the mesh point
      [[nodiscard]] value_t value() const { return _value; }

      /// The Hash for the mesh configuration
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }

      operator value_t() const { return _value; }

      // https://godbolt.org/z/xoYP3vTW4
#define IMPL_OP(OP)                                                                                                                                  \
  template <typename T> friend auto operator OP(mesh_point_t const &mp, T &&y) { return mp.value() OP std::forward<T>(y); }                          \
  template <typename T>                                                                                                                              \
  friend auto operator OP(T &&x, mesh_point_t const &mp)                                                                                             \
    requires(not std::is_same_v<std::decay_t<T>, mesh_point_t>)                                                                                      \
  {                                                                                                                                                  \
    return std::forward<T>(x) OP mp.value();                                                                                                         \
  }
      IMPL_OP(+);
      IMPL_OP(-);
      IMPL_OP(*);
      IMPL_OP(/);
#undef IMPL_OP
    };

    // -------------------- checks -------------------

    [[nodiscard]] bool is_index_valid(index_t index) const noexcept { return 0 <= index and index < L; }

    private:
    [[nodiscard]] bool is_value_valid(value_t val) const noexcept { return xmin <= val and val <= xmax; }

    // -------------------- to_data_index ------------------
    public:
    [[nodiscard]] data_index_t to_data_index(index_t index) const noexcept {
      EXPECTS(is_index_valid(index));
      return index;
    }

    [[nodiscard]] index_t to_data_index(closest_mesh_point_t<value_t> const &cmp) const noexcept {
      EXPECTS(is_value_valid(cmp.value));
      return to_data_index(to_index(cmp));
    }

    // ------------------ to_index -------------------

    [[nodiscard]] index_t to_index(data_index_t data_index) const noexcept {
      EXPECTS(is_index_valid(data_index));
      return data_index;
    }

    [[nodiscard]] index_t to_index(closest_mesh_point_t<value_t> const &cmp) const noexcept {
      EXPECTS(is_value_valid(cmp.value));
      return index_t((cmp.value - xmin) * delta_x_inv + 0.5);
    }

    // ------------------ operator [] () -------------------

    [[nodiscard]] mesh_point_t operator[](long data_index) const noexcept { return (*this)(data_index); }

    [[nodiscard]] mesh_point_t operator[](closest_mesh_point_t<value_t> const &cmp) const noexcept { return (*this)[this->to_data_index(cmp)]; }

    [[nodiscard]] mesh_point_t operator()(index_t index) const noexcept {
      EXPECTS(is_index_valid(index));
      return {index, index, _mesh_hash, to_value(index)};
    }

    // -------------------- to_value ------------------

    [[nodiscard]] value_t to_value(index_t index) const noexcept {
      EXPECTS(is_index_valid(index));
      if (L == 1) return xmin;
      double wr  = double(index) / (L - 1);
      double res = xmin * (1 - wr) + xmax * wr;
      return res;
    }

    // -------------------- Accessors -------------------

    /// The Hash for the mesh configuration
    [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }

    /// The total number of points in the mesh
    [[nodiscard]] long size() const noexcept { return L; }

    /// Step of the mesh
    [[nodiscard]] value_t delta() const noexcept { return delta_x; }

    /// Inverse of the step of the mesh
    [[nodiscard]] value_t delta_inv() const noexcept { return delta_x_inv; }

    /// First index of the mesh
    static constexpr long first_index() { return 0; }

    /// Last index of the mesh
    [[nodiscard]] long last_index() const { return L - 1; }

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

    protected:
    //  -------------------------- HDF5  --------------------------

    void h5_write_impl(h5::group fg, std::string const &subgroup_name, const char *format) const {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format_as_string(gr, format);
      h5::write(gr, "min", this->xmin);
      h5::write(gr, "max", this->xmax);
      h5::write(gr, "size", this->size());
    }

    /// Read from HDF5
    void h5_read_impl(h5::group fg, std::string const &subgroup_name, const char *format_expected) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format_as_string(gr, format_expected, true);

      auto a  = h5::read<value_t>(gr, "min");
      auto b  = h5::read<value_t>(gr, "max");
      auto sz = h5::read<long>(gr, "size");
      *this   = linear(a, b, sz);
    }

    public:
    // it is fine in public, it is equivalent to the function via UFCS
    // ------------------------- Evaluate -----------------------------

    auto evaluate(auto const &f, double x) const {
      EXPECTS(this->is_value_valid(x) and this->size() > 1);
      x        = std::max(x, this->xmin);
      double a = (x - this->xmin) * this->delta_inv();
      long i   = std::min(static_cast<long>(a), this->size() - 2);
      double w = std::min(a - i, 1.0); //NOLINT
      return (1 - w) * f(i) + w * f(i + 1);
    }
  };

} // namespace triqs::mesh::details
