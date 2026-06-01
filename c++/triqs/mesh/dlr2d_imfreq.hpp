// Copyright (c) 2025 Simons Foundation
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
// Authors: Nils Wentzell

/**
 * @file
 * @brief Provides a mesh type for the 2D discrete Lehmann representation in imaginary frequency space.
 */

#pragma once

#include "./matsubara_freq.hpp"
#include "./mesh_iterator.hpp"
#include "./utils.hpp"
#include "./dlr2d.hpp"

#include <cppdlr/cppdlr.hpp>
#include <cppdlr2d/cppdlr2d.hpp>
#include <h5/h5.hpp>
#include <fmt/format.h>
#include <itertools/itertools.hpp>
#include <nda/nda.hpp>

#include <algorithm>
#include <array>
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
   * @brief Imaginary frequency 2D discrete Lehmann representation (DLR) mesh type.
   *
   * @details A 2D imaginary frequency DLR mesh is designed for representing three-point functions
   * using the discrete Lehmann representation. It stores function values on a 2D grid of
   * Matsubara frequency pairs.
   *
   * A 2D imaginary frequency DLR mesh has the following properties:
   *
   * - Each mesh point is identified by a unique data index \f$ d \in \{0, 1, \ldots, N-1\} \f$.
   * - The size of the mesh \f$ N \f$ depends on the DLR rank and the compression settings.
   * - A data index \f$ d \f$ is mapped to a pair of Matsubara frequency indices \f$ (n_1, n_2) \f$.
   * - A data index \f$ d \f$ is mapped to a pair of Matsubara frequencies \f$ (i\omega_{n_1}, i\omega_{n_2}) \f$.
   *
   * @note The index type is `std::array<long, 2>` representing the pair of Matsubara frequency indices.
   * This is different from the linear data_index.
   *
   * @code
   * #include <fmt/base.h>
   * #include <triqs/mesh.hpp>
   *
   * int main() {
   *   // initialize a 2D DLR imfreq mesh with beta = 10, w_max = 1.0 and eps = 1e-10
   *   triqs::mesh::dlr2d_imfreq m{10, 1.0, 1e-10};
   *
   *   // print mesh information
   *   fmt::println("Mesh size: {}, Rank: {}", m.size(), m.rank());
   * }
   * @endcode
   */
  class C2PY_RENAME(MeshDLR2DImFreq) dlr2d_imfreq {
    public:
    /// Value type: pair of Matsubara frequencies.
    using value_t = std::pair<matsubara_freq, matsubara_freq>;

    /// Index type: pair of Matsubara frequency indices.
    using index_t = std::array<long, 2>;

    /// Data index type: linear index.
    using data_index_t = long;

    /**
     * @brief %Mesh point of a triqs::mesh::dlr2d_imfreq mesh.
     *
     * @details It stores the data index \f$ d \f$, the hash value of the parent mesh,
     * the inverse temperature, and the Matsubara frequency index pair.
     */
    class C2PY_IGNORE mesh_point_t {
      public:
      /// Parent mesh type.
      using mesh_t = dlr2d_imfreq;

      /// Default constructor leaves the mesh point uninitialized.
      mesh_point_t() = default;

      /**
       * @brief Construct a mesh point with given parameters.
       *
       * @param b Inverse temperature \f$ \beta \f$.
       * @param n1 First Matsubara frequency index.
       * @param n2 Second Matsubara frequency index.
       * @param d Data index \f$ d \f$ of the mesh point.
       * @param mhash Hash value of the parent mesh.
       */
      mesh_point_t(double b, long n1, long n2, long d, uint64_t mhash) : data_index_(d), mesh_hash_(mhash), beta_(b), n1_(n1), n2_(n2) {}

      /// Get the index (pair of Matsubara frequency indices) of the mesh point.
      [[nodiscard]] index_t index() const { return {n1_, n2_}; }

      /// Get the data index \f$ d \f$ of the mesh point.
      [[nodiscard]] long data_index() const { return data_index_; }

      /// Get the value (pair of Matsubara frequencies) of the mesh point.
      [[nodiscard]] value_t value() const { return {matsubara_freq(n1_, beta_, Fermion), matsubara_freq(n2_, beta_, Fermion)}; }

      /// Get the hash value of the parent mesh.
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return mesh_hash_; }

      /// Conversion to the value type of the parent mesh.
      operator value_t() const { return value(); }

      /// Access the N-th Matsubara frequency (for structured bindings).
      template <size_t N>
        requires(N < 2)
      [[nodiscard]] matsubara_freq get() const {
        if constexpr (N == 0)
          return matsubara_freq(n1_, beta_, Fermion);
        else
          return matsubara_freq(n2_, beta_, Fermion);
      }

      private:
      long data_index_    = 0;
      uint64_t mesh_hash_ = 0;
      double beta_        = 1.0;
      long n1_            = 0;
      long n2_            = 0;
    };

    private:
    // Construct a 2D DLR imfreq mesh with given DLR operations.
    dlr2d_imfreq(double b, double wmax, double epsilon, channel_enum channel, bool compressgrid, bool compressbasis, detail::dlr2d_ops ops)
       : beta_(b),
         w_max_(wmax),
         eps_(epsilon),
         channel_(channel),
         compressgrid_(compressgrid),
         compressbasis_(compressbasis),
         mesh_hash_(hash(b, wmax, epsilon, channel, compressgrid, compressbasis, nda::sum(ops.dlr2d_if))),
         dlr2d_{std::make_shared<detail::dlr2d_ops>(std::move(ops))} {
      max_n_ = std::max(max_element(dlr2d_->dlr2d_if), -min_element(dlr2d_->dlr2d_if) - 1);
    }

    public:
    /// Default constructor constructs an empty mesh.
    dlr2d_imfreq() = default;

    /**
     * @brief Construct a 2D DLR imfreq mesh with a given energy cutoff \f$ \omega_{\text{max}} \f$ and error tolerance \f$
     * \epsilon \f$.
     *
     * @param b Inverse temperature \f$ \beta > 0 \f$.
     * @param wmax DLR energy cutoff \f$ \omega_{\text{max}} = \Lambda / \beta \f$.
     * @param epsilon Error tolerance \f$ \epsilon \f$.
     * @param channel Channel: PP (particle-particle) or PH (particle-hole).
     * @param compressgrid Whether to compress the imaginary frequency grid (default: false).
     * @param compressbasis Whether to compress the real frequency basis (default: true).
     */
    dlr2d_imfreq(double b, double wmax, double epsilon, channel_enum channel, bool compressgrid = false, bool compressbasis = true)
       : beta_(b), w_max_(wmax), eps_(epsilon), channel_(channel), compressgrid_(compressgrid), compressbasis_(compressbasis) {
      // Build 1D DLR real frequencies
      double lambda  = wmax * b;
      auto dlr_rf    = cppdlr::build_dlr_rf(lambda, epsilon);
      auto [imf, rf] = ::cppdlr2d::build_dlr2d(lambda, epsilon, compressgrid, compressbasis);
      auto cf2if     = ::cppdlr2d::build_cf2if(b, dlr_rf, imf, rf);

      mesh_hash_ = hash(b, wmax, epsilon, channel, compressgrid, compressbasis, nda::sum(imf));
      dlr2d_     = std::make_shared<detail::dlr2d_ops>(detail::dlr2d_ops{.dlr_rf = dlr_rf, .dlr2d_if = imf, .dlr2d_rf = rf, .cf2if = cf2if});
      max_n_     = std::max(max_element(dlr2d_->dlr2d_if), -min_element(dlr2d_->dlr2d_if) - 1);
    }

    /**
     * @brief Construct a 2D DLR imfreq mesh from a 2D DLR coefficient mesh.
     *
     * @param m triqs::mesh::dlr2d mesh.
     */
    explicit dlr2d_imfreq(dlr2d const &m)
       : beta_(m.beta_),
         w_max_(m.w_max_),
         eps_(m.eps_),
         channel_(m.channel_),
         compressgrid_(m.compressgrid_),
         compressbasis_(m.compressbasis_),
         mesh_hash_(hash(beta_, w_max_, eps_, channel_, compressgrid_, compressbasis_, nda::sum(m.dlr2d_->dlr2d_if))),
         dlr2d_(m.dlr2d_) {
      max_n_ = std::max(max_element(dlr2d_->dlr2d_if), -min_element(dlr2d_->dlr2d_if) - 1);
    }

    /// Equal-to comparison operator compares the hash values.
    bool operator==(dlr2d_imfreq const &m) const { return mesh_hash_ == m.mesh_hash_; }

    /**
     * @brief Check if a data index \f$ d \f$ is valid.
     *
     * @param d Data index \f$ d \f$ to check.
     * @return True if \f$ 0 \leq d < N \f$, false otherwise.
     */
    [[nodiscard]] bool is_data_index_valid(long d) const noexcept { return 0 <= d and d < size(); }

    /**
     * @brief Check if an index (pair of Matsubara frequency indices) is valid.
     *
     * @param idx Index (pair of Matsubara frequency indices) to check.
     * @return True if the index is in the mesh, false otherwise.
     */
    [[nodiscard]] bool is_index_valid(index_t idx) const noexcept {
      auto pp_idx = (channel_ == PH) ? index_t{-idx[0] - 1, idx[1]} : idx;
      for (long d = 0; d < size(); ++d) {
        if (dlr2d_->dlr2d_if(d, 0) == pp_idx[0] and dlr2d_->dlr2d_if(d, 1) == pp_idx[1]) { return true; }
      }
      return false;
    }

    /**
     * @brief Map an index (pair of Matsubara frequency indices) to its corresponding data index.
     *
     * @param idx Index (pair of Matsubara frequency indices).
     * @return Data index.
     */
    [[nodiscard]] long to_data_index(index_t idx) const noexcept {
      // For PH channel, convert from PH indices to PP indices for lookup
      auto pp_idx = (channel_ == PH) ? index_t{-idx[0] - 1, idx[1]} : idx;
      for (long d = 0; d < size(); ++d) {
        if (dlr2d_->dlr2d_if(d, 0) == pp_idx[0] and dlr2d_->dlr2d_if(d, 1) == pp_idx[1]) { return d; }
      }
      EXPECTS(false); // Index not found
      return -1;
    }

    /**
     * @brief Map a data index to the corresponding index (pair of Matsubara frequency indices).
     *
     * @param d Data index.
     * @return Index (pair of Matsubara frequency indices).
     */
    [[nodiscard]] index_t to_index(long d) const noexcept {
      EXPECTS(is_data_index_valid(d));
      auto n1 = dlr2d_->dlr2d_if(d, 0);
      auto n2 = dlr2d_->dlr2d_if(d, 1);
      // For PH channel, return PH-transformed first index
      if (channel_ == PH) n1 = -n1 - 1;
      return {n1, n2};
    }

    /**
     * @brief Subscript operator to access a mesh point by its data index.
     *
     * @param d Data index of the mesh point.
     * @return mesh_point_t with the data index, hash value, and Matsubara frequency pair.
     */
    [[nodiscard]] mesh_point_t operator[](long d) const {
      EXPECTS(is_data_index_valid(d));
      auto idx = to_index(d);
      return {beta_, idx[0], idx[1], d, mesh_hash_};
    }

    /**
     * @brief Function call operator to access a mesh point by its index (pair of Matsubara frequency indices).
     *
     * @param idx Index (pair of Matsubara frequency indices).
     * @return mesh_point_t with the data index, hash value, and Matsubara frequency pair.
     */
    [[nodiscard]] mesh_point_t operator()(index_t idx) const {
      auto d = to_data_index(idx);
      return {beta_, idx[0], idx[1], d, mesh_hash_};
    }

    /**
     * @brief Map an index (pair of Matsubara frequency indices) to its corresponding value (pair of Matsubara frequencies).
     *
     * @param idx Index (pair of Matsubara frequency indices) to map.
     * @return Pair of Matsubara frequencies.
     */
    [[nodiscard]] value_t to_value(index_t idx) const noexcept {
      EXPECTS(is_index_valid(idx));
      return {matsubara_freq(idx[0], beta_, Fermion), matsubara_freq(idx[1], beta_, Fermion)};
    }

    /// Get the inverse temperature \f$ \beta \f$.
    [[nodiscard]] C2PY_PROPERTY_GET(beta) double beta() const noexcept { return beta_; }

    /// Get the particle statistics (always fermionic for 2D DLR).
    [[nodiscard]] C2PY_PROPERTY_GET(statistic) statistic_enum statistic() const noexcept { return stat_; }

    /// Get the DLR energy cutoff \f$ \omega_{\text{max}} = \Lambda / \beta \f$.
    [[nodiscard]] C2PY_PROPERTY_GET(w_max) double w_max() const noexcept { return w_max_; }

    /// Get the DLR error tolerance \f$ \epsilon \f$.
    [[nodiscard]] C2PY_PROPERTY_GET(eps) double eps() const noexcept { return eps_; }

    /// Get the channel: PP (particle-particle) or PH (particle-hole).
    [[nodiscard]] C2PY_PROPERTY_GET(channel) channel_enum channel() const noexcept { return channel_; }

    /// Is the imaginary frequency grid compressed?
    [[nodiscard]] C2PY_PROPERTY_GET(compressgrid) bool compressgrid() const noexcept { return compressgrid_; }

    /// Is the real frequency basis compressed?
    [[nodiscard]] C2PY_PROPERTY_GET(compressbasis) bool compressbasis() const noexcept { return compressbasis_; }

    /// Get the maximum absolute Matsubara index across all mesh points.
    [[nodiscard]] C2PY_PROPERTY_GET(max_n) long max_n() const noexcept { return max_n_; }

    /// Get the 1D DLR rank (number of 1D DLR frequencies).
    [[nodiscard]] C2PY_PROPERTY_GET(rank) long rank() const { return (dlr2d_ ? dlr2d_->dlr_rf.size() : 0); }

    /// Get the `nda::vector` of 1D DLR frequencies \f$ \omega_l \f$.
    [[nodiscard]] C2PY_PROPERTY_GET(dlr_rf) auto const &dlr_rf() const { return dlr2d_->dlr_rf; }

    /// Get the 2D DLR imaginary frequency grid indices.
    [[nodiscard]] C2PY_PROPERTY_GET(dlr2d_if) auto const &dlr2d_if() const { return dlr2d_->dlr2d_if; }

    /// Get the 2D DLR real frequency grid indices.
    [[nodiscard]] C2PY_PROPERTY_GET(dlr2d_rf) auto const &dlr2d_rf() const { return dlr2d_->dlr2d_rf; }

    /// Get the coefficients-to-values transformation matrix.
    [[nodiscard]] C2PY_IGNORE auto const &cf2if() const { return dlr2d_->cf2if; }

    /**
     * @brief Get the 2D DLR imaginary frequency grid indices for the particle-hole channel.
     *
     * @return 2D array of Matsubara frequency index pairs for particle-hole channel.
     */
    [[nodiscard]] auto get_dlr2d_if_ph() const { return ::cppdlr2d::get_dlr2d_if_ph(dlr2d_->dlr2d_if); }

    /// Get the hash value of the mesh.
    [[nodiscard]] C2PY_PROPERTY_GET(mesh_hash) uint64_t mesh_hash() const noexcept { return mesh_hash_; }

    /// Get the size \f$ N \f$ of the mesh (number of 2D DLR imfreq points).
    [[nodiscard]] long size() const noexcept { return (dlr2d_ ? dlr2d_->dlr2d_if.shape(0) : 0); }

    /// Get an iterator to the beginning of the mesh.
    [[nodiscard]] auto begin() const { return mesh_iterator<dlr2d_imfreq>{.mesh_ptr = this, .data_index = 0}; }

    /// Get a const iterator to the beginning of the mesh.
    [[nodiscard]] auto cbegin() const { return begin(); }

    /// Get an iterator to the end of the mesh.
    [[nodiscard]] auto end() const { return mesh_iterator<dlr2d_imfreq>{.mesh_ptr = this, .data_index = size()}; }

    /// Get a const iterator to the end of the mesh.
    [[nodiscard]] auto cend() const { return end(); }

    /**
     * @brief Write a triqs::mesh::dlr2d_imfreq mesh to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param m %Mesh to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, dlr2d_imfreq const &m) {
      return sout << fmt::format("2D DLR imaginary frequency mesh of size {} (rank {}) with beta = {}, w_max = {}, eps = {}", m.size(), m.rank(),
                                 m.beta_, m.w_max_, m.eps_);
    }

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "MeshDLR2DImFreq"; }

    /**
     * @brief Write a triqs::mesh::dlr2d_imfreq mesh to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, dlr2d_imfreq const &m) {
      h5::group gr = g.create_group(name);
      h5::write_hdf5_format(gr, m); // NOLINT (downcasting to base class)
      h5::write(gr, "beta", m.beta_);
      h5::write(gr, "w_max", m.w_max_);
      h5::write(gr, "eps", m.eps_);
      h5::write(gr, "channel", (m.channel_ == PP ? "PP" : "PH"));
      h5::write(gr, "compressgrid", m.compressgrid_);
      h5::write(gr, "compressbasis", m.compressbasis_);
      h5::write(gr, "dlr_rf", m.dlr_rf());
      h5::write(gr, "dlr2d_if", m.dlr2d_if());
      h5::write(gr, "dlr2d_rf", m.dlr2d_rf());
    }

    /**
     * @brief Read a triqs::mesh::dlr2d_imfreq mesh from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, dlr2d_imfreq &m) {
      h5::group gr = g.open_group(name);
      h5::assert_hdf5_format(gr, m, true);
      auto b             = h5::read<double>(gr, "beta");
      auto wmax          = h5::read<double>(gr, "w_max");
      auto epsilon       = h5::read<double>(gr, "eps");
      auto channel       = (h5::read<std::string>(gr, "channel") == "PP" ? PP : PH);
      auto compressgrid  = h5::read<bool>(gr, "compressgrid");
      auto compressbasis = h5::read<bool>(gr, "compressbasis");
      auto dlr_rf        = h5::read<nda::vector<double>>(gr, "dlr_rf");
      auto dlr2d_if      = h5::read<nda::array<int, 2>>(gr, "dlr2d_if");
      auto dlr2d_rf      = h5::read<nda::array<int, 2>>(gr, "dlr2d_rf");
      auto cf2if         = ::cppdlr2d::build_cf2if(b, dlr_rf, dlr2d_if, dlr2d_rf);
      m                  = dlr2d_imfreq(b, wmax, epsilon, channel, compressgrid, compressbasis,
                                        {.dlr_rf = dlr_rf, .dlr2d_if = dlr2d_if, .dlr2d_rf = dlr2d_rf, .cf2if = cf2if});
    }

    /**
     * @brief Serialize the mesh to a generic archive.
     * @param ar Archive to serialize to.
     */
    void serialize(auto &ar) const { ar & beta_ & w_max_ & eps_ & channel_ & compressgrid_ & compressbasis_; }

    /**
     * @brief Deserialize the mesh from a generic archive.
     * @param ar Archive to deserialize from.
     */
    void deserialize(auto &ar) {
      ar & beta_ & w_max_ & eps_ & channel_ & compressgrid_ & compressbasis_;
      *this = dlr2d_imfreq(beta_, w_max_, eps_, channel_, compressgrid_, compressbasis_);
    }

    // Friend declarations.
    friend class dlr2d;

    private:
    double beta_                                    = 1.0;
    statistic_enum stat_                            = Fermion;
    double w_max_                                   = 0.0;
    double eps_                                     = 1e-6;
    channel_enum channel_                           = PH;
    bool compressgrid_                              = false;
    bool compressbasis_                             = true;
    uint64_t mesh_hash_                             = 0;
    long max_n_                                     = 0;
    std::shared_ptr<const detail::dlr2d_ops> dlr2d_ = {};
  };

  // Define the conversion constructor for dlr2d from dlr2d_imfreq
  inline dlr2d::dlr2d(dlr2d_imfreq const &m)
     : beta_(m.beta_),
       w_max_(m.w_max_),
       eps_(m.eps_),
       channel_(m.channel_),
       compressgrid_(m.compressgrid_),
       compressbasis_(m.compressbasis_),
       mesh_hash_(hash(beta_, w_max_, eps_, channel_, compressgrid_, compressbasis_, nda::sum(m.dlr2d_->dlr_rf))),
       dlr2d_(m.dlr2d_) {}

  /**
   * @brief Evaluating a function at a given Matsubara frequency pair is deleted for
   * triqs::mesh::dlr2d_imfreq meshes.
   */
  double evaluate(dlr2d_imfreq const &m, ...) = delete;

  /** @} */

} // namespace triqs::mesh

// Structured binding support for dlr2d_imfreq mesh points.
template <> struct std::tuple_size<triqs::mesh::dlr2d_imfreq::mesh_point_t> : std::integral_constant<size_t, 2> {};
template <size_t N> struct std::tuple_element<N, triqs::mesh::dlr2d_imfreq::mesh_point_t> {
  using type = triqs::mesh::matsubara_freq;
};
