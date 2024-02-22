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
// Authors: Alexander Hampel, Olivier Parcollet, Hugo Strand, Nils Wentzell

#pragma once
#include "utils.hpp"
#include "dlr.hpp"
#include "domains/matsubara.hpp"

#include <cppdlr/cppdlr.hpp>

#include <memory>

namespace triqs::mesh {

  struct dlr_imfreq {

    using index_t      = long;
    using data_index_t = long;
    using value_t      = matsubara_freq;

    // -------------------- Data -------------------

    private:
    double _beta                        = 1.0;
    statistic_enum _statistic           = Fermion;
    double _w_max                       = 0.0;
    double _eps                         = 1e-10;
    bool _symmetrize                    = false;
    uint64_t _mesh_hash                 = 0;
    std::shared_ptr<const dlr_ops> _dlr = {};

    // -------------------- Constructors -------------------
    public:
    dlr_imfreq() = default;

    /**
     * Construct a DLR mesh in Matsubara frequencies
     *
     * The mesh-point for a given linear_index `i` can be otained
     * through `m[i]` and for an index `n` through `m(n)`
     *
     * The associated Green function does not allow for evaluation on
     * arbitrary Matsubara frequencies. For this use the Green function
     * on the associated dlr_mesh.
     *
     * @param beta Inverse temperature
     * @param statistic, Fermion or Boson
     * @param w_max DLR energy cutoff, same as Lambda / beta
     * @param eps Representation accuracy
     * @param symmetrize Experimental! Whether to choose the frequencies symmetrically
     *            around 0. For fermionic/bosonic statistic enforces even/odd dlr-rank
     *            and number of frequencies [default = false]
     */
    dlr_imfreq(double beta, statistic_enum statistic, double w_max, double eps, bool symmetrize = false)
       : dlr_imfreq(beta, statistic, w_max, eps, symmetrize,
                    cppdlr::build_dlr_rf(w_max * beta, eps, symmetrize)) {}

    private:
    dlr_imfreq(double beta, statistic_enum statistic, double w_max, double eps, bool symmetrize, nda::vector<double> const &dlr_freq)
       : dlr_imfreq(beta, statistic, w_max, eps, symmetrize,
                    dlr_ops{dlr_freq,
                            {w_max * beta, dlr_freq, symmetrize},
                            {w_max * beta, dlr_freq, static_cast<cppdlr::statistic_t>(statistic), symmetrize}}) {}

    dlr_imfreq(double beta, statistic_enum statistic, double w_max, double eps, bool symmetrize, dlr_ops dlr)
       : _beta(beta),
         _statistic(statistic),
         _w_max(w_max),
         _eps(eps),
         _symmetrize(symmetrize),
         _mesh_hash(hash(beta, statistic, w_max, eps, sum(dlr.imf.get_ifnodes()))),
         _dlr{std::make_shared<dlr_ops>(std::move(dlr))} {}

    friend class dlr_imtime;
    friend class dlr;

    public:
    template <any_of<dlr_imtime, dlr_imfreq, dlr> M>
    explicit dlr_imfreq(M const &m)
       : _beta(m._beta), _statistic(m._statistic), _w_max(m._w_max), _eps(m._eps), _symmetrize(m._symmetrize), _dlr(m._dlr) {
      if constexpr (std::is_same_v<M, dlr_imfreq>)
        _mesh_hash = m._mesh_hash;
      else
        _mesh_hash = hash(_beta, _statistic, _w_max, _eps, sum(_dlr->imf.get_ifnodes()));
    }

    // -------------------- Comparisons -------------------

    bool operator==(dlr_imfreq const &m) const { return _mesh_hash == m._mesh_hash; }
    bool operator!=(dlr_imfreq const &m) const { return !(operator==(m)); }

    // -------------------- mesh_point -------------------

    struct mesh_point_t : public matsubara_freq {
      using mesh_t = dlr_imfreq;

      private:
      long _data_index    = 0;
      uint64_t _mesh_hash = 0;

