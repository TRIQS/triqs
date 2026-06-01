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
 * @brief Provides a mesh type for the 2D discrete Lehmann representation.
 */

#pragma once

#include "./matsubara_freq.hpp"
#include "./mesh_iterator.hpp"
#include "./utils.hpp"

#include <cppdlr/cppdlr.hpp>
#include <cppdlr2d/cppdlr2d.hpp>
#include <h5/h5.hpp>
#include <fmt/format.h>
#include <itertools/itertools.hpp>
#include <nda/nda.hpp>

#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

namespace triqs::mesh {

  using nda::make_regular;

  namespace detail {

    // Struct that combines the 2D DLR data structures.
    struct dlr2d_ops {
      nda::vector<double> dlr_rf;  // 1D DLR real frequencies
      nda::array<int, 2> dlr2d_if; // 2D imfreq grid indices, shape (N_if, 2)
      nda::array<int, 2> dlr2d_rf; // 2D rf grid indices, shape (N_rf, 3)
      ::cppdlr2d::fmatrix cf2if;   // Coefs-to-values transformation matrix
    };

    // Helper to convert flat DLR2D coefficients to regular + singular format.
    // Returns a pair of (f_reg, f_sing) where:
    // - f_reg is a 3D array of shape [3, r, r] for the three regular term types (0, 1, 2)
    // - f_sing is a 1D array of size r for the singular terms (term_type = 3)
    // dlr2d_rf columns: [term_type, k_index, l_index]
    template <typename F> auto flat_to_reg_sing(long rank, nda::array<int, 2> const &dlr2d_rf, F const &f) {
      auto n_coefs = dlr2d_rf.shape(0);

      using value_t = std::decay_t<decltype(make_regular(f(0)))>;

      // Create arrays - use zeros for scalar types, basic_array for non-trivially copyable types
      nda::array<value_t, 3> f_reg;
      nda::array<value_t, 1> f_sing;
      if constexpr (nda::is_scalar_v<value_t>) {
        f_reg  = nda::zeros<value_t>(3, rank, rank);
        f_sing = nda::zeros<value_t>(rank);
      } else {
        // For array-valued coefficients, default-initialize (arrays will be zero-initialized)
        f_reg  = nda::array<value_t, 3>(3, rank, rank);
        f_sing = nda::array<value_t, 1>(rank);
      }

      for (long i = 0; i < n_coefs; ++i) {
        int term_type = dlr2d_rf(i, 0);
        int k_idx     = dlr2d_rf(i, 1);
        int l_idx     = dlr2d_rf(i, 2);
        auto val      = make_regular(f(i));
        if (term_type < 3) {
          // Regular terms (term_type = 0, 1, 2)
          f_reg(term_type, k_idx, l_idx) = val;
        } else {
          // Singular term (term_type = 3)
          f_sing(k_idx) = val;
        }
      }
      return std::make_pair(std::move(f_reg), std::move(f_sing));
    }

    // Helper to convert regular + singular format back to flat DLR2D coefficients.
    // Inverse of flat_to_reg_sing().
    // dlr2d_rf columns: [term_type, k_index, l_index]
    template <typename Arr3, typename Arr1> auto reg_sing_to_flat(nda::array<int, 2> const &dlr2d_rf, Arr3 const &f_reg, Arr1 const &f_sing) {
      using T      = typename std::decay_t<decltype(f_reg(0, 0, 0))>;
      auto n_coefs = dlr2d_rf.shape(0);
      nda::array<T, 1> flat(n_coefs);
      for (long i = 0; i < n_coefs; ++i) {
        int term_type = dlr2d_rf(i, 0);
        int k_idx     = dlr2d_rf(i, 1);
        int l_idx     = dlr2d_rf(i, 2);
        flat(i)       = (term_type < 3) ? f_reg(term_type, k_idx, l_idx) : f_sing(k_idx);
      }
      return flat;
    }

  } // namespace detail

  // Forward declarations.
  class dlr2d_imfreq;

  /**
   * @addtogroup triqs-meshes-func
   * @{
   */

