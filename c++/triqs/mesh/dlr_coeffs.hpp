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

    double beta              = 0.0;
    statistic_enum statistic = Fermion;
    double Lambda            = 1e+10;
    double eps               = 1e-10;

    private:
    uint64_t mesh_hash_ = 0;
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
       : beta(beta),
         statistic(statistic),
         Lambda(Lambda),
         eps(eps),
         mesh_hash_(hash(beta, statistic, Lambda, eps, sum(dlr.freq))),
         _dlr{std::make_shared<dlr_ops>(std::move(dlr))} {}

    friend class dlr_imtime;
    friend class dlr_imfreq;

    public:
    template <any_of<dlr_imtime, dlr_imfreq, dlr_coeffs> M>
    explicit dlr_coeffs(M const &m) : beta(m.beta), statistic(m.statistic), Lambda(m.Lambda), eps(m.eps), _dlr(m._dlr) {
      if constexpr (std::is_same_v<M, dlr_coeffs>)
        mesh_hash_ = m.mesh_hash_;
      else
        mesh_hash_ = hash(beta, statistic, Lambda, eps, sum(_dlr->freq));
    }

    // -------------------- Comparisons -------------------

    bool operator==(dlr_coeffs const &m) const { return mesh_hash_ == m.mesh_hash_; }
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

    [[nodiscard]] auto const &dlr_freq() const { return _dlr->freq; }

    [[nodiscard]] auto const &dlr_it() const { return _dlr->imt; }

    [[nodiscard]] auto const &dlr_if() const { return _dlr->imf; }

    [[nodiscard]] uint64_t mesh_hash() const noexcept { return mesh_hash_; }

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
      return {idx, idx, mesh_hash_, to_value(idx)};
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
      auto stat_cstr = (m.statistic == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("DLR coefficient mesh of size {} with beta = {}, statistic = {}, Lambda = {}, eps = {}", m.beta, m.size(), stat_cstr,
                                 m.Lambda, m.eps);
    }

    // -------------------- HDF5 -------------------

    [[nodiscard]] static std::string hdf5_format() { return "MeshDLRImTime"; }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string const &subgroup_name, dlr_coeffs const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, m);

      h5::write(gr, "beta", m.beta);
      h5::write(gr, "statistic", (m.statistic == Fermion ? "F" : "B"));
      h5::write(gr, "Lambda", m.Lambda);
      h5::write(gr, "eps", m.eps);
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
    auto res = make_regular(f(0) * cppdlr::k_it(tau / m.beta, m.dlr_freq()[0]));
    for (auto l : range(1, m.size())) res += f(l) * cppdlr::k_it(tau / m.beta, m.dlr_freq()[l]);
    return res;
  }

  auto evaluate(dlr_coeffs const &m, auto const &f, matsubara_freq const &iw) {
    EXPECTS(m.size() > 0);
    auto res = make_regular(-f(0) * cppdlr::k_if(2 * iw.n + iw.statistic, m.dlr_freq()[0]) * m.beta);
    for (auto l : range(1, m.size())) { res += -f(l) * cppdlr::k_if(2 * iw.n + iw.statistic, m.dlr_freq()[l]) * m.beta; }
    return res;
  }

  // check concept
  static_assert(MeshWithValues<dlr_coeffs>);

} // namespace triqs::mesh
