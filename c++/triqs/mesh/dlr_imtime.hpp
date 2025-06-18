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
 * @brief Provides a mesh type for the discrete Lehmann representation in imaginary time.
 */

#pragma once

#include "./mesh_iterator.hpp"
#include "./utils.hpp"
#include "./dlr.hpp"

#include <cppdlr/cppdlr.hpp>
#include <h5/h5.hpp>
#include <fmt/format.h>
#include <itertools/itertools.hpp>
#include <nda/nda.hpp>

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

namespace triqs::mesh {

  /**
   * @addtogroup triqs-meshes-imag
   * @{
   */

  /**
   * @brief Imaginary time discrete Lehmann representation (DLR) mesh type.
   *
   * @details An imaginary time DLR mesh satisfies the triqs::mesh::MeshWithValues concept and is defined by the inverse
   * temperature \f$ \beta > 0 \f$, the particle statistics (triqs::mesh::statistic_enum), a DLR energy cutoff \f$
   * \omega_{\text{max}} \f$, an error tolerance \f$ \epsilon \f$ and a boolean flag specifying if the mesh should be
   * symmetric around \f$ \tau = \beta / 2 \f$ (a symmetric mesh enforces the DLR rank to be even for Fermions and odd
   * for Bosons).
   *
   * An imaginary time DLR mesh has the following properties:
   *
   * - Each mesh point is identified by a unique index \f$ l \in \{0, 1, \ldots, N-1\} \f$.
   * - The size of the mesh \f$ N \f$ depends on \f$ \beta \f$ and the choice of \f$ \omega_{\text{max}} \f$ and \f$ 
   * \epsilon \f$. It is equal to the DLR rank \f$ r \f$ and the number of DLR basis functions \f$ K(\tau, \omega_l) 
   * \f$.
   * - An index \f$ l \f$ is mapped to the corresponding data index \f$ d \f$ by the identity function \f$ d(l) = l \f$
   * and vice versa.
   * - An index \f$ l \f$ is mapped to the corresponding value \f$ \tau_l \f$, where \f$ \tau_l \f$ is the l<sup>th
   * </sup> DLR interpolation node in imaginary time space.
   * 
   * @ref triqs-gfs containers that are based on an imaginary time DLR mesh store the function values at the discrete 
   * time points \f$ \tau_l \f$, i.e. \f$ f_l = f(\tau_l) \f$. In contrast to triqs::mesh::dlr and triqs::mesh::imtime, 
   * the GF container cannot evaluate the function at an arbitrary imaginary time \f$ \tau \in [0, \beta] \f$ (see the
   * deleted triqs::mesh::evaluate(dlr_imtime const &, ...)).
   *
   * @code
   * #include <fmt/base.h>
   * #include <triqs/mesh.hpp>
   * 
   * int main() {
   *   // initialize a fermionic imaginary time DLR mesh with beta = 10, omega_max = 0.5 and epsilon = 1e-6
   *   triqs::mesh::dlr_imtime m{10, triqs::mesh::Fermion, 0.5, 1e-6};
   * 
   *   // loop over all mesh points and print their index, data index and value
   *   for (int i = 0; auto mp : m) {
   *     fmt::println("mesh point #{}: index = {}, data index = {}, value = {}", i++, mp.index(), mp.data_index(), mp.value());
   *   }
   * }
   * @endcode
   *
   * Output:
   *
   * ```
   * mesh point #0: index = 0, data index = 0, value = 0.012031950007446723
   * mesh point #1: index = 1, data index = 1, value = 0.6496895210536141
   * mesh point #2: index = 2, data index = 2, value = 2.0222028313159592
   * mesh point #3: index = 3, data index = 3, value = 5.283961182488998
   * mesh point #4: index = 4, data index = 4, value = 7.022202831315959
   * mesh point #5: index = 5, data index = 5, value = 8.584483769065113
   * mesh point #6: index = 6, data index = 6, value = 9.550004964934757
   * mesh point #7: index = 7, data index = 7, value = 9.987968049992553
   * ```
   */
  class dlr_imtime {
    public:
    /// Value type.
    using value_t = double;

    /// Index type.
    using index_t = long;

    /// Data index type.
    using data_index_t = long;

    /**
     * @brief %Mesh point of a triqs::mesh::dlr_imtime mesh.
     * 
     * @details It stores the index \f$ l \f$, the data index \f$ d \f$, the hash value of the parent mesh and the value
     * \f$ \tau_l \f$ of the mesh point.
     */
    class mesh_point_t {
      public:
      /// Parent mesh type.
      using mesh_t = dlr;

      /// Default constructor leaves the mesh point uninitialized.
      mesh_point_t() = default;

      /**
       * @brief Construct a mesh point with a given index \f$ l \f$, data index \f$ d \f$, hash value of the parent mesh
       * and value \f$ \tau_l \f$.
       *
       * @param l Index \f$ l \f$ of the mesh point.
       * @param d Data index \f$ d \f$ of the mesh point.
       * @param mhash Hash value of the parent mesh.
       * @param tau_l Value \f$ \tau_l \f$ of the mesh point.
       */
      mesh_point_t(long l, long d, uint64_t mhash, double tau_l) : index_(l), data_index_(d), mesh_hash_(mhash), value_(tau_l) {}