  /**
   * @brief 2D Discrete Lehmann representation (DLR) coefficient mesh type.
   *
   * @details A 2D DLR mesh is designed for representing three-point functions
   * using the discrete Lehmann representation. It stores the DLR expansion
   * coefficients for two-frequency dependent quantities.
   *
   * A 2D DLR mesh has the following properties:
   *
   * - Each mesh point is identified by a unique linear index \f$ l \in \{0, 1, \ldots, N-1\} \f$.
   * - The size of the mesh \f$ N \f$ depends on the DLR rank and the compression settings.
   * - An index \f$ l \f$ is mapped to the corresponding data index \f$ d \f$ by the identity function \f$ d(l) = l \f$
   * and vice versa.
   * - An index \f$ l \f$ is mapped to a pair of DLR frequencies \f$ (\omega_i, \omega_j) \f$.
   *
   * @code
   * #include <fmt/base.h>
   * #include <triqs/mesh.hpp>
   *
   * int main() {
   *   // initialize a 2D DLR mesh with beta = 10, w_max = 1.0 and eps = 1e-10
   *   triqs::mesh::dlr2d m{10, 1.0, 1e-10};
   *
   *   // print mesh information
   *   fmt::println("Mesh size: {}, Rank: {}", m.size(), m.rank());
   * }
   * @endcode
   */
  class C2PY_RENAME(MeshDLR2D) dlr2d {
    public:
    /// Value type: pair of DLR frequencies.
    using value_t = std::pair<double, double>;

    /// Index type: {term_type, k_index, l_index} where term_type is 0,1,2 for regular terms, 3 for singular.
    using index_t = std::array<long, 3>;

    /// Data index type: linear index into data array.
    using data_index_t = long;

    /**
     * @brief %Mesh point of a triqs::mesh::dlr2d mesh.
     *
     * @details It stores the index (term_type, k, l), the data index, the hash value of the parent mesh and the value
     * (pair of DLR frequencies) of the mesh point.
     */
    class C2PY_IGNORE mesh_point_t {
      public:
      /// Parent mesh type.
      using mesh_t = dlr2d;

      /// Default constructor leaves the mesh point uninitialized.
      mesh_point_t() = default;

      /**
       * @brief Construct a mesh point with a given index, data index, hash value of the parent mesh and value.
       *
       * @param idx Index {term_type, k_index, l_index} of the mesh point.
       * @param d Data index (linear) of the mesh point.
       * @param mhash Hash value of the parent mesh.
       * @param val Value (DLR frequency pair) of the mesh point.
       */
      mesh_point_t(index_t idx, long d, uint64_t mhash, value_t val) : index_(idx), data_index_(d), mesh_hash_(mhash), value_(val) {}

      /// Get the index {term_type, k_index, l_index} of the mesh point.
      [[nodiscard]] index_t index() const { return index_; }

      /// Get the data index (linear) of the mesh point.
      [[nodiscard]] long data_index() const { return data_index_; }

      /// Get the value (DLR frequency pair) of the mesh point.
      [[nodiscard]] value_t value() const { return value_; }

      /// Get the hash value of the parent mesh.
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return mesh_hash_; }

      /// Conversion to the value type of the parent mesh.
      operator value_t() const { return value_; }

      private:
      index_t index_      = {};
      long data_index_    = 0;
      uint64_t mesh_hash_ = 0;
      value_t value_      = {};
    };

    private:
    // Construct a 2D DLR mesh with given DLR operations.
    dlr2d(double b, double wmax, double epsilon, channel_enum channel, bool compressgrid, bool compressbasis, detail::dlr2d_ops ops)
       : beta_(b),
         w_max_(wmax),
         eps_(epsilon),
         channel_(channel),
         compressgrid_(compressgrid),
         compressbasis_(compressbasis),
         mesh_hash_(hash(b, wmax, epsilon, channel, compressgrid, compressbasis, nda::sum(ops.dlr_rf))),
         dlr2d_{std::make_shared<detail::dlr2d_ops>(std::move(ops))} {}

    public:
    /// Default constructor constructs an empty mesh.
    dlr2d() = default;

