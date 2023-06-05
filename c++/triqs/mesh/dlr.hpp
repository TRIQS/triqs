// Copyright (c) 2023 Simons Foundation
// Copyright (c) 2023 Hugo Strand
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
// Authors: Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell

#pragma once
#include "utils.hpp"
#include "domains/matsubara.hpp"
#include <cppdlr/cppdlr.hpp>
#include <memory>

namespace triqs::mesh {

  struct dlr_imtime;
  struct dlr_imfreq;

  struct dlr_ops {
    nda::vector<double> freq;
    cppdlr::imtime_ops imt;
    cppdlr::imfreq_ops imf;
  };

  struct dlr {

    using index_t      = long;
    using data_index_t = long;
    using value_t      = double;

    // -------------------- Data -------------------

    private:
    double _beta                        = 1.0;
    statistic_enum _statistic           = Fermion;
    double _w_max                       = 0.0;
    double _eps                         = 1e-10;
    uint64_t _mesh_hash                 = 0;
    std::shared_ptr<const dlr_ops> _dlr = {};

    // -------------------- Constructors -------------------
    public:
    dlr() = default;

    /**
     * Construct a DLR coefficient mesh
     *
     * @param beta Inverse temperature
     * @param statistic, Fermion or Boson
     * @param w_max DLR energy cutoff, same as Lambda / beta
     * @param eps Representation accuracy
     */
    dlr(double beta, statistic_enum statistic, double w_max, double eps)
       : dlr(beta, statistic, w_max, eps, cppdlr::build_dlr_rf(w_max * beta, eps)) {}

    private:
    dlr(double beta, statistic_enum statistic, double w_max, double eps, nda::vector<double> const &dlr_freq)
       : dlr(beta, statistic, w_max, eps,
             dlr_ops{dlr_freq, {w_max * beta, dlr_freq}, {w_max * beta, dlr_freq, static_cast<cppdlr::statistic_t>(statistic)}}) {}

    dlr(double beta, statistic_enum statistic, double w_max, double eps, dlr_ops dlr)
       : _beta(beta),
         _statistic(statistic),
         _w_max(w_max),
         _eps(eps),
         _mesh_hash(hash(beta, statistic, w_max, eps, sum(dlr.freq))),
         _dlr{std::make_shared<dlr_ops>(std::move(dlr))} {}

    friend class dlr_imtime;
    friend class dlr_imfreq;

    public:
    template <any_of<dlr_imtime, dlr_imfreq, dlr> M>
    explicit dlr(M const &m) : _beta(m._beta), _statistic(m._statistic), _w_max(m._w_max), _eps(m._eps), _dlr(m._dlr) {
      if constexpr (std::is_same_v<M, dlr>)
        _mesh_hash = m._mesh_hash;
      else
        _mesh_hash = hash(_beta, _statistic, _w_max, _eps, sum(_dlr->freq));
    }

    // -------------------- Comparisons -------------------

    bool operator==(dlr const &m) const { return _mesh_hash == m._mesh_hash; }
    bool operator!=(dlr const &m) const { return !(operator==(m)); }

    // -------------------- mesh_point -------------------

    struct mesh_point_t {
      using mesh_t = dlr;

      private:
      long _index         = 0;
      long _data_index    = 0;
      uint64_t _mesh_hash = 0;
      double _value       = {};

      public:
      mesh_point_t() = default;
      mesh_point_t(long index, long data_index, uint64_t mesh_hash, double value)
         : _index(index), _data_index(data_index), _mesh_hash(mesh_hash), _value(value) {}

      /// The index of the mesh point
      [[nodiscard]] long index() const { return _index; }

      /// The data index of the mesh point
      [[nodiscard]] long data_index() const { return _data_index; }

      /// The value of the mesh point
      [[nodiscard]] double value() const { return _value; }

      /// The Hash for the mesh configuration
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }

      operator double() const { return _value; }
    };

    // -------------------- Accessors -------------------

    /// The inverse temperature
    [[nodiscard]] double beta() const noexcept { return _beta; }

    /// The particle statistic: Fermion or Boson
    [[nodiscard]] statistic_enum statistic() const noexcept { return _statistic; }

    /// DLR energy cutoff, w_max = Lambda / beta
    [[nodiscard]] double w_max() const noexcept { return _w_max; }

