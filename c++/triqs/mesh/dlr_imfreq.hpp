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
#include "dlr_coeffs.hpp"
#include "domains/matsubara.hpp"

#include <cppdlr/cppdlr.hpp>

#include <memory>

namespace triqs::mesh {

  struct dlr_imfreq {

    using idx_t    = long;
    using datidx_t = long;
    using value_t  = matsubara_freq;

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
    dlr_imfreq() = default;

    /**
     * Construct a DLR mesh in Matsubara frequencies
     *
     * @param beta Inverse temperature
     * @param statistic, Fermion or Boson
     * @param Lambda Lambda energy over beta parameter
     * @param eps Representation accuracy
     */
    dlr_imfreq(double beta, statistic_enum statistic, double Lambda, double eps)
       : dlr_imfreq(beta, statistic, Lambda, eps, cppdlr::build_dlr_rf(Lambda, eps)) {}

    private:
    dlr_imfreq(double beta, statistic_enum statistic, double Lambda, double eps, nda::vector<double> const &dlr_freq)
       : dlr_imfreq(beta, statistic, Lambda, eps,
                    dlr_ops{dlr_freq, {Lambda, dlr_freq}, {Lambda, dlr_freq, static_cast<cppdlr::statistic_t>(statistic)}}) {}

    dlr_imfreq(double beta, statistic_enum statistic, double Lambda, double eps, dlr_ops dlr)
       : _beta(beta),
         _statistic(statistic),
         _Lambda(Lambda),
         _eps(eps),
         _mesh_hash(hash(beta, statistic, Lambda, eps, sum(dlr.imf.get_ifnodes()))),
         _dlr{std::make_shared<dlr_ops>(std::move(dlr))} {}

    friend class dlr_imtime;
    friend class dlr_coeffs;

    public:
    template <any_of<dlr_imtime, dlr_imfreq, dlr_coeffs> M>
    explicit dlr_imfreq(M const &m) : _beta(m._beta), _statistic(m._statistic), _Lambda(m._Lambda), _eps(m._eps), _dlr(m._dlr) {
      if constexpr (std::is_same_v<M, dlr_imfreq>)
        _mesh_hash = m._mesh_hash;
      else
        _mesh_hash = hash(_beta, _statistic, _Lambda, _eps, sum(_dlr->imf.get_ifnodes()));
    }

    // -------------------- Comparisons -------------------

    bool operator==(dlr_imfreq const &m) const { return _mesh_hash == m._mesh_hash; }
    bool operator!=(dlr_imfreq const &m) const { return !(operator==(m)); }

    // -------------------- mesh_point -------------------

    struct mesh_point_t : public matsubara_freq {
      using mesh_t       = dlr_imfreq;
      long datidx        = 0;
      uint64_t mesh_hash = 0;
      [[nodiscard]] matsubara_freq const &value() const { return *this; }

      mesh_point_t() = default;
      mesh_point_t(double beta, statistic_enum statistic, long idx, long datidx_, uint64_t _mesh_hash)
         : matsubara_freq(idx, beta, statistic), datidx(datidx_), mesh_hash(_mesh_hash) {}
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

    [[nodiscard]] std::pair<matsubara_freq, matsubara_freq> min_max_frequencies() const noexcept {
      return {(*this)(0).value(), (*this)(size() - 1).value()};
    }

    /// The total number of points in the mesh
    [[nodiscard]] long size() const noexcept {
      if(_dlr) return _dlr->freq.size();
      else return 0;
    }

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
      return {_beta, _statistic, _dlr->imf.get_ifnodes()[idx], idx, _mesh_hash};
    }

    // -------------------- to_value ------------------

    [[nodiscard]] matsubara_freq to_value(idx_t idx) const noexcept {
      EXPECTS(is_idx_valid(idx));
      return {_dlr->imf.get_ifnodes()[idx], _beta, _statistic};
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

    friend std::ostream &operator<<(std::ostream &sout, dlr_imfreq const &m) {
      auto stat_cstr = (m._statistic == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("DLR imfreq mesh of size {} with beta = {}, statistic = {}, Lambda = {}, eps = {}", m._beta, m.size(), stat_cstr,
                                 m._Lambda, m._eps);
    }

    // -------------------- HDF5 -------------------

    [[nodiscard]] static std::string hdf5_format() { return "MeshDLRImFreq"; }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string const &subgroup_name, dlr_imfreq const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, m);

      h5::write(gr, "beta", m._beta);
      h5::write(gr, "statistic", (m._statistic == Fermion ? "F" : "B"));
      h5::write(gr, "Lambda", m._Lambda);
      h5::write(gr, "eps", m._eps);
      h5::write(gr, "dlr_freq", m.dlr_freq());
      h5::write(gr, "dlr_it", m.dlr_it());
      h5::write(gr, "dlr_if", m.dlr_if());
    }

    /// Read from HDF5
    friend void h5_read(h5::group fg, std::string const &subgroup_name, dlr_imfreq &m) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format(gr, m, true);

      auto beta      = h5::read<double>(gr, "beta");
      auto statistic = (h5::read<std::string>(gr, "statistic") == "F" ? Fermion : Boson);
      auto Lambda    = h5::read<double>(gr, "Lambda");
      auto eps       = h5::read<double>(gr, "eps");
      auto _dlr_freq = h5::read<nda::vector<double>>(gr, "dlr_freq");
      auto _dlr_it   = h5::read<cppdlr::imtime_ops>(gr, "dlr_it");
      auto _dlr_if   = h5::read<cppdlr::imfreq_ops>(gr, "dlr_if");
      m              = dlr_imfreq(beta, statistic, Lambda, eps, {_dlr_freq, _dlr_it, _dlr_if});
    }
  };

  double evaluate(dlr_imfreq const &m, ...) = delete;

  // check concept
  static_assert(MeshWithValues<dlr_imfreq>);

} // namespace triqs::mesh
