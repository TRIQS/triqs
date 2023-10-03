// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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
// Authors: Thomas Ayral, Philipp Dumitrescu, Olivier Parcollet, Nils Wentzell

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
   *  A mesh representing Matsubara frequencies 
   *   - fermionic $\omega_n = \frac{(2n+1)\pi}{\beta}$ 
   *   - bosonic $\omega_n = \frac{2n\pi}{\beta}$
   *
   * The mesh has a finite range of frequencies.
   * It spans by default both positive and negative frequencies (which is necessary for complex functions $G(\tau)$), 
   * but can be restricted to positive frequencies
   *
   */
  struct imfreq : public tail_fitter_handle {

    using index_t      = long;
    using data_index_t = long;
    using value_t      = matsubara_freq;

    // -------------------- Data -------------------

    /// Option for the constructor. Restrict to positive frequency or not
    enum class option { all_frequencies, positive_frequencies_only };

    private:
    double _beta              = 1.0;
    statistic_enum _statistic = Fermion;
    long _n_iw                = 0;
    option _opt               = option::all_frequencies;
    long _last_index          = _n_iw - 1;
    long _first_index         = -(_last_index + ((_statistic == Fermion) ? 1 : 0));
    uint64_t _mesh_hash       = 0;

    // -------------------- Constructors -------------------
    public:
    ///
    imfreq() = default;

    /**
     * @brief Construct a new imfreq object
     *  
     * @param beta Inverse temperature
     * @param statistic Statistic (Fermion or Boson)
     * @param n_iw The number of positive Matsubara frequencies
     * @param opt Whether to use all frequencies, or only the positive ones
     *
     */
    imfreq(double beta, statistic_enum statistic, long n_iw = 1025, option opt = option::all_frequencies)
       : _beta(beta), _statistic(statistic), _n_iw(n_iw), _opt(opt), _mesh_hash(hash(beta, statistic, n_iw, opt)) {
      if (opt == option::positive_frequencies_only) _first_index = 0;
    }

    /**
     * @brief Construct a new imfreq object
     * 
     * @param beta Inverse temperature
     * @param statistic Statistic (Fermion or Boson)
     * @param omega_max Energy window. The number of points is computed to match this energy window. 
     * @param opt Whether to use all frequencies, or only the positive ones
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
    [[deprecated("matsubara_freq_domain is deprecated.")]] imfreq(matsubara_freq_domain dom, long n_iw = 1025, option opt = option::all_frequencies)
       : imfreq(dom.beta, dom.statistic, n_iw, opt) {}

    /**
     * @param dom
     * @param omega_max
     */
    [[deprecated("matsubara_freq_domain is deprecated")]] imfreq(matsubara_freq_domain dom, energy_t omega_max, option opt = option::all_frequencies)
       : imfreq(dom.beta, dom.statistic, omega_max, opt) {}

    // -------------------- Comparisons -------------------

    ///
    bool operator==(imfreq const &m) const { return (std::tie(_beta, _statistic, _n_iw, _opt) == std::tie(m._beta, m._statistic, m._n_iw, m._opt)); }

    ///
    bool operator!=(imfreq const &m) const { return !(operator==(m)); }

    // -------------------- Get the grid for positive freq only -------------------

    imfreq get_positive_freq() const { return {_beta, _statistic, _n_iw, option::positive_frequencies_only}; }

    // -------------------- Convertion to matsubara_freq -------------------

    /// From an index of a point in the mesh, returns the corresponding point in the domain
    matsubara_freq index_to_freq(index_t index) const { return {index, _beta, _statistic}; }

    /// Maximum freq of the mesh
    dcomplex w_max() const { return index_to_freq(_last_index); }

    // -------------------- mesh_point -------------------

    /// Type of the mesh point
    struct mesh_point_t : public matsubara_freq {
      using mesh_t = imfreq;

      private:
      long _data_index    = 0;
      uint64_t _mesh_hash = 0;

      public:
      mesh_point_t() = default;
      mesh_point_t(double beta, statistic_enum statistic, index_t index, long data_index, uint64_t mesh_hash) //NOLINT
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

    /// Number of positive Matsubara frequencies
    [[nodiscard]] long n_iw() const noexcept { return _n_iw; }

    /// The Hash for the mesh configuration
    [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }

    /// The associated domain
    [[deprecated("matsubara_freq_domain is deprecated")]] [[nodiscard]] matsubara_freq_domain domain() const noexcept { return {_beta, _statistic}; }

    /// The total number of points in the mesh
    [[nodiscard]] long size() const noexcept { return _last_index - _first_index + 1; }

    /// first Matsubara index
    [[nodiscard]] long first_index() const { return _first_index; }

    /// last Matsubara index
    [[nodiscard]] long last_index() const { return _last_index; }

    /// Is the mesh only for positive omega_n (G(tau) real))
    [[nodiscard]] bool positive_only() const { return _opt == option::positive_frequencies_only; }

    // -------------------- checks -------------------

    /// Checks that the index is in [first_index(), last_index()]
    [[nodiscard]] bool is_index_valid(index_t index) const { return first_index() <= index and index <= last_index(); }

    // -------------------- to_data_index -------------------

    [[nodiscard]] data_index_t to_data_index(index_t index) const noexcept { return index - first_index(); }

    [[nodiscard]] data_index_t to_data_index(matsubara_freq const &iw) const noexcept {
      EXPECTS(_beta == iw.beta and _statistic == iw.statistic);
      return to_data_index(iw.n);
    }

    [[nodiscard]] data_index_t to_data_index(closest_mesh_point_t<value_t> const &cmp) const {
      EXPECTS(_beta == cmp.value.beta and _statistic == cmp.value.statistic);
      return to_data_index(to_index(cmp));
    }

    // -------------------- to_index -------------------

    [[nodiscard]] index_t to_index(data_index_t data_index) const { return data_index + first_index(); }

    [[nodiscard]] index_t to_index(closest_mesh_point_t<value_t> const &cmp) const {
      EXPECTS(is_index_valid(cmp.value.n));
      return cmp.value.n;
    }

    // -------------------- operator [] () -------------------

    [[nodiscard]] mesh_point_t operator[](long data_index) const {
      auto index = to_index(data_index);
      EXPECTS(is_index_valid(index));
      return {_beta, _statistic, index, data_index, _mesh_hash};
    }

    [[nodiscard]] mesh_point_t operator[](closest_mesh_point_t<value_t> const &cmp) const { return (*this)[this->to_data_index(cmp)]; }

    [[nodiscard]] mesh_point_t operator()(long index) const {
      EXPECTS(is_index_valid(index));
      auto data_index = to_data_index(index);
      return {_beta, _statistic, index, data_index, _mesh_hash};
    }

    // -------------------- to_value ------------------
    /// From an index to a matsubara_freq
    [[nodiscard]] matsubara_freq to_value(index_t index) const {
      EXPECTS(is_index_valid(index));
      return {index, _beta, _statistic};
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
      auto stat_cstr = (m._statistic == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("Imaginary Freq Mesh with beta = {}, statistic = {}, n_iw = {}, positive_only = {}", m._beta, stat_cstr, m._n_iw,
                                 m.positive_only());
    }

    // -------------------- HDF5 -------------------

    [[nodiscard]] static std::string hdf5_format() { return "MeshImFreq"; }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string subgroup_name, imfreq const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, m); //NOLINT

      h5::write(gr, "beta", m._beta);
      h5::write(gr, "statistic", (m._statistic == Fermion ? "F" : "B"));
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
      auto beta      = h5::read<double>(gr, "beta");
      auto statistic = (h5::read<std::string>(gr, "statistic") == "F" ? Fermion : Boson);

      m = imfreq{beta, statistic, n_iw, opt};
    }

    // -------------------------- evaluation --------------------------

    friend auto evaluate(imfreq const &m, auto const &f, matsubara_freq const &iw) {
      EXPECTS(m.beta() == iw.beta and m.statistic() == iw.statistic);
      return f(iw.n);
    }

    bool eval_to_zero(imfreq::index_t index) const { return !is_index_valid(index); }
    bool eval_to_zero(matsubara_freq iw) const { return eval_to_zero(iw.n); }
    bool eval_to_zero(imfreq::mesh_point_t mp) const { return eval_to_zero(mp.value()); }
  };

  static_assert(MeshWithValues<imfreq>);

} // namespace triqs::mesh