      /// Get the index \f$ l \f$ of the mesh point.
      [[nodiscard]] long index() const { return index_; }

      /// Get the data index \f$ d \f$ of the mesh point.
      [[nodiscard]] long data_index() const { return data_index_; }

      /// Get the value \f$ \tau_l \f$ of the mesh point.
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
    // Construct an imaginary time DLR mesh with a given set of DLR frequencies.
    dlr_imtime(double b, statistic_enum stat, double wmax, double epsilon, bool sym, nda::vector<double> const &dlr_freq)
       : dlr_imtime(b, stat, wmax, epsilon, sym,
                    detail::dlr_ops{.freq = dlr_freq,
                                    .imt  = {wmax * b, dlr_freq, sym},
                                    .imf  = {wmax * b, dlr_freq, static_cast<cppdlr::statistic_t>(stat), sym}}) {}

    // Construct an imaginary time DLR mesh with given DLR operations.
    dlr_imtime(double b, statistic_enum stat, double wmax, double epsilon, bool sym, detail::dlr_ops ops)
       : beta_(b),
         stat_(stat),
         w_max_(wmax),
         eps_(epsilon),
         symmetrize_(sym),
         mesh_hash_(hash(b, stat, wmax, epsilon, sym, nda::sum(ops.imt.get_itnodes()))),
         dlr_{std::make_shared<detail::dlr_ops>(std::move(ops))} {}

    public:
    /// Default constructor constructs an empty mesh.
    dlr_imtime() = default;

    /**
     * @brief Construct an imaginary time DLR mesh with a given energy cutoff \f$ \omega_{\text{max}} \f$ and error
     * tolerance \f$ \epsilon \f$.
     * 
     * @details It calls `cppdlr::build_dlr_rf` with \f$ \Lambda = \omega_{\text{max}} \beta \f$ and \f$ \epsilon \f$ to
     * build the DLR frequencies \f$ \omega_l \f$, which are then passed to the constructors of `cppdlr::imtime_ops` and 
     * `cppdlr::imfreq_ops` objects.
     *
     * @param b Inverse temperature \f$ \beta > 0 \f$.
     * @param stat Particle statistics.
     * @param wmax DLR energy cutoff \f$ \omega_{\text{max}} = \Lambda / \beta \f$.
     * @param epsilon Error tolerance \f$ \epsilon \f$.
     * @param sym Whether to choose the imaginary time points symmetrically around \f$ \tau = \beta / 2 \f$.
     */
    dlr_imtime(double b, statistic_enum stat, double wmax, double epsilon, bool sym = false)
       : dlr_imtime(b, stat, wmax, epsilon, sym, cppdlr::build_dlr_rf(wmax * b, epsilon, sym)) {}

    /**
     * @brief Construct an imaginary frequency DLR mesh from another DLR type mesh.
     *
     * @tparam M triqs::mesh::dlr or triqs::mesh::dlr_imfreq type.
     * @param m Other mesh.
     */
    template <nda::AnyOf<dlr_imfreq, dlr> M>
    explicit dlr_imtime(M const &m)
       : beta_(m.beta_),
         stat_(m.stat_),
         w_max_(m.w_max_),
         eps_(m.eps_),
         symmetrize_(m.symmetrize_),
         mesh_hash_(hash(beta_, stat_, w_max_, eps_, symmetrize_, nda::sum(m.dlr_->imt.get_itnodes()))),
         dlr_(m.dlr_) {}

    /// Equal-to comparison operator compares the hash values.
    bool operator==(dlr_imtime const &m) const { return mesh_hash_ == m.mesh_hash_ and stat_ == m.stat_; }

    /// Not-equal-to comparison operator compares the hash values.
    bool operator!=(dlr_imtime const &m) const { return !(operator==(m)); }

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

    /// Mapping of a value \f$ \tau \in [0, \beta] \f$ to the data index of the closest mesh point is deleted.
    [[nodiscard]] long to_data_index(closest_mesh_point_t<double> const &cmp) const = delete;

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
     * the imaginary time node \f$ \tau_l \f$ as its value.
     */
    [[nodiscard]] mesh_point_t operator[](long d) const { return (*this)(d); }

    /**
     * @brief Function call operator to access a mesh point by its index \f$ l \in \{0, 1, \ldots, N-1\} \f$.
     *
     * @param l Index \f$ l \f$ of the mesh point.
     * @return mesh_point_t with the index \f$ l \f$, data index \f$ d(l) = l \f$, hash value of the current mesh and
     * the imaginary time node \f$ \tau_l \f$ as its value.
     */
    [[nodiscard]] mesh_point_t operator()(long l) const {
      EXPECTS(is_index_valid(l));
      return {l, l, mesh_hash_, to_value(l)};
    }