      public:
      mesh_point_t() = default;
      mesh_point_t(double beta, statistic_enum statistic, long index, long data_index, uint64_t mesh_hash)
         : matsubara_freq(index, beta, statistic), _data_index(data_index), _mesh_hash(mesh_hash) {}

      /// The index of the mesh point
      [[nodiscard]] long index() const { return n; }

      /// The data index of the mesh point
      [[nodiscard]] long data_index() const { return _data_index; }

      /// The value of the mesh point
      [[nodiscard]] matsubara_freq const &value() const { return *this; }

      /// The Hash for the mesh configuration
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }
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

    [[nodiscard]] std::pair<matsubara_freq, matsubara_freq> min_max_frequencies() const noexcept {
      return {(*this)(0).value(), (*this)(size() - 1).value()};
    }

    /// The total number of points in the mesh
    [[nodiscard]] long size() const noexcept { return (_dlr ? _dlr->imf.get_ifnodes().size() : 0); }

    // -------------------- index checks and conversions -------------------

    [[nodiscard]] bool is_index_valid(long idx) const noexcept { return 0 <= idx and idx < size(); }

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
      return {_beta, _statistic, _dlr->imf.get_ifnodes()[index], index, _mesh_hash};
    }

    // -------------------- to_value ------------------

    [[nodiscard]] matsubara_freq to_value(long index) const noexcept {
      EXPECTS(is_index_valid(index));
      return {_dlr->imf.get_ifnodes()[index], _beta, _statistic};
    }

    // -------------------------- Range & Iteration --------------------------

    private:
    [[nodiscard]] auto r_() const {
      return itertools::transform(nda::range(size()), [this](long i) { return (*this)[i]; });
    }

    public:
    [[nodiscard]] auto begin() const { return r_().begin(); }
    [[nodiscard]] auto cbegin() const { return r_().cbegin(); }
    [[nodiscard]] auto end() const { return r_().end(); }
    [[nodiscard]] auto cend() const { return r_().cend(); }

    // -------------------- print  -------------------

    friend std::ostream &operator<<(std::ostream &sout, dlr_imfreq const &m) {
      auto stat_cstr = (m._statistic == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("DLR imfreq mesh of size {} with beta = {}, statistic = {}, w_max = {}, eps = {}", m.size(), m._beta, stat_cstr,
                                 m._w_max, m._eps);
    }

    // -------------------- HDF5 -------------------

    [[nodiscard]] static std::string hdf5_format() { return "MeshDLRImFreq"; }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string const &subgroup_name, dlr_imfreq const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, m);

      h5::write(gr, "beta", m._beta);
      h5::write(gr, "statistic", (m._statistic == Fermion ? "F" : "B"));
      h5::write(gr, "w_max", m._w_max);
      h5::write(gr, "eps", m._eps);
      h5::write(gr, "symmetrize", m._symmetrize);
      h5::write(gr, "dlr_freq", m.dlr_freq());
      h5::write(gr, "dlr_it", m.dlr_it());
      h5::write(gr, "dlr_if", m.dlr_if());
    }

    /// Read from HDF5
    friend void h5_read(h5::group fg, std::string const &subgroup_name, dlr_imfreq &m) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format(gr, m, true);

      auto beta       = h5::read<double>(gr, "beta");
      auto statistic  = (h5::read<std::string>(gr, "statistic") == "F" ? Fermion : Boson);
      auto w_max      = h5::read<double>(gr, "w_max");
      auto eps        = h5::read<double>(gr, "eps");
      auto symmetrize = h5::read<bool>(gr, "symmetrize");
      auto _dlr_freq  = h5::read<nda::vector<double>>(gr, "dlr_freq");
      auto _dlr_it    = h5::read<cppdlr::imtime_ops>(gr, "dlr_it");
      auto _dlr_if    = h5::read<cppdlr::imfreq_ops>(gr, "dlr_if");
      m               = dlr_imfreq(beta, statistic, w_max, eps, symmetrize, {_dlr_freq, _dlr_it, _dlr_if});
    }
  };

  double evaluate(dlr_imfreq const &m, ...) = delete;

  // check concept
  static_assert(MeshWithValues<dlr_imfreq>);

} // namespace triqs::mesh