    /// Representation accuracy
    [[nodiscard]] double eps() const noexcept { return _eps; }

    /// The vector of DLR frequencies
    [[nodiscard]] auto const &dlr_freq() const { return _dlr->freq; }

    /// The imaginary time DLR operations object
    [[nodiscard]] auto const &dlr_it() const { return _dlr->imt; }

    /// The Matsubara frequency DLR operations object
    [[nodiscard]] auto const &dlr_if() const { return _dlr->imf; }

    /// The Hash for the mesh configuration
    [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }

    /// The total number of points in the mesh
    [[nodiscard]] long size() const noexcept { return (_dlr ? _dlr->freq.size() : 0); }

    // -------------------- index checks and conversions -------------------

    [[nodiscard]] bool is_index_valid(long index) const noexcept { return 0 <= index and index < size(); }

    [[nodiscard]] long to_data_index(long index) const noexcept {
      EXPECTS(is_index_valid(index));
      return index;
    }

    [[nodiscard]] long to_index(long data_index) const noexcept {
      EXPECTS(is_index_valid(data_index));
      return data_index;
    }

    // -------------------- operator [] () -------------------

    [[nodiscard]] mesh_point_t operator[](long data_index) const { return (*this)(data_index); }

    [[nodiscard]] mesh_point_t operator()(long index) const {
      EXPECTS(is_index_valid(index));
      return {index, index, _mesh_hash, to_value(index)};
    }

    // -------------------- to_value ------------------

    [[nodiscard]] double to_value(long index) const noexcept {
      EXPECTS(is_index_valid(index));
      return (_dlr->freq)[index];
    }

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

    friend std::ostream &operator<<(std::ostream &sout, dlr const &m) {
      auto stat_cstr = (m._statistic == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("DLR coefficient mesh of size {} with beta = {}, statistic = {}, w_max = {}, eps = {}", m._beta, m.size(), stat_cstr,
                                 m._w_max, m._eps);
    }

    // -------------------- HDF5 -------------------

    [[nodiscard]] static std::string hdf5_format() { return "MeshDLRImTime"; }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string const &subgroup_name, dlr const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, m); // NOLINT

      h5::write(gr, "beta", m._beta);
      h5::write(gr, "statistic", (m._statistic == Fermion ? "F" : "B"));
      h5::write(gr, "w_max", m._w_max);
      h5::write(gr, "eps", m._eps);
      h5::write(gr, "dlr_freq", m.dlr_freq());
      h5::write(gr, "dlr_it", m.dlr_it());
      h5::write(gr, "dlr_if", m.dlr_if());
    }

    /// Read from HDF5
    friend void h5_read(h5::group fg, std::string const &subgroup_name, dlr &m) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format(gr, m, true);

      auto beta      = h5::read<double>(gr, "beta");
      auto statistic = (h5::read<std::string>(gr, "statistic") == "F" ? Fermion : Boson);
      auto w_max     = h5::read<double>(gr, "w_max");
      auto eps       = h5::read<double>(gr, "eps");
      auto _dlr_freq = h5::read<nda::vector<double>>(gr, "dlr_freq");
      auto _dlr_it   = h5::read<cppdlr::imtime_ops>(gr, "dlr_it");
      auto _dlr_if   = h5::read<cppdlr::imfreq_ops>(gr, "dlr_if");
      m              = dlr(beta, statistic, w_max, eps, {_dlr_freq, _dlr_it, _dlr_if});
    }
  };

  // -------------------- evaluation -------------------

  auto evaluate(dlr const &m, auto const &f, double tau) {
    EXPECTS(m.size() > 0);
    return details::sum_to_regular(range(m.size()), [&](auto &&l) { return f(l) * cppdlr::k_it(tau / m.beta(), m.dlr_freq()[l]); });
  }

  auto evaluate(dlr const &m, auto const &f, matsubara_freq const &iw) {
    EXPECTS(m.size() > 0);
    return details::sum_to_regular(range(m.size()),
                                   [&](auto &&l) { return f(l) * cppdlr::k_if(iw.n, m.dlr_freq()[l], (cppdlr::statistic_t) iw.statistic) * m.beta(); });
  }

  // check concept
  static_assert(MeshWithValues<dlr>);

} // namespace triqs::mesh
