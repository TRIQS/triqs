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

  struct dlr_coeffs {

    using idx_t    = long;
    using datidx_t = long;
    using value_t  = double;

    // -------------------- Data -------------------

    private:
    double _beta              = 0.0;
    statistic_enum _statistic = Fermion;
    double _Lambda            = 1e+10;
    double _eps               = 1e-10;
    uint64_t _mesh_hash       = 0;
    std::shared_ptr<const dlr_ops> _dlr;

    // -------------------- Constructors -------------------
    public:
    dlr_coeffs() = default;

    /**
     * Construct a DLR coefficient mesh
     *
     * @param beta Inverse temperature
     * @param statistic, Fermion or Boson
     * @param Lambda Lambda energy over beta parameter
     * @param eps Representation accuracy
     */
    dlr_coeffs(double beta, statistic_enum statistic, double Lambda, double eps)
       : dlr_coeffs(beta, statistic, Lambda, eps, cppdlr::build_dlr_rf(Lambda, eps)) {}

    private:
    dlr_coeffs(double beta, statistic_enum statistic, double Lambda, double eps, nda::vector<double> const &dlr_freq)
       : dlr_coeffs(beta, statistic, Lambda, eps,
                    dlr_ops{dlr_freq, {Lambda, dlr_freq}, {Lambda, dlr_freq, static_cast<cppdlr::statistic_t>(statistic)}}) {}

    dlr_coeffs(double beta, statistic_enum statistic, double Lambda, double eps, dlr_ops dlr)
       : _beta(beta),
         _statistic(statistic),
         _Lambda(Lambda),
         _eps(eps),
         _mesh_hash(hash(beta, statistic, Lambda, eps, sum(dlr.freq))),
         _dlr{std::make_shared<dlr_ops>(std::move(dlr))} {}

    friend class dlr_imtime;
    friend class dlr_imfreq;

    public:
    template <any_of<dlr_imtime, dlr_imfreq, dlr_coeffs> M>
    explicit dlr_coeffs(M const &m) : _beta(m._beta), _statistic(m._statistic), _Lambda(m._Lambda), _eps(m._eps), _dlr(m._dlr) {
      if constexpr (std::is_same_v<M, dlr_coeffs>)
        _mesh_hash = m._mesh_hash;
      else
        _mesh_hash = hash(_beta, _statistic, _Lambda, _eps, sum(_dlr->freq));
    }

    // -------------------- Comparisons -------------------

    bool operator==(dlr_coeffs const &m) const { return _mesh_hash == m._mesh_hash; }
    bool operator!=(dlr_coeffs const &m) const { return !(operator==(m)); }

    // -------------------- mesh_point -------------------

    struct mesh_point_t {
      using mesh_t       = dlr_coeffs;
      long idx           = 0;
      long datidx        = 0;
      uint64_t mesh_hash = 0;
      double val         = {};
      [[nodiscard]] double value() const { return val; }
      operator double() const { return val; }
    };

    // -------------------- Accessors -------------------

    /// The inverse temperature
    [[nodiscard]] double beta() const noexcept { return _beta; }

    /// The particle statistic: Fermion or Boson
    [[nodiscard]] statistic_enum statistic() const noexcept { return _statistic; }

    /// DLR energy cutoff Lambda = beta*w_max
    [[nodiscard]] double Lambda() const noexcept { return _Lambda; }

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
    [[nodiscard]] long size() const noexcept { return _dlr->freq.size(); }

    // -------------------- idx checks and conversions -------------------

    [[nodiscard]] bool is_idx_valid(idx_t idx) const noexcept { return 0 <= idx and idx < size(); }

    [[nodiscard]] datidx_t to_datidx(idx_t idx) const noexcept {
      EXPECTS(is_idx_valid(idx));
      return idx;
    }

    [[nodiscard]] idx_t to_idx(long datidx) const noexcept {
      EXPECTS(is_idx_valid(datidx));
      return datidx;
    }

    // -------------------- operator [] () -------------------

    [[nodiscard]] mesh_point_t operator[](long datidx) const { return (*this)(datidx); }

    [[nodiscard]] mesh_point_t operator()(idx_t idx) const {
      EXPECTS(is_idx_valid(idx));
      return {idx, idx, _mesh_hash, to_value(idx)};
    }

    // -------------------- to_value ------------------

    [[nodiscard]] double to_value(idx_t idx) const noexcept {
      EXPECTS(is_idx_valid(idx));
      return (_dlr->freq)[idx];
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

    friend std::ostream &operator<<(std::ostream &sout, dlr_coeffs const &m) {
      auto stat_cstr = (m._statistic == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("DLR coefficient mesh of size {} with beta = {}, statistic = {}, Lambda = {}, eps = {}", m._beta, m.size(),
                                 stat_cstr, m._Lambda, m._eps);
    }

    // -------------------- HDF5 -------------------

    [[nodiscard]] static std::string hdf5_format() { return "MeshDLRImTime"; }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string const &subgroup_name, dlr_coeffs const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, m); // NOLINT

      h5::write(gr, "beta", m._beta);
      h5::write(gr, "statistic", (m._statistic == Fermion ? "F" : "B"));
      h5::write(gr, "Lambda", m._Lambda);
      h5::write(gr, "eps", m._eps);
      h5::write(gr, "dlr_freq", m.dlr_freq());
      h5::write(gr, "dlr_it", m.dlr_it());
      h5::write(gr, "dlr_if", m.dlr_if());
    }

    /// Read from HDF5
    friend void h5_read(h5::group fg, std::string const &subgroup_name, dlr_coeffs &m) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format(gr, m, true);

      auto beta      = h5::read<double>(gr, "beta");
      auto statistic = (h5::read<std::string>(gr, "statistic") == "F" ? Fermion : Boson);
      auto Lambda    = h5::read<double>(gr, "Lambda");
      auto eps       = h5::read<double>(gr, "eps");
      auto _dlr_freq = h5::read<nda::vector<double>>(gr, "dlr_freq");
      auto _dlr_it   = h5::read<cppdlr::imtime_ops>(gr, "dlr_it");
      auto _dlr_if   = h5::read<cppdlr::imfreq_ops>(gr, "dlr_if");
      m              = dlr_coeffs(beta, statistic, Lambda, eps, {_dlr_freq, _dlr_it, _dlr_if});
    }
  };

  // -------------------- evaluation -------------------

  auto evaluate(dlr_coeffs const &m, auto const &f, double tau) {
    EXPECTS(m.size() > 0);
    return details::sum_to_regular(range(m.size()), [&](auto &&l) { return f(l) * cppdlr::k_it(tau / m.beta(), m.dlr_freq()[l]); });
  }

  auto evaluate(dlr_coeffs const &m, auto const &f, matsubara_freq const &iw) {
    EXPECTS(m.size() > 0);
    return details::sum_to_regular(range(m.size()),
                                   [&](auto &&l) { return f(l) * cppdlr::k_if(2 * iw.n + iw.statistic, m.dlr_freq()[l]) * m.beta(); });
  }

  // check concept
  static_assert(MeshWithValues<dlr_coeffs>);

} // namespace triqs::mesh