    /**
     * @brief Construct a 2D DLR mesh with a given energy cutoff \f$ \omega_{\text{max}} \f$ and error tolerance \f$
     * \epsilon \f$.
     *
     * @param b Inverse temperature \f$ \beta > 0 \f$.
     * @param wmax DLR energy cutoff \f$ \omega_{\text{max}} = \Lambda / \beta \f$.
     * @param epsilon Error tolerance \f$ \epsilon \f$.
     * @param channel Channel: PP (particle-particle) or PH (particle-hole).
     * @param compressgrid Whether to compress the imaginary frequency grid (default: false).
     * @param compressbasis Whether to compress the real frequency basis (default: true).
     */
    dlr2d(double b, double wmax, double epsilon, channel_enum channel, bool compressgrid = false, bool compressbasis = true)
       : beta_(b), w_max_(wmax), eps_(epsilon), channel_(channel), compressgrid_(compressgrid), compressbasis_(compressbasis) {
      // Build 1D DLR real frequencies
      double lambda  = wmax * b;
      auto dlr_rf    = cppdlr::build_dlr_rf(lambda, epsilon);
      auto [imf, rf] = ::cppdlr2d::build_dlr2d(lambda, epsilon, compressgrid, compressbasis);
      auto cf2if     = ::cppdlr2d::build_cf2if(b, dlr_rf, imf, rf);

      mesh_hash_ = hash(b, wmax, epsilon, channel, compressgrid, compressbasis, nda::sum(dlr_rf));
      dlr2d_     = std::make_shared<detail::dlr2d_ops>(detail::dlr2d_ops{.dlr_rf = dlr_rf, .dlr2d_if = imf, .dlr2d_rf = rf, .cf2if = cf2if});
    }

    /**
     * @brief Construct a 2D DLR coefficient mesh from a 2D DLR imfreq mesh.
     *
     * @param m triqs::mesh::dlr2d_imfreq mesh.
     */
    explicit dlr2d(dlr2d_imfreq const &m);

    /// Equal-to comparison operator compares the hash values.
    bool operator==(dlr2d const &m) const { return mesh_hash_ == m.mesh_hash_; }

    /**
     * @brief Check if a data index is valid.
     *
     * @param d Data index to check.
     * @return True if \f$ 0 \leq d < N \f$, false otherwise.
     */
    [[nodiscard]] bool is_data_index_valid(long d) const noexcept { return 0 <= d and d < size(); }

    /**
     * @brief Check if an index {term_type, k, l} is valid.
     *
     * @param idx Index to check.
     * @return True if term_type is in [0,3] and k,l are in [0, rank), false otherwise.
     */
    [[nodiscard]] bool is_index_valid(index_t const &idx) const noexcept {
      auto [term, k, l] = idx;
      if (term < 0 or term > 3) return false;
      if (k < 0 or k >= rank()) return false;
      if (l < 0 or l >= rank()) return false;
      // For singular terms (term_type=3), k and l must be equal
      if (term == 3 and k != l) return false;
      return true;
    }

    /**
     * @brief Identity conversion for data index (required by GF framework).
     *
     * @param d Data index.
     * @return The same data index.
     */
    [[nodiscard]] data_index_t to_data_index(data_index_t d) const noexcept { return d; }

    /**
     * @brief Map an index {term_type, k, l} to its corresponding data index.
     *
     * @param idx Index to map.
     * @return Data index (linear index into dlr2d_rf).
     */
    [[nodiscard]] data_index_t to_data_index(index_t const &idx) const {
      EXPECTS(is_index_valid(idx));
      auto [term, k, l] = idx;
      // Search for the matching entry in dlr2d_rf
      for (long d = 0; d < size(); ++d) {
        if (dlr2d_->dlr2d_rf(d, 0) == term and dlr2d_->dlr2d_rf(d, 1) == k and dlr2d_->dlr2d_rf(d, 2) == l) { return d; }
      }
      EXPECTS(false); // Index not found in dlr2d mesh
      return -1;
    }

    /**
     * @brief Map a data index to the corresponding index {term_type, k, l}.
     *
     * @param d Data index to map.
     * @return Index {term_type, k_index, l_index}.
     */
    [[nodiscard]] index_t to_index(data_index_t d) const {
      EXPECTS(is_data_index_valid(d));
      return {dlr2d_->dlr2d_rf(d, 0), dlr2d_->dlr2d_rf(d, 1), dlr2d_->dlr2d_rf(d, 2)};
    }

