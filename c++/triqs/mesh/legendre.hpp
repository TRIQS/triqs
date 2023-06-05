// Copyright (c) 2013-2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2016 Centre national de la recherche scientifique (CNRS)
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
// Authors: Philipp Dumitrescu, Olivier Parcollet, Nils Wentzell

#pragma once

#include "utils.hpp"
#include <triqs/utility/legendre.hpp>

namespace triqs::mesh {

  struct legendre {

    using index_t      = long;
    using data_index_t = long;

    // -------------------- Data -------------------

    private:
    double _beta              = 1.0;
    statistic_enum _statistic = Fermion;
    long _max_n               = 0;
    uint64_t _mesh_hash       = 0;

    // -------------------- Constructors -------------------
    public:
    legendre() = default;

    /**
     *  Construct a Mesh of Legendre polynomials with degrees in the interval [0,max_n]
     *
     *  @param beta Inverse temperature
     *  @param statistic, Fermion or Boson
     *  @param max_n Largest degree
     */
    legendre(double beta, statistic_enum statistic, long max_n)
       : _beta(beta), _statistic(statistic), _max_n(max_n), _mesh_hash(hash(beta, statistic, max_n)) {}

    // -------------------- Comparison -------------------

    bool operator==(legendre const &) const = default;
    bool operator!=(legendre const &) const = default;

    // --------------------  Mesh Point -------------------

    struct mesh_point_t {
      using mesh_t = legendre;

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

    [[nodiscard]] bool is_index_valid(index_t idx) const noexcept { return 0 <= idx and idx < _max_n; }

    // -------------------- to_data_index ------------------

    [[nodiscard]] data_index_t to_data_index(index_t index) const noexcept {
      EXPECTS(is_index_valid(index));
      return index;
    }

    // ------------------ to_index -------------------

    [[nodiscard]] index_t to_index(long data_index) const noexcept {
      EXPECTS(is_index_valid(data_index));
      return data_index;
    }

    // ------------------ operator [] () -------------------

    [[nodiscard]] mesh_point_t operator[](long data_index) const { return {to_index(data_index), data_index, _mesh_hash}; }

    [[nodiscard]] mesh_point_t operator()(long index) const { return {index, to_data_index(index), _mesh_hash}; }

    // -------------------- Accessors -------------------

    /// The inverse temperature
    [[nodiscard]] double beta() const noexcept { return _beta; }

    /// The particle statistic: Fermion or Boson
    [[nodiscard]] statistic_enum statistic() const noexcept { return _statistic; }

    /// The Hash for the mesh configuration
    [[nodiscard]] uint64_t mesh_hash() const { return _mesh_hash; }

    /// The total number of points in the mesh
    [[nodiscard]] long size() const { return _max_n; }

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

    friend std::ostream &operator<<(std::ostream &sout, legendre const &m) {
      auto stat_cstr = (m._statistic == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("Legendre mesh with beta = {}, statistic = {}, max_n = {}", m._beta, stat_cstr, m.size());
    }

    // -------------------- HDF5 -------------------

    [[nodiscard]] static std::string hdf5_format() { return "MeshLegendre"; }

    friend void h5_write(h5::group fg, std::string const &subgroup_name, legendre const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, m);

      h5::write(gr, "beta", m._beta);
      h5::write(gr, "statistic", (m._statistic == Fermion ? "F" : "B"));
      h5::write(gr, "max_n", m._max_n);
    }

    friend void h5_read(h5::group fg, std::string const &subgroup_name, legendre &m) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format(gr, m, true);

      if (gr.has_key("domain")) { gr = gr.open_group("domain"); } // Backward Compat
      auto beta      = h5::read<double>(gr, "beta");
      auto statistic = (h5::read<std::string>(gr, "statistic") == "F" ? Fermion : Boson);

      long max_n{};
      if (not h5::try_read(gr, "max_n", max_n)) h5::read(gr, "n_max", max_n); // Backward Compat

      m = legendre(beta, statistic, max_n);
    }
  };

  inline auto evaluate(legendre const &m, auto const &f, double tau) {
    EXPECTS(m.size() > 0);
    utility::legendre_generator L{};
    L.reset(2 * tau / m.beta() - 1);
    return details::sum_to_regular(range(m.size()), [&](auto &&l) { return f(l) * std::sqrt(2 * l + 1) * L.next() / m.beta(); });
  }

  // check concept
  static_assert(Mesh<legendre>);

} // namespace triqs::mesh
