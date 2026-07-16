// Copyright (c) 2023 Simons Foundation
// Copyright (c) 2023 Hugo U.R. Strand
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
// Authors: Alexander Hampel, Olivier Parcollet, Hugo U.R. Strand, Nils Wentzell

/**
 * @file
 * @brief Provides a mesh type for the discrete Lehmann representation.
 */

#pragma once

#include "./matsubara_freq.hpp"
#include "./mesh_iterator.hpp"
#include "./utils.hpp"
#include "../utility/macros.hpp"

#include <cppdlr/cppdlr.hpp>
#include <h5/h5.hpp>
#include <fmt/format.h>
#include <itertools/itertools.hpp>
#include <nda/nda.hpp>

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace triqs::mesh {

  namespace detail {

    // Struct that combines the DLR frequencies, DLR imaginary time operations and DLR Matsubara frequency operations.
    struct dlr_ops {
      nda::vector<double> freq;
      cppdlr::imtime_ops imt;
      cppdlr::imfreq_ops imf;
    };

  } // namespace detail

  // Forward declarations.
  class dlr_imtime;
  class dlr_imfreq;

  /**
   * @addtogroup triqs-meshes-func
   * @{
   */

  /**
   * @brief Discrete Lehmann representation (DLR) mesh type.
   *
   * @details A DLR mesh is defined by the inverse temperature \f$ \beta > 0 \f$, the particle statistics, a DLR energy
   * cutoff \f$ \omega_{\text{max}} \f$, an error tolerance \f$ \epsilon \f$ and a boolean flag specifying if the mesh
   * should be symmetric around \f$ \omega = 0 \f$.
   *
   * A DLR mesh has the following properties:
   *
   * - Each mesh point is identified by a unique index \f$ l \in \{0, 1, \ldots, N-1\} \f$.
   * - The size of the mesh \f$ N \f$ depends on \f$ \beta \f$ and the choice of \f$ \omega_{\text{max}} \f$ and
   *   \f$ \epsilon \f$. It is equal to the DLR rank \f$ r \f$ and the number of DLR basis functions
   *   \f$ K(\tau, \omega_l) \f$ or \f$ K(i\omega_n, \omega_l) \f$.
   * - An index \f$ l \f$ is mapped to the corresponding data index \f$ d \f$ by the identity function \f$ d(l) = l \f$
   *   and vice versa.
   * - An index \f$ l \f$ is mapped to the corresponding value \f$ \omega_l \f$, where \f$ \omega_l \f$ is the l-th DLR
   *   frequency.
   *
   * Green's function containers that are based on a DLR mesh store the coefficients \f$ f_l \f$ of the discrete Lehmann
   * representation of a function \f$ f(\tau) \f$ or \f$ f(i\omega_n) \f$. To evaluate the function at an arbitrary
   * imaginary time \f$ \tau \in [0, \beta] \f$ or at a specific Matsubara frequency \f$ i\omega_n \f$, the GF container
   * calculates the DLR approximation of the function via
   * \f[
   *   f(\tau) \approx \sum_{l=0}^{N-1} \frac{e^{-\omega_l \tau}}{1 + e^{-\omega_l \beta}} f_l \; ,
   * \f]
   * or
   * \f[
   *   f(i\omega_n) \approx \sum_{l=0}^{N-1} \frac{1}{i\omega_n + \omega_l} f_l \; .
   * \f]
   */
  class C2PY_RENAME(MeshDLR) dlr {
    public:
    /// Value type.
    using value_t = double;

    /// Index type.
    using index_t = long;

    /// Data index type.
    using data_index_t = long;

    /**
     * @brief %Mesh point of a triqs::mesh::dlr mesh.
     *
     * @details It stores the index \f$ l \f$, the data index \f$ d \f$, the hash value of the parent mesh and the value
     * \f$ \omega_l \f$ of the mesh point.
     */
    class C2PY_IGNORE mesh_point_t {
      public:
      /// Parent mesh type.
      using mesh_t = dlr;

      /// Default constructor leaves the mesh point uninitialized.
      mesh_point_t() = default;

      /**
       * @brief Construct a mesh point with a given index \f$ l \f$, data index \f$ d \f$, hash value of the parent mesh
       * and value \f$ \omega_l \f$.
       *
       * @param l Index \f$ l \f$ of the mesh point.
       * @param d Data index \f$ d \f$ of the mesh point.
       * @param mhash Hash value of the parent mesh.
       * @param w_l Value \f$ \omega_l \f$ of the mesh point.
       */
      mesh_point_t(long l, long d, uint64_t mhash, double w_l) : index_(l), data_index_(d), mesh_hash_(mhash), value_(w_l) {}

      /// Get the index \f$ l \f$ of the mesh point.
      [[nodiscard]] long index() const { return index_; }

      /// Get the data index \f$ d \f$ of the mesh point.
      [[nodiscard]] long data_index() const { return data_index_; }

      /// Get the value \f$ \omega_l \f$ of the mesh point.
      [[nodiscard]] double value() const { return value_; }

      /// Get the hash value of the parent mesh.
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return mesh_hash_; }

      /// Conversion to the value type of the parent mesh.
      operator double() const { return value_; }

      private:
      long index_         = 0;
      long data_index_    = 0;
      uint64_t mesh_hash_ = 0;
      double value_       = {};
    };

    private:
    // Construct a DLR mesh with a given set of DLR frequencies.
    dlr(double beta, statistic_enum statistic, double w_max, double eps, bool symmetrize, nda::vector<double> const &dlr_freq)
       : dlr(beta, statistic, w_max, eps, symmetrize,
             detail::dlr_ops{.freq = dlr_freq,
                             .imt  = {w_max * beta, dlr_freq, symmetrize},
                             .imf  = {w_max * beta, dlr_freq, static_cast<cppdlr::statistic_t>(statistic), symmetrize}}) {}

    // Construct a DLR mesh with given DLR operations.
    dlr(double beta, statistic_enum statistic, double w_max, double eps, bool symmetrize, detail::dlr_ops ops)
       : beta_(beta),
         stat_(statistic),
         w_max_(w_max),
         eps_(eps),
         symmetrize_(symmetrize),
         mesh_hash_(hash(beta, statistic, w_max, eps, symmetrize, hash_bytes(ops.imf.get_ifnodes()), std::string_view{"dlr"})),
         dlr_{std::make_shared<detail::dlr_ops>(std::move(ops))} {}

    public:
    /// Default constructor constructs an empty mesh.
    dlr() = default;

    /**
     * @brief Construct a DLR mesh with a given energy cutoff \f$ \omega_{\text{max}} \f$ and error tolerance \f$
     * \epsilon \f$.
     *
     * @details It builds the DLR frequencies \f$ \omega_l \f$ from \f$ \Lambda = \omega_{\text{max}} \beta \f$ and the
     * error tolerance \f$ \epsilon \f$, then constructs the imaginary-time and imaginary-frequency DLR operator tables
     * from them.
     *
     * @param beta Inverse temperature \f$ \beta > 0 \f$.
     * @param statistic Particle statistics.
     * @param w_max DLR energy cutoff \f$ \omega_{\text{max}} = \Lambda / \beta \f$.
     * @param eps Error tolerance \f$ \epsilon \f$.
     * @param symmetrize Whether to choose the DLR frequencies symmetrically around \f$ \omega = 0 \f$.
     */
    dlr(double beta, statistic_enum statistic, double w_max, double eps, bool symmetrize = true)
       : dlr(beta, statistic, w_max, eps, symmetrize, cppdlr::build_dlr_rf(w_max * beta, eps, symmetrize)) {}

    /**
     * @brief Construct a DLR mesh from another DLR type mesh.
     *
     * @tparam M triqs::mesh::dlr_imtime or triqs::mesh::dlr_imfreq type.
     * @param m Other mesh.
     */
    template <nda::AnyOf<dlr_imtime, dlr_imfreq> M>
    explicit dlr(M const &m)
       : beta_(m.beta_),
         stat_(m.stat_),
         w_max_(m.w_max_),
         eps_(m.eps_),
         symmetrize_(m.symmetrize_),
         mesh_hash_(hash(beta_, stat_, w_max_, eps_, symmetrize_, hash_bytes(m.dlr_->imf.get_ifnodes()), std::string_view{"dlr"})),
         dlr_(m.dlr_) {}

    /// Equal-to comparison operator compares the hash values
    bool operator==(dlr const &m) const { return mesh_hash_ == m.mesh_hash_; }

    /**
     * @brief Check if an index \f$ l \f$ is valid.
     *
     * @param l Index \f$ l \f$ to check.
     * @return True if \f$ 0 \leq l < N \f$, false otherwise.
     */
    [[nodiscard]] bool is_index_valid(long l) const noexcept { return 0 <= l and l < size(); }

    /**
     * @brief Map an index \f$ l \in \{0, 1, \ldots, N-1\} \f$ to its corresponding data index \f$ d(l) \f$.
     *
     * @param l Index \f$ l \f$ to map.
     * @return Data index \f$ d(l) = l \f$.
     */
    [[nodiscard]] long to_data_index(long l) const noexcept {
      EXPECTS(is_index_valid(l));
      return l;
    }

    /**
     * @brief Map a data index \f$ d \in \{0, 1, \ldots, N-1\} \f$ to the corresponding index \f$ l(d) \f$.
     *
     * @param d Data index \f$ d \f$ to map.
     * @return Index \f$ l(d) = d \f$.
     */
    [[nodiscard]] long to_index(long d) const noexcept {
      EXPECTS(is_index_valid(d));
      return d;
    }

    /**
     * @brief Subscript operator to access a mesh point by its data index \f$ d \in \{0, 1, \ldots, N-1\} \f$.
     *
     * @param d Data index \f$ d \f$ of the mesh point.
     * @return mesh_point_t with the index \f$ l(d) = d \f$, data index \f$ d \f$, hash value of the current mesh and
     * the DLR frequency \f$ \omega_l \f$ as its value.
     */
    [[nodiscard]] mesh_point_t operator[](long d) const { return (*this)(d); }

    /**
     * @brief Function call operator to access a mesh point by its index \f$ l \in \{0, 1, \ldots, N-1\} \f$.
     *
     * @param l Index \f$ l \f$ of the mesh point.
     * @return mesh_point_t with the index \f$ l \f$, data index \f$ d(l) = l \f$, hash value of the current mesh and
     * the DLR frequency \f$ \omega_l \f$ as its value.
     */
    [[nodiscard]] mesh_point_t operator()(long l) const { return {l, l, mesh_hash_, to_value(l)}; }

    /**
     * @brief Map an index \f$ l \in \{0, 1, \ldots, N-1\} \f$ to its corresponding value \f$ \omega_l \f$.
     *
     * @param l Index \f$ l \f$ to map.
     * @return Value of the l-th DLR frequency \f$ \omega_l \f$.
     */
    [[nodiscard]] double to_value(long l) const noexcept {
      EXPECTS(is_index_valid(l));
      return (dlr_->freq)[l];
    }

    /// Get the inverse temperature \f$ \beta \f$.
    [[nodiscard]] C2PY_PROPERTY_GET(beta) double beta() const noexcept { return beta_; }

    /// Get the particle statistics.
    [[nodiscard]] C2PY_PROPERTY_GET(statistic) statistic_enum statistic() const noexcept { return stat_; }

    /// Get the DLR energy cutoff \f$ \omega_{\text{max}} = \Lambda / \beta \f$.
    [[nodiscard]] C2PY_PROPERTY_GET(w_max) double w_max() const noexcept { return w_max_; }

    /// Get the DLR error tolerance \f$ \epsilon \f$.
    [[nodiscard]] C2PY_PROPERTY_GET(eps) double eps() const noexcept { return eps_; }

    /// Is the mesh symmetric around \f$ \omega = 0 \f$?
    [[nodiscard]] C2PY_PROPERTY_GET(symmetrize) bool symmetrize() const noexcept { return symmetrize_; }

    /// Get the array of DLR frequencies \f$ \omega_l \f$.
    [[nodiscard]] C2PY_PROPERTY_GET(dlr_freq) auto const &dlr_freq() const { return dlr_->freq; }

    /// Get the imaginary time DLR operations object (see also `cppdlr::imtime_ops`).
    [[nodiscard]] C2PY_IGNORE auto const &dlr_it() const { return dlr_->imt; }

    /// Get the Matsubara frequency DLR operations object (see also `cppdlr::imfreq_ops`).
    [[nodiscard]] C2PY_IGNORE auto const &dlr_if() const { return dlr_->imf; }

    /// Get the hash value of the mesh.
    [[nodiscard]] C2PY_PROPERTY_GET(mesh_hash) uint64_t mesh_hash() const noexcept { return mesh_hash_; }

    /// Get the size \f$ N \f$ of the mesh, i.e. the DLR rank \f$ r \f$.
    [[nodiscard]] long size() const noexcept { return (dlr_ ? dlr_->freq.size() : 0); }

    /// Get an iterator to the beginning of the mesh.
    [[nodiscard]] auto begin() const { return mesh_iterator<dlr>{.mesh_ptr = this, .data_index = 0}; }

    /// Get a const iterator to the beginning of the mesh.
    [[nodiscard]] auto cbegin() const { return begin(); }

    /// Get an iterator to the end of the mesh.
    [[nodiscard]] auto end() const { return mesh_iterator<dlr>{.mesh_ptr = this, .data_index = size()}; }

    /// Get a const iterator to the end of the mesh.
    [[nodiscard]] auto cend() const { return end(); }

    /**
     * @brief Write a triqs::mesh::dlr mesh to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param m %Mesh to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, dlr const &m) {
      auto stat_cstr = (m.stat_ == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("DLR coefficient mesh of size {} with beta = {}, statistics = {}, w_max = {}, eps = {}, symmetrized = {}", m.size(),
                                 m.beta_, stat_cstr, m.w_max_, m.eps_, m.symmetrize_);
    }

    /**
     * @brief Serialize the mesh to a generic archive.
     * @param ar Archive to serialize to.
     */
    void serialize(auto &ar) const {
      EXPECTS(dlr_);
      ar & beta_ & stat_ & w_max_ & eps_ & symmetrize_ & mesh_hash_ & dlr_->freq;
      dlr_->imt.serialize(ar);
      dlr_->imf.serialize(ar);
    }

    /**
     * @brief Deserialize the mesh from a generic archive.
     * @param ar Archive to deserialize from.
     */
    void deserialize(auto &ar) {
      nda::vector<double> freq;
      cppdlr::imtime_ops imt;
      cppdlr::imfreq_ops imf;
      ar & beta_ & stat_ & w_max_ & eps_ & symmetrize_ & mesh_hash_ & freq;
      imt.deserialize(ar);
      imf.deserialize(ar);
      dlr_ = std::make_shared<detail::dlr_ops>(freq, imt, imf);
    }

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "MeshDLR"; }

    /**
     * @brief Write a triqs::mesh::dlr mesh to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, dlr const &m) {
      h5::group gr = g.create_group(name);
      h5::write_hdf5_format(gr, m); // NOLINT (downcasting to base class)
      h5::write(gr, "beta", m.beta_);
      h5::write(gr, "statistic", (m.stat_ == Fermion ? "F" : "B"));
      h5::write(gr, "w_max", m.w_max_);
      h5::write(gr, "eps", m.eps_);
      h5::write(gr, "symmetrize", m.symmetrize_);
      h5::write(gr, "dlr_freq", m.dlr_freq());
      h5::write(gr, "dlr_it", m.dlr_it());
      h5::write(gr, "dlr_if", m.dlr_if());
    }

    /**
     * @brief Read a triqs::mesh::dlr mesh from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, dlr &m) {
      h5::group gr = g.open_group(name);
      h5::assert_hdf5_format(gr, m, true); // NOLINT (downcasting to base class)
      auto b       = h5::read<double>(gr, "beta");
      auto stat    = (h5::read<std::string>(gr, "statistic") == "F" ? Fermion : Boson);
      auto wmax    = h5::read<double>(gr, "w_max");
      auto epsilon = h5::read<double>(gr, "eps");
      bool sym     = false;
      h5::try_read(gr, "symmetrize", sym);
      auto freq = h5::read<nda::vector<double>>(gr, "dlr_freq");
      auto imt  = h5::read<cppdlr::imtime_ops>(gr, "dlr_it");
      auto imf  = h5::read<cppdlr::imfreq_ops>(gr, "dlr_if");
      m         = dlr(b, stat, wmax, epsilon, sym, {.freq = freq, .imt = imt, .imf = imf});
    }

    // Friend declarations.
    friend class dlr_imtime;
    friend class dlr_imfreq;

    private:
    double beta_                                = 1.0;
    statistic_enum stat_                        = Fermion;
    double w_max_                               = 0.0;
    double eps_                                 = 1e-10;
    bool symmetrize_                            = false;
    uint64_t mesh_hash_                         = 0;
    std::shared_ptr<const detail::dlr_ops> dlr_ = {};
  };

  // -------------------- evaluation -------------------

  /**
   * @brief Evaluate the DLR approximation of a function \f$ f \f$ at a given imaginary time point \f$ \tau \in [0,
   * \beta] \f$.
   *
   * @details We calculate
   * \f[
   *   f(\tau) \approx \sum_{l=0}^{N-1} K(\tau, \omega_l) f_l \; ,
   * \f]
   * where \f$ f_l \f$ are the DLR coefficients, \f$ \omega_l \f$ are the DLR frequencies and
   * \f[
   *   K(\tau, \omega_l) = \frac{e^{-\omega_l \tau}}{1 + e^{-\omega_l \beta}} \; ,
   * \f]
   * are the DLR basis functions.
   *
   * @param m triqs::mesh::dlr mesh.
   * @param f Callable object \f$ f \f$ containing the DLR coefficients \f$ f_l \f$.
   * @param tau Imaginary time point \f$ \tau \in [0, \beta] \f$ at which to approximate the function.
   * @return DLR approximation of \f$ f(\tau) \f$.
   */
  auto evaluate(dlr const &m, auto const &f, double tau) {
    EXPECTS(m.size() > 0);
    EXPECTS(tau >= 0 and tau <= m.beta());
    return detail::sum_to_regular(nda::range(m.size()), [&](auto l) { return f(l) * cppdlr::k_it(tau / m.beta(), m.dlr_freq()[l]); });
  }

  /**
   * @brief Evaluate the DLR approximation of a function \f$ f \f$ at a given Matsubara frequency \f$ i\omega_n \f$.
   *
   * @details We calculate
   * \f[
   *   f(i\omega_n) \approx \sum_{l=0}^{N-1} K(i\omega_n, \omega_l) f_l \; ,
   * \f]
   * where \f$ f_l \f$ are the DLR coefficients, \f$ \omega_l \f$ are the DLR frequencies and
   * \f[
   *   K(i\omega_n, \omega_l) = \frac{1}{i\omega_n + \omega_l} \; ,
   * \f]
   * are the Fourier transformed DLR basis functions.
   *
   * @param m triqs::mesh::dlr mesh.
   * @param f Callable object \f$ f \f$ containing the DLR coefficients \f$ f_l \f$.
   * @param iw Matsubara frequency \f$ i\omega_n \f$ at which to approximate the function.
   * @return DLR approximation of \f$ f(i\omega_n) \f$.
   */
  auto evaluate(dlr const &m, auto const &f, matsubara_freq const &iw) {
    EXPECTS(m.size() > 0);
    return detail::sum_to_regular(nda::range(m.size()),
                                  [&](auto l) { return f(l) * cppdlr::k_if(iw.n, m.dlr_freq()[l], (cppdlr::statistic_t)iw.statistic) * m.beta(); });
  }

  /** @} */

} // namespace triqs::mesh