    /**
     * @brief Subscript operator to access a mesh point by its data index.
     *
     * @param d Data index of the mesh point.
     * @return mesh_point_t with the index, data index, hash value and DLR frequency pair.
     */
    [[nodiscard]] mesh_point_t operator[](data_index_t d) const { return {to_index(d), d, mesh_hash_, to_value(d)}; }

    /**
     * @brief Function call operator to access a mesh point by its index {term_type, k, l}.
     *
     * @param idx Index of the mesh point.
     * @return mesh_point_t with the index, data index, hash value and DLR frequency pair.
     */
    [[nodiscard]] mesh_point_t operator()(index_t const &idx) const {
      auto d = to_data_index(idx);
      return {idx, d, mesh_hash_, to_value(d)};
    }

    /**
     * @brief Map a data index to its corresponding value (DLR frequency pair).
     *
     * @param d Data index to map.
     * @return Pair of DLR frequencies \f$ (\omega_i, \omega_j) \f$.
     */
    [[nodiscard]] value_t to_value(data_index_t d) const noexcept {
      EXPECTS(is_data_index_valid(d));
      // dlr2d_rf has columns: [term_type, k_index, l_index]
      auto i = dlr2d_->dlr2d_rf(d, 1);
      auto j = dlr2d_->dlr2d_rf(d, 2);
      return {dlr2d_->dlr_rf(i), dlr2d_->dlr_rf(j)};
    }

    /**
     * @brief Map an index {term_type, k, l} to its corresponding value (DLR frequency pair).
     *
     * @param idx Index to map.
     * @return Pair of DLR frequencies \f$ (\omega_i, \omega_j) \f$.
     */
    [[nodiscard]] value_t to_value(index_t const &idx) const { return to_value(to_data_index(idx)); }

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

    /// Get the hash value of the mesh.
    [[nodiscard]] C2PY_PROPERTY_GET(mesh_hash) uint64_t mesh_hash() const noexcept { return mesh_hash_; }

    /// Get the size \f$ N \f$ of the mesh (number of 2D DLR coefficient points).
    [[nodiscard]] long size() const noexcept { return (dlr2d_ ? dlr2d_->dlr2d_rf.shape(0) : 0); }

    /// Get an iterator to the beginning of the mesh.
    [[nodiscard]] auto begin() const { return mesh_iterator<dlr2d>{.mesh_ptr = this, .data_index = 0}; }

    /// Get a const iterator to the beginning of the mesh.
    [[nodiscard]] auto cbegin() const { return begin(); }

    /// Get an iterator to the end of the mesh.
    [[nodiscard]] auto end() const { return mesh_iterator<dlr2d>{.mesh_ptr = this, .data_index = size()}; }

    /// Get a const iterator to the end of the mesh.
    [[nodiscard]] auto cend() const { return end(); }

