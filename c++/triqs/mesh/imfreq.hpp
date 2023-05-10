// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell, tayral

#pragma once
#include "./utils.hpp"
#include "./domains/matsubara.hpp"
#include "./tail_fitter.hpp"

namespace triqs::mesh {

  /// A double with no implicit conversion.
  struct energy_t {
    double value = 0;
    explicit operator double() const { return value; }
  };

  /**
   *  Matsubara frequencies 
   *   - fermionic $\omega_n = \frac{(2n+1)\pi}{\beta}$ 
   *   - bosonic $\omega_n = \frac{2n\pi}{\beta}$
   *
   *  The mesh is a finite segment of 
   *
   * The mesh can span either only positive frequencies or both positive and negative frequencies (which is necessary for complex functions $G(\tau)$).
   *
   * @figure matsubara_freq_mesh.png: Pictorial representation of ``imfreq({beta, Fermion/Boson, 3, all_frequencies/positive_frequencies_only})``. See :ref:`constructor <imfreq_constructor>` for more details.
   */
  struct imfreq : public tail_fitter_handle {

    using idx_t    = long;
    using datidx_t = long;
    using value_t  = matsubara_freq;

    // -------------------- Data -------------------
    double beta              = 0.0;
    statistic_enum statistic = Fermion;

    /// Option for the constructor. Restrict to positive frequency or not
    enum class option { all_frequencies, positive_frequencies_only };

    private:
    long _n_iw          = 1;
    option _opt         = option::all_frequencies;
    long _last_idx      = _n_iw - 1;
    long _first_idx     = -(_last_idx + ((statistic == Fermion) ? 1 : 0));
    uint64_t mesh_hash_ = 0;

    // -------------------- Constructors -------------------
    public:
    ///
    imfreq() = default;

    /**
     * Construct a Mesh of Matsubara frequencies on a Matsubara domain
     *
     * @param beta Inverse temperature
     * @param statistic Statistic (Fermion or Boson)
     * @param n_iw The number of positive Matsubara frequencies
     * @param option Wether to use all frequencies, or only the positive ones
     */
    imfreq(double beta, statistic_enum statistic, long n_iw = 1025, option opt = option::all_frequencies)
       : beta(beta), statistic(statistic), _n_iw(n_iw), _opt(opt), mesh_hash_(hash(beta, statistic, n_iw, opt)) {
      if (opt == option::positive_frequencies_only) _first_idx = 0;
    }

    /**
     */
    imfreq(double beta, statistic_enum statistic, energy_t omega_max, option opt = option::all_frequencies)
       : imfreq(beta, statistic, long(((omega_max.value * beta / M_PI) - 1) / 2), opt) {}

    /**
     * Construct a Mesh of Matsubara frequencies on a Matsubara domain
     *
     * @param dom Matsubara domain
     * @param n_iw Number of positive Matsubara frequencies
     * @param option Wether to use all frequencies, or only the positive ones
     */
    [[deprecated("matsubara_freq_domain is deprecated")]] imfreq(matsubara_freq_domain dom, long n_iw = 1025, option opt = option::all_frequencies)
       : imfreq(dom.beta, dom.statistic, n_iw, opt) {}

    /**
     * @param dom
     * @param omega_max
     */
    [[deprecated("matsubara_freq_domain is deprecated")]] imfreq(matsubara_freq_domain dom, energy_t omega_max, option opt = option::all_frequencies)
       : imfreq(dom.beta, dom.statistic, omega_max, opt) {}

    // -------------------- Comparisons -------------------

    ///
    bool operator==(imfreq const &m) const { return (std::tie(beta, statistic, _n_iw, _opt) == std::tie(m.beta, m.statistic, m._n_iw, m._opt)); }

    ///
    bool operator!=(imfreq const &m) const { return !(operator==(m)); }

    // -------------------- Get the grid for positive freq only -------------------

    imfreq get_positive_freq() const { return {beta, statistic, _n_iw, option::positive_frequencies_only}; }

    // -------------------- tail -------------------

    /// From an index of a point in the mesh, returns the corresponding point in the domain
    matsubara_freq idx_to_freq(idx_t idx) const { return {idx, beta, statistic}; }

    /// Maximum freq of the mesh
    dcomplex w_max() const { return idx_to_freq(_last_idx); }

    // -------------------- mesh_point -------------------

    /// Type of the mesh point
    struct mesh_point_t : public matsubara_freq {
      using mesh_t       = imfreq;
      const long idx     = n;
      long datidx        = 0;
      uint64_t mesh_hash = 0;
      [[nodiscard]] matsubara_freq const &value() const { return *this; }

      mesh_point_t() = default;
      mesh_point_t(double beta, statistic_enum statistic, idx_t idx, long datidx_, uint64_t mesh_hash_)
         : matsubara_freq(idx, beta, statistic), datidx(datidx_), mesh_hash(mesh_hash_) {}
    };

    // -------------------- Accessors -------------------

    [[nodiscard]] size_t mesh_hash() const noexcept { return mesh_hash_; }

    /// The associated domain
    [[deprecated("matsubara_freq_domain is deprecated")]] [[nodiscard]] matsubara_freq_domain domain() const noexcept { return {beta, statistic}; }

