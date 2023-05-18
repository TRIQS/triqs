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
#include "./utils.hpp"
namespace triqs::mesh {

  struct index {

    using idx_t    = long;
    using datidx_t = long;

    // -------------------- Data -------------------
    private:
    long L_;
    uint64_t mesh_hash_ = 0;

    // -------------------- Constructors -------------------
    public:
    index(long L = 0) : L_(L), mesh_hash_(L) {}

    // -------------------- Comparison -------------------

    bool operator==(index const &) const = default;
    bool operator!=(index const &) const = default;

    // --------------------  Mesh Point -------------------

    struct mesh_point_t {
      using mesh_t       = index;
      long idx           = 0;
      long datidx        = 0;
      uint64_t mesh_hash = 0;
    };

    // -------------------- idx checks and conversions -------------------

    [[nodiscard]] bool is_idx_valid(idx_t idx) const noexcept { return 0 <= idx and idx < L_; }

    // -------------------- to_datidx -------------------

    [[nodiscard]] datidx_t to_datidx(idx_t idx) const noexcept {
      EXPECTS(is_idx_valid(idx));
      return idx;
    }

    // -------------------- to_idx -------------------

    [[nodiscard]] idx_t to_idx(long datidx) const noexcept {
      EXPECTS(is_idx_valid(datidx));
      return datidx;
    }

    // -------------------- operator[] -------------------

    [[nodiscard]] mesh_point_t operator[](long datidx) const {
      auto idx = to_idx(datidx);
      EXPECTS(is_idx_valid(idx));
      return {idx, datidx, mesh_hash_};
    }

    [[nodiscard]] mesh_point_t operator()(idx_t idx) const {
      EXPECTS(is_idx_valid(idx));
      auto datidx = to_datidx(idx);
      return {idx, datidx, mesh_hash_};
    }

    // -------------------- Accessors -------------------

    /// The Hash for the mesh configuration
    [[nodiscard]] uint64_t mesh_hash() const { return mesh_hash_; }

    /// The total number of points in the mesh
    [[nodiscard]] long size() const { return L_; }

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

    friend std::ostream &operator<<(std::ostream &sout, index const &m) { return sout << "index mesh of size " << m.size(); }

    // -------------------- HDF5 -------------------

    [[nodiscard]] static std::string hdf5_format() { return "MeshIndex"; }

    friend void h5_write(h5::group fg, std::string const &subgroup_name, index const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, m);
      h5_write(gr, "size", m.size());
    }

    friend void h5_read(h5::group fg, std::string const &subgroup_name, index &m) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format(gr, m, true);
      long L = h5_read<long>(gr, "size");
      m      = index(L);
    }
  };

  // check concept
  static_assert(Mesh<index>);

} // namespace triqs::mesh
