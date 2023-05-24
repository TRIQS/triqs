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

    using index_t      = long;
    using data_index_t = long;

    // -------------------- Data -------------------
    private:
    long L_;
    uint64_t _mesh_hash = 0;

    // -------------------- Constructors -------------------
    public:
    index(long L = 0) : L_(L), _mesh_hash(L) {}

    // -------------------- Comparison -------------------

    bool operator==(index const &) const = default;
    bool operator!=(index const &) const = default;

    // --------------------  Mesh Point -------------------

    struct mesh_point_t {
      using mesh_t = index;

      private:
      long _index         = 0;
      long _data_index    = 0;
      uint64_t _mesh_hash = 0;

      public:
      mesh_point_t() = default;
      mesh_point_t(long index, long data_index, uint64_t mesh_hash) : _index(index), _data_index(data_index), _mesh_hash(mesh_hash) {}

      /// The index of the mesh point
      [[nodiscard]] long index() const { return _index; }

      /// The data index of the mesh point
      [[nodiscard]] long data_index() const { return _data_index; }

      /// The Hash for the mesh configuration
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }
    };

    // -------------------- index checks and conversions -------------------

    [[nodiscard]] bool is_index_valid(index_t idx) const noexcept { return 0 <= idx and idx < L_; }

    // -------------------- to_data_index -------------------

    [[nodiscard]] data_index_t to_data_index(index_t index) const noexcept {
      EXPECTS(is_index_valid(index));
      return index;
    }

    // -------------------- to_index -------------------

    [[nodiscard]] index_t to_index(long data_index) const noexcept {
      EXPECTS(is_index_valid(data_index));
      return data_index;
    }

    // -------------------- operator[] -------------------

    [[nodiscard]] mesh_point_t operator[](long data_index) const {
      auto index = to_index(data_index);
      EXPECTS(is_index_valid(index));
      return {index, data_index, _mesh_hash};
    }

    [[nodiscard]] mesh_point_t operator()(index_t index) const {
      EXPECTS(is_index_valid(index));
      auto data_index = to_data_index(index);
      return {index, data_index, _mesh_hash};
    }

    // -------------------- Accessors -------------------

    /// The Hash for the mesh configuration
    [[nodiscard]] uint64_t mesh_hash() const { return _mesh_hash; }

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