    /**
     * @brief Write a triqs::mesh::dlr2d mesh to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param m %Mesh to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, dlr2d const &m) {
      return sout << fmt::format("2D DLR coefficient mesh of size {} (rank {}) with beta = {}, w_max = {}, eps = {}", m.size(), m.rank(), m.beta_,
                                 m.w_max_, m.eps_);
    }

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "MeshDLR2D"; }

    /**
     * @brief Write a triqs::mesh::dlr2d mesh to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, dlr2d const &m) {
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
     * @brief Read a triqs::mesh::dlr2d mesh from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, dlr2d &m) {
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
      m                  = dlr2d(b, wmax, epsilon, channel, compressgrid, compressbasis,
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
      *this = dlr2d(beta_, w_max_, eps_, channel_, compressgrid_, compressbasis_);
    }

    // Friend declarations.
    friend class dlr2d_imfreq;

    private:
    double beta_                                    = 1.0;
    statistic_enum stat_                            = Fermion;
    double w_max_                                   = 0.0;
    double eps_                                     = 1e-6;
    channel_enum channel_                           = PH;
    bool compressgrid_                              = false;
    bool compressbasis_                             = true;
    uint64_t mesh_hash_                             = 0;
    std::shared_ptr<const detail::dlr2d_ops> dlr2d_ = {};
  };

  /**
   * @brief Evaluate the 2D DLR approximation of a function \f$ f \f$ at a given pair of Matsubara frequency indices.
   *
   * @details We calculate the 2D DLR approximation using the cppdlr2d library.
   *
   * @param m triqs::mesh::dlr2d mesh.
   * @param f_reg Regular part of the DLR coefficients (3D array of shape [3, r, r]).
   * @param f_sing Singular part of the DLR coefficients (1D array of size r).
   * @param m_idx First Matsubara frequency index.
   * @param n_idx Second Matsubara frequency index.
   * @param channel Channel index: 1 for particle-particle, 2 for particle-hole.
   * @return 2D DLR approximation of \f$ f(i\omega_m, i\omega_n) \f$.
   */
  inline auto evaluate(dlr2d const &m, nda::array_const_view<dcomplex, 3> f_reg, nda::array_const_view<dcomplex, 1> f_sing, int m_idx, int n_idx,
                       int channel) {
    EXPECTS(m.size() > 0);
    EXPECTS(channel == 1 or channel == 2);
    return ::cppdlr2d::coefs2eval_if(m.beta(), m.dlr_rf(), f_reg, f_sing, m_idx, n_idx, channel);
  }

  /**
   * @brief Evaluate the 2D DLR approximation of a function \f$ f \f$ at a given pair of Matsubara frequencies.
   *
   * @details This overload accepts a callable that provides access to the flat DLR2D coefficients.
   * The coefficients are internally converted to the regular + singular format required by cppdlr2d.
   *
   * @param m triqs::mesh::dlr2d mesh.
   * @param f Callable object that takes a linear index and returns the coefficient value.
   * @param iw_pair Pair of Matsubara frequencies (iw1, iw2).
   * @return 2D DLR approximation of \f$ f(i\omega_1, i\omega_2) \f$.
   */
  auto evaluate(dlr2d const &m, auto const &f, std::pair<matsubara_freq, matsubara_freq> const &iw_pair) {
    EXPECTS(m.size() > 0);

    int n1 = iw_pair.first.n;
    int n2 = iw_pair.second.n;

    // Deduce the coefficient type from the callable
    using value_t = std::decay_t<decltype(make_regular(f(0)))>;

    if constexpr (nda::is_scalar_v<value_t>) {
      // Scalar case: convert and call cppdlr2d directly
      auto [f_reg, f_sing] = detail::flat_to_reg_sing(m.rank(), m.dlr2d_rf(), f);
      return ::cppdlr2d::coefs2eval_if(m.beta(), m.dlr_rf(), f_reg, f_sing, n1, n2, m.channel());
    } else {
      // Array-valued case: evaluate element-wise
      auto sample           = make_regular(f(0));
      auto shape            = sample.shape();
      constexpr size_t Rank = std::tuple_size_v<std::decay_t<decltype(shape)>>;
      nda::array<dcomplex, Rank> result(shape);

      auto f_reg_scalar    = nda::array<dcomplex, 3>(3, m.rank(), m.rank());
      auto f_sing_scalar   = nda::array<dcomplex, 1>(m.rank());
      auto const &dlr2d_rf = m.dlr2d_rf();

      nda::for_each(shape, [&](auto... is) {
        // Zero out the arrays
        f_reg_scalar()  = 0.0;
        f_sing_scalar() = 0.0;

        // Fill in from flat coefficients
        for (long i = 0; i < m.size(); ++i) {
          int term_type = dlr2d_rf(i, 0);
          int k_idx     = dlr2d_rf(i, 1);
          int l_idx     = dlr2d_rf(i, 2);
          auto val      = make_regular(f(i))(is...);
          if (term_type < 3) {
            f_reg_scalar(term_type, k_idx, l_idx) = val;
          } else {
            f_sing_scalar(k_idx) = val;
          }
        }
        result(is...) = ::cppdlr2d::coefs2eval_if(m.beta(), m.dlr_rf(), f_reg_scalar, f_sing_scalar, n1, n2, m.channel());
      });
      return result;
    }
  }

  /** @} */

} // namespace triqs::mesh
