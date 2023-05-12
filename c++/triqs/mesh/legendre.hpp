// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once

#include "utils.hpp"
#include <triqs/utility/legendre.hpp>

namespace triqs::mesh {

  struct legendre {

    using idx_t    = long;
    using datidx_t = long;

    // -------------------- Data -------------------

    double beta              = 0.0;
    statistic_enum statistic = Fermion;

    private:
    long max_n_         = 1;
    uint64_t mesh_hash_ = 0;

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
       : beta(beta), statistic(statistic), max_n_(max_n), mesh_hash_(hash(beta, statistic, max_n)) {}

    // -------------------- Comparison -------------------

    bool operator==(legendre const &) const = default;
    bool operator!=(legendre const &) const = default;

    // --------------------  Mesh Point -------------------

    struct mesh_point_t {
      using mesh_t       = legendre;
      long idx           = 0;
      long datidx        = 0;
      uint64_t mesh_hash = 0;
    };

    // -------------------- idx checks and conversions -------------------

    [[nodiscard]] bool is_idx_valid(idx_t idx) const noexcept { return 0 <= idx and idx < max_n_; }

    [[nodiscard]] datidx_t to_datidx(idx_t idx) const noexcept {
      EXPECTS(is_idx_valid(idx));
      return idx;
    }

    [[nodiscard]] idx_t to_idx(long datidx) const noexcept {
      EXPECTS(is_idx_valid(datidx));
      return datidx;
    }

    [[nodiscard]] mesh_point_t operator[](long datidx) const { return {to_idx(datidx), datidx, mesh_hash_}; }

    [[nodiscard]] mesh_point_t operator()(long idx) const { return {idx, to_datidx(idx), mesh_hash_}; }

    // -------------------- Accessors -------------------

    /// The Hash for the mesh configuration
    [[nodiscard]] uint64_t mesh_hash() const { return mesh_hash_; }

    /// The total number of points in the mesh
    [[nodiscard]] long size() const { return max_n_; }

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
      auto stat_cstr = (m.statistic == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("Legendre mesh with beta = {}, statistic = {}, max_n = {}", m.beta, stat_cstr, m.size());
    }

    // -------------------- HDF5 -------------------

    [[nodiscard]] static std::string hdf5_format() { return "MeshLegendre"; }

    friend void h5_write(h5::group fg, std::string const &subgroup_name, legendre const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, m);

      h5::write(gr, "beta", m.beta);
      h5::write(gr, "statistic", (m.statistic == Fermion ? "F" : "B"));
      h5::write(gr, "max_n", m.max_n_);
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

  inline decltype(auto) evaluate(legendre const &m, auto const &f, double tau) {
    EXPECTS(m.size() > 0);

    utility::legendre_generator L{};
    L.reset(2 * tau / m.beta - 1);

    // OPFIXME : again the sum (transform ) pattern ...
    auto res = make_regular(f(0) * L.next() / m.beta);
    for (auto l : range(1, m.size())) { res += make_regular(f(l) * std::sqrt(2 * l + 1) * L.next() / m.beta); }

    return res;
  }

  // check concept
  static_assert(Mesh<legendre>);

} // namespace triqs::mesh