    /**
     * @brief Map an index \f$ l \in \{0, 1, \ldots, N-1\} \f$ to its corresponding value \f$ \tau_l \f$.
     *
     * @param l Index \f$ l \f$ to map.
     * @return Value of the l<sup>th</sup> DLR interpolation node in imaginary time space, i.e. \f$ \tau_l \f$ .
     */
    [[nodiscard]] double to_value(long l) const noexcept {
      EXPECTS(is_index_valid(l));
      auto res = dlr_->imt.get_itnodes()[l] * beta_;
      if (res < 0) res = beta_ + res;
      return res;
    }

    /// Get the inverse temperature \f$ \beta \f$.
    [[nodiscard]] double beta() const noexcept { return beta_; }

    /// Get the particle statistics.
    [[nodiscard]] statistic_enum statistic() const noexcept { return stat_; }

    /// Get the DLR energy cutoff \f$ \omega_{\text{max}} = \Lambda / \beta \f$.
    [[nodiscard]] double w_max() const noexcept { return w_max_; }

    /// Get the DLR error tolerance \f$ \epsilon \f$.
    [[nodiscard]] double eps() const noexcept { return eps_; }

    /// Is the mesh symmetric around \f$ \tau = \beta / 2 \f$?
    [[nodiscard]] bool symmetrize() const noexcept { return symmetrize_; }

    /// Get the `nda::vector` of DLR frequencies \f$ \omega_l \f$.
    [[nodiscard]] auto const &dlr_freq() const { return dlr_->freq; }

    /// Get the imaginary time DLR operations object (see also `cppdlr::imtime_ops`).
    [[nodiscard]] auto const &dlr_it() const { return dlr_->imt; }

    /// Get the Matsubara frequency DLR operations object (see also `cppdlr::imfreq_ops`).
    [[nodiscard]] auto const &dlr_if() const { return dlr_->imf; }

    /// Get the hash value of the mesh.
    [[nodiscard]] uint64_t mesh_hash() const noexcept { return mesh_hash_; }

    /// Get the size \f$ N \f$ of the mesh, i.e. the DLR rank \f$ r \f$.
    [[nodiscard]] long size() const noexcept { return (dlr_ ? dlr_->imt.get_itnodes().size() : 0); }

    /// Get an iterator to the beginning of the mesh.
    [[nodiscard]] auto begin() const { return mesh_iterator<dlr_imtime>{.mesh_ptr = this, .data_index = 0}; }

    /// Get a const iterator to the beginning of the mesh.
    [[nodiscard]] auto cbegin() const { return begin(); }

    /// Get an iterator to the end of the mesh.
    [[nodiscard]] auto end() const { return mesh_iterator<dlr_imtime>{.mesh_ptr = this, .data_index = size()}; }

    /// Get a const iterator to the end of the mesh.
    [[nodiscard]] auto cend() const { return end(); }

    /**
     * @brief Write a triqs::mesh::dlr_imtime mesh to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param m %Mesh to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, dlr_imtime const &m) {
      auto stat_cstr = (m.stat_ == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("DLR imaginary time mesh of size {} with beta = {}, statistics = {}, w_max = {}, eps = {}", m.size(), m.beta_,
                                 stat_cstr, m.w_max_, m.eps_);
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
    [[nodiscard]] static std::string hdf5_format() { return "MeshDLRImTime"; }

    /**
     * @brief Write a triqs::mesh::dlr_imtime mesh to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, dlr_imtime const &m) {
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
     * @brief Read a triqs::mesh::dlr_imtime mesh from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, dlr_imtime &m) {
      h5::group gr = g.open_group(name);
      h5::assert_hdf5_format(gr, m, true);
      auto b       = h5::read<double>(gr, "beta");
      auto stat    = (h5::read<std::string>(gr, "statistic") == "F" ? Fermion : Boson);
      auto wmax    = h5::read<double>(gr, "w_max");
      auto epsilon = h5::read<double>(gr, "eps");
      bool sym     = false;
      h5::try_read(gr, "symmetrize", sym);
      auto freq = h5::read<nda::vector<double>>(gr, "dlr_freq");
      auto imt  = h5::read<cppdlr::imtime_ops>(gr, "dlr_it");
      auto imf  = h5::read<cppdlr::imfreq_ops>(gr, "dlr_if");
      m         = dlr_imtime{b, stat, wmax, epsilon, sym, {.freq = freq, .imt = imt, .imf = imf}};
    }

    // Friend declarations.
    friend class dlr_imfreq;
    friend class dlr;

    private:
    double beta_                                = 1.0;
    statistic_enum stat_                        = Fermion;
    double w_max_                               = 0.0;
    double eps_                                 = 1e-10;
    bool symmetrize_                            = false;
    uint64_t mesh_hash_                         = 0;
    std::shared_ptr<const detail::dlr_ops> dlr_ = {};
  };

  /**
   * @brief Evaluating a function \f$ f \f$ at a given imaginary time point \f$ \tau \in [0, \beta] \f$ is deleted for
   * triqs::mesh::dlr_imtime meshes.
   */
  double evaluate(dlr_imtime const &m, ...) = delete;

  /** @} */

  // Check mesh concepts.
  static_assert(Mesh<dlr_imtime>);
  static_assert(MeshWithValues<dlr_imtime>);

} // namespace triqs::mesh