    /// The total number of points in the mesh
    [[nodiscard]] long size() const noexcept { return _last_idx - _first_idx + 1; }

    /// first Matsubara idx
    long first_idx() const { return _first_idx; }

    /// last Matsubara idx
    long last_idx() const { return _last_idx; }

    /// Is the mesh only for positive omega_n (G(tau) real))
    bool positive_only() const { return _opt == option::positive_frequencies_only; }

    // -------------------- index checks and conversions -------------------

    [[nodiscard]] bool is_idx_valid(idx_t idx) const { return first_idx() <= idx and idx <= last_idx(); }

    /// Index to linear index
    [[nodiscard]] datidx_t to_datidx(idx_t idx) const noexcept { return idx - first_idx(); }

    [[nodiscard]] datidx_t to_datidx(matsubara_freq const &iw) const noexcept {
      EXPECTS(beta == iw.beta and statistic == iw.statistic);
      return to_datidx(iw.n);
    }

    [[nodiscard]] datidx_t to_datidx(mesh_point_t const &mp) const noexcept {
      EXPECTS(mesh_hash_ == mp.mesh_hash);
      return mp.datidx;
    }

    [[nodiscard]] datidx_t to_datidx(closest_mesh_point_t<value_t> const &cmp) const {
      EXPECTS(beta == cmp.value.beta and statistic == cmp.value.statistic);
      return to_datidx(to_idx(cmp));
    }

    /// Index from a linear index
    [[nodiscard]] idx_t to_idx(datidx_t datidx) const { return datidx + first_idx(); }

    [[nodiscard]] idx_t to_idx(closest_mesh_point_t<value_t> const &cmp) const {
      EXPECTS(is_idx_valid(cmp.value.n));
      return cmp.value.n;
    }

    [[nodiscard]] mesh_point_t operator[](long datidx) const {
      auto idx = to_idx(datidx);
      EXPECTS(is_idx_valid(idx));
      return {beta, statistic, idx, datidx, mesh_hash_};
    }
    [[nodiscard]] mesh_point_t operator[](closest_mesh_point_t<value_t> const &cmp) const { return (*this)[this->to_datidx(cmp)]; }

    [[nodiscard]] mesh_point_t operator()(long idx) const {
      EXPECTS(is_idx_valid(idx));
      auto datidx = to_datidx(idx);
      return {beta, statistic, idx, datidx, mesh_hash_};
    }

    /// From an index to a matsubara_freq
    [[nodiscard]] matsubara_freq to_value(idx_t idx) const {
      EXPECTS(is_idx_valid(idx));
      return {idx, beta, statistic};
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

    friend std::ostream &operator<<(std::ostream &sout, imfreq const &m) {
      auto stat_cstr = (m.statistic == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("Imaginary Freq Mesh with beta = {}, statistic = {}, n_iw = {}, positive_only = {}", m.beta, stat_cstr, m.size(),
                                 m.positive_only());
    }

    // -------------------- HDF5 -------------------

    [[nodiscard]] static std::string hdf5_format() { return "MeshImFreq"; }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string subgroup_name, imfreq const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, m);

      h5::write(gr, "beta", m.beta);
      h5::write(gr, "statistic", (m.statistic == Fermion ? "F" : "B"));
      h5::write(gr, "size", m.size());
      h5::write(gr, "positive_freq_only", (m.positive_only() ? 1 : 0));
    }

    /// Read from HDF5
    friend void h5_read(h5::group fg, std::string subgroup_name, imfreq &m) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format(gr, m, true);

      int pos_freq = 0;
      if (gr.has_key("positive_freq_only")) h5_read(gr, "positive_freq_only", pos_freq);
      if (gr.has_key("start_at_0")) h5_read(gr, "start_at_0", pos_freq); // backward compatibility only
      auto opt = (pos_freq == 1 ? option::positive_frequencies_only : option::all_frequencies);

      long L    = h5_read<long>(gr, "size");
      long n_iw = (pos_freq ? L : (L + 1) / 2); // positive freq, size is correct, otherwise divide by 2 (euclidian, ok for bosons).

      if (gr.has_key("domain")) { gr = gr.open_group("domain"); } // Backward Compat
      double beta    = h5::read<double>(gr, "beta");
      auto statistic = (h5::read<std::string>(gr, "statistic") == "F" ? Fermion : Boson);

      m = imfreq{beta, statistic, n_iw, opt};
    }

    // -------------------------- evaluation --------------------------

    friend decltype(auto) evaluate(imfreq const &m, auto const &f, matsubara_freq const &iw) {
      EXPECTS(m.beta == iw.beta and m.statistic == iw.statistic);
      return f(iw.n);
    }

    bool eval_to_zero(imfreq::idx_t idx) const { return !is_idx_valid(idx); }
    bool eval_to_zero(matsubara_freq iw) const { return eval_to_zero(iw.n); }
    bool eval_to_zero(imfreq::mesh_point_t mp) const { return eval_to_zero(mp.value()); }
  };

  static_assert(MeshWithValues<imfreq>);

} // namespace triqs::mesh
