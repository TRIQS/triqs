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

/**
 * @file
 * @brief Provides a mesh type on the imaginary frequency axis.
 */

#pragma once

#include "./matsubara_freq.hpp"
#include "./mesh_iterator.hpp"
#include "./tail_fitter.hpp"
#include "./utils.hpp"

#include <fmt/format.h>
#include <h5/h5.hpp>
#include <nda/nda.hpp>

#include <complex>
#include <cstdint>
#include <iostream>
#include <numbers>
#include <string>
#include <tuple>

namespace triqs::mesh {

  /**
   * @ingroup triqs-meshes-utils
   * @brief Represents an energy value to distinguish constructors in triqs::mesh::imfreq. 
   */
  struct energy_t {
    /// Energy value.
    double value = 0;

    /// Explicit conversion to double.
    explicit operator double() const { return value; }
  };

  /**
   * @addtogroup triqs-meshes-imag
   * @{
   */

  /**
   * @brief Imaginary frequency mesh type.
   *
   * @details An imaginary frequency mesh satisfies the triqs::mesh::MeshWithValues concept and is defined by the number
   * of positive frequencies \f$ N_{i\omega_n} \geq 0 \f$, the inverse temperature \f$ \beta > 0 \f$, its particle
   * statistics (triqs::mesh::statistic_enum) and an triqs::mesh::imfreq::option (`all_frequencies` vs
   * `positive_frequencies_only`).
   *
   * An imaginary frequency mesh has the following properties:
   *
   * - Each mesh point is identified by a unique index \f$ n \in \{ n_{\text{min}}, n_{\text{min}} + 1, \dots,
   * n_{\text{max}} - 1, n_{\text{max}} \} \f$, where \f$ n_{\text{max}} = N_{i\omega_n} - 1 \f$ and
   * \f[
   *   n_{\text{min}} =
   *   \begin{cases}
   *   -N_{i\omega_n} & \text{if fermionic statistics and all frequencies} \\
   *   -N_{i\omega_n} + 1 & \text{if bosonic statistics and all frequencies} \\
   *   0 & \text{if positive frequencies only}
   *   \end{cases}
   *   \; .
   * \f]
   * - The size of the mesh is \f$ N = n_{\text{max}} - n_{\text{min}} + 1 \f$.
   * - An index \f$ n \f$ is mapped to the corresponding data index \f$ d \f$ by the function \f$ d(n) = n -
   * n_{\text{min}} \f$. The inverse map is \f$ n(d) = d + n_{\text{min}} \f$.
   * - An index \f$ n \f$ is mapped to the corresponding value \f$ i\omega_n \equiv i\omega(n) \f$ by the function
   * \f[
   *   i\omega_n =
   *   \begin{cases}
   *   i\frac{2n \pi}{\beta} & \text{if fermionic statistics} \\
   *   i\frac{(2n + 1) \pi}{\beta} & \text{if bosonic statistics}
   *   \end{cases}
   *   \; .
   * \f]
   * The inverse map is trivially \f$ n(i\omega_n) = n \f$.
   * 
   * @ref triqs-gfs containers that are based on an imaginary frequency mesh store the function values at the discrete 
   * frequency points \f$ i\omega_n \f$, i.e. \f$ f_n = f(i\omega(n)) \f$, and to evaluate the function at a specific 
   * Matsubara frequency \f$ i\omega_n \f$ with \f$ n_{\text{min}} \leq n \leq n_{\text{max}} \f$ the container simply
   * returns the corresponding \f$ f_n \f$ (see triqs::mesh::evaluate(imfreq const &, auto const &, matsubara_freq const 
   * &) for details). 
   *
   * @code
   * #include <fmt/std.h>
   * #include <triqs/mesh.hpp>
   * 
   * int main() {
   *   // initialize a fermionic imaginary frequency mesh with 3 positive frequencies and beta = 10
   *   triqs::mesh::imfreq m{10, triqs::mesh::Fermion, 3};
   * 
   *   // loop over all mesh points and print their index, data index and value
   *   for (int i = 0; auto mp : m) {
   *     fmt::println("mesh point #{}: index = {}, data index = {}, value = {}", i++, mp.index(), mp.data_index(), std::complex<double>(mp.value()));
   *   }
   * }
   * @endcode
   *
   * Output:
   *
   * ```
   * mesh point #0: index = -3, data index = 0, value = -1.5707963267948966i
   * mesh point #1: index = -2, data index = 1, value = -0.9424777960769379i
   * mesh point #2: index = -1, data index = 2, value = -0.3141592653589793i
   * mesh point #3: index = 0, data index = 3, value = 0.3141592653589793i
   * mesh point #4: index = 1, data index = 4, value = 0.9424777960769379i
   * mesh point #5: index = 2, data index = 5, value = 1.5707963267948966i
   * ```
   */
  class imfreq : public tail_fitter_handle {
    public:
    /// Value type.
    using value_t = matsubara_freq;

    /// Index type.
    using index_t = long;

    /// Data index type.
    using data_index_t = long;

    /**
     * @brief %Mesh point of a triqs::mesh::imfreq mesh.
     * 
     * @details It inherits from triqs::mesh::matsubara_freq and in addition to the Matsubara index \f$ n \f$, the
     * inverse temperature \f$ \beta \f$ and the particle statistics, it also stores the data index \f$ d \f$ and the
     * hash value of the parent mesh.
     */
    class mesh_point_t : public matsubara_freq {
      public:
      /// Parent mesh type.
      using mesh_t = imfreq;

      /// Default constructor leaves the mesh point uninitialized.
      mesh_point_t() = default;

      /**
       * @brief Construct a mesh point with the given parameters.
       *
       * @details The index \f$ n \f$, the inverse temperature \f$ \beta \f$ and the particle statistics are forwarded
       * to the constructor of triqs::mesh::matsubara_freq.
       *
       * @param b Inverse temperature \f$ \beta \f$.
       * @param stat Particle statistics.
       * @param n_idx Matsubara index \f$ n \f$ of the mesh point.
       * @param d Data index \f$ d \f$ of the mesh point.
       * @param mhash Hash value of the parent mesh.
       */
      mesh_point_t(double b, statistic_enum stat, index_t n_idx, long d, uint64_t mhash)
         : matsubara_freq(n_idx, b, stat), data_index_(d), mesh_hash_(mhash) {}

      /// Get the Matsubara index \f$ n \f$ of the mesh point.
      [[nodiscard]] long index() const { return n; }

      /// Get the data index \f$ d \f$ of the mesh point.
      [[nodiscard]] long data_index() const { return data_index_; }

      /// Get the corresponding Matsubara frequency \f$ i\omega_n \f$.
      [[nodiscard]] matsubara_freq const &value() const { return *this; }

      /// Get the hash value of the parent mesh.
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return mesh_hash_; }

      private:
      long data_index_    = 0;
      uint64_t mesh_hash_ = 0;
    };

    /**
     * @brief Enum to specify which frequencies should be included in the mesh.
     *
     * @details The following options are supported:
     * - `all_frequencies`: Include negative and positive frequencies.
     * - `positive_frequencies_only`: Include only positive frequencies.
     */
    enum class option { all_frequencies, positive_frequencies_only };

    /// Default constructor constructs an empty mesh.
    imfreq() = default;

    /**
     * @brief Construct an imaginary frequency mesh with \f$ N_{i\omega_n} \geq 0 \f$ positive Matsubara frequencies. 
     *
     * @param b Inverse temperature \f$ \beta > 0 \f$.
     * @param stat Particle statistics.
     * @param N_iw Number of positive Matsubara frequencies, i.e. \f$ N_{i\omega_n} \f$.
     * @param opt Whether to use all or only the positive frequencies.
     */
    imfreq(double b, statistic_enum stat, long N_iw = 1025, option opt = option::all_frequencies)
       : beta_(b), stat_(stat), N_iw_(N_iw), opt_(opt), mesh_hash_(hash(b, stat, N_iw, opt)) {
      EXPECTS(beta_ > 0);
      EXPECTS(N_iw_ >= 0);
      if (opt == option::positive_frequencies_only) first_index_ = 0;
    }

    /**
     * @brief Construct an imaginary frequency mesh with a threshold for the largest positive Matsubara frequency on the
     * mesh.
     *
     * @details The number of positive Matsubara frequencies is related to the threshold \f$ \omega_{\text{max}} \f$ by
     * \f[
     *   N_{i\omega_n} =
     *   \begin{cases}
     *   \left\lfloor \frac{\omega_{\text{max}} \beta / \pi - 1}{2} \right\rfloor  & \text{if fermionic statistics} \\
     *   \left\lfloor \frac{\omega_{\text{max}} \beta / \pi}{2} \right\rfloor  & \text{if bosonic statistics} \\
     *   \end{cases} \; .
     * \f]
     *
     * @param b Inverse temperature \f$ \beta > 0 \f$.
     * @param stat Particle statistics.
     * @param w_max Threshold \f$ \omega_{\text{max}} > 0 \f$ for the largest positive Matsubara frequency.
     * @param opt Whether to use all or only the positive frequencies.
     */
    imfreq(double b, statistic_enum stat, energy_t w_max, option opt = option::all_frequencies)
       : imfreq(b, stat, static_cast<long>((w_max.value * b / std::numbers::pi - (stat == Fermion ? 1 : 0)) / 2) + 1, opt) {}

    /**
     * @brief Equal-to comparison operator compares \f$ \beta \f$, the particle statistics, \f$ N_{i\omega_n} \f$ and
     * whether all or only positive frequencies are in the mesh.
     */
    bool operator==(imfreq const &m) const { return (std::tie(beta_, stat_, N_iw_, opt_) == std::tie(m.beta_, m.stat_, m.N_iw_, m.opt_)); }

    /**
     * @brief Not-equal-to comparison operator compares \f$ \beta \f$, the particle statistics, \f$ N_{i\omega_n} \f$ 
     * and whether all or only positive frequencies are in the mesh.
     */
    bool operator!=(imfreq const &m) const { return !(operator==(m)); }

    /**
     * @brief Check if a Matsubara index \f$ n \f$ is valid.
     *
     * @param n Matsubara index \f$ n \f$ to check.
     * @return True if \f$ n_\text{min} \leq n \leq n_\text{max} \f$, false otherwise.
     */
    [[nodiscard]] bool is_index_valid(index_t n) const { return first_index() <= n and n <= last_index(); }

    /**
     * @brief Map a Matsubara index \f$ n \in \{ n_{\text{min}}, \dots, n_{\text{max}} \} \f$ to its corresponding data
     * index \f$ d(n) \f$.
     *
     * @param n Matsubara index \f$ n \f$ to map.
     * @return Data index \f$ d(n) = n - n_{\text{min}} \f$.
     */
    [[nodiscard]] data_index_t to_data_index(index_t n) const noexcept {
      EXPECTS(is_index_valid(n));
      return n - first_index();
    }

    /**
     * @brief Map a Matsubara frequency \f$ i\omega_n \f$ to its data index \f$ d(i\omega_n) \f$.
     *
     * @param iw triqs::mesh::matsubara_freq to map.
     * @return Data index \f$ d(i\omega_n) = n - n_{\text{min}} \f$.
     */
    [[nodiscard]] data_index_t to_data_index(matsubara_freq const &iw) const noexcept {
      EXPECTS(beta_ == iw.beta and stat_ == iw.statistic);
      return to_data_index(iw.n);
    }

    /**
     * @brief Map a Matsubara frequency \f$ i\omega_n \f$ contained in a triqs::mesh::closest_mesh_point_t to its data 
     * index \f$ d(i\omega_n) \f$.
     *
     * @param cmp triqs::mesh::closest_mesh_point_t containing \f$ i\omega_n \f$.
     * @return Data index \f$ d(i\omega_n) = n - n_{\text{min}} \f$.
     */
    [[nodiscard]] data_index_t to_data_index(closest_mesh_point_t<value_t> const &cmp) const {
      EXPECTS(beta_ == cmp.value.beta and stat_ == cmp.value.statistic);
      return to_data_index(to_index(cmp));
    }

    /**
     * @brief Map a data index \f$ d \in \{0, 1, \ldots, N-1\} \f$ to the corresponding Matsubara index \f$ n(d) \f$.
     *
     * @param d Data index \f$ d \f$ to map.
     * @return Matsubara index \f$ n(d) = d + n_{\text{min}} \f$.
     */
    [[nodiscard]] index_t to_index(data_index_t d) const {
      EXPECTS(0 <= d and d < size());
      return d + first_index();
    }

    /**
     * @brief Map a Matsubara frequency \f$ i\omega_n \f$ contained in a triqs::mesh::closest_mesh_point_t to its 
     * Matsubara index \f$ n(i\omega_n) \f$.
     *
     * @param cmp triqs::mesh::closest_mesh_point_t containing \f$ i\omega_n \f$.
     * @return Matsubara index \f$ n(i\omega_n) = n \f$.
     */
    [[nodiscard]] index_t to_index(closest_mesh_point_t<value_t> const &cmp) const {
      EXPECTS(is_index_valid(cmp.value.n));
      return cmp.value.n;
    }

    /**
     * @brief Subscript operator to access a mesh point by its data index \f$ d \in \{0, 1, \ldots, N-1\} \f$.
     *
     * @param d Data index \f$ d \f$ of the mesh point.
     * @return mesh_point_t with the Matsubara index \f$ n(d) = d + n_{\text{min}} \f$, data index \f$ d \f$ and same
     * \f$ \beta \f$, particle statistics and hash value as the current mesh.
     */
    [[nodiscard]] mesh_point_t operator[](long d) const { return {beta_, stat_, to_index(d), d, mesh_hash_}; }

    /**
     * @brief Subscript operator to access a mesh point by a Matsubara frequency \f$ i\omega_n \f$ contained in a
     * triqs::mesh::closest_mesh_point_t.
     *
     * @param cmp triqs::mesh::closest_mesh_point_t containing \f$ i\omega_n \f$.
     * @return mesh_point_t with the Matsubara index \f$ n(i\omega_n) = n \f$, data index \f$ d(i\omega_n) = n -
     * n_{\text{min}} \f$ and same \f$ \beta \f$, particle statistics and hash value as the current mesh.
     */
    [[nodiscard]] mesh_point_t operator[](closest_mesh_point_t<value_t> const &cmp) const { return (*this)[to_data_index(cmp)]; }

    /**
     * @brief Function call operator to access a mesh point by its Matsubara index \f$ n \in \{ n_{\text{min}}, \dots,
     * n_{\text{max}} \} \f$.
     *
     * @param n Matsubara index \f$ n \f$ of the mesh point.
     * @return mesh_point_t with the Matsubara index \f$ n \f$, data index \f$ d(n) = n - n_{\text{min}} \f$ and same
     * \f$ \beta \f$, particle statistics and hash value as the current mesh.
     */
    [[nodiscard]] mesh_point_t operator()(long n) const { return {beta_, stat_, n, to_data_index(n), mesh_hash_}; }

    /**
     * @brief Map a Matsubara index \f$ n \in \{ n_{\text{min}}, \dots, n_{\text{max}} \} \f$ to its corresponding
     * triqs::mesh::matsubara_freq \f$ i\omega_n \f$.
     *
     * @param n Matsubara index \f$ n \f$ to map.
     * @return Matsubara frequency \f$ i\omega_n \f$.
     */
    [[nodiscard]] matsubara_freq to_value(index_t n) const {
      EXPECTS(is_index_valid(n));
      return {n, beta_, stat_};
    }

    /**
     * @brief Get a new mesh with the same \f$ \beta \f$, particle statistics and \f$ N_{i\omega_n} \f$ but only
     * positive frequencies.
     */
    imfreq get_positive_freq() const { return {beta_, stat_, N_iw_, option::positive_frequencies_only}; }

    /**
     * @brief Map an index \f$ n \f$ to its corresponding Matsubara frequency \f$ i\omega_n \f$.
     *
     * @note This function does not check if the index is valid and it returns a triqs::mesh::matsubara_freq object not
     * a mesh_point_t.
     *
     * @param n Matsubara index \f$ n \f$ to map.
     * @return Matsubara frequency \f$ i\omega_n \f$.
     */
    matsubara_freq index_to_freq(index_t n) const { return {n, beta_, stat_}; }

    /// Get the complex value of the largest positive Matsubara frequency in the mesh.
    std::complex<double> w_max() const { return index_to_freq(last_index_); }

    /// Get the inverse temperature \f$ \beta \f$.
    [[nodiscard]] double beta() const noexcept { return beta_; }

    /// Get the particle statistics.
    [[nodiscard]] statistic_enum statistic() const noexcept { return stat_; }

    /// Get the number of positive Matsubara frequencies \f$ N_{i\omega_n} \f$.
    [[nodiscard]] long n_iw() const noexcept { return N_iw_; }

    /// Get the hash value of the mesh.
    [[nodiscard]] uint64_t mesh_hash() const noexcept { return mesh_hash_; }

    /// Get the size \f$ N \f$ of the mesh, i.e. the total number of mesh points.
    [[nodiscard]] long size() const noexcept { return last_index_ - first_index_ + 1; }

    /// Get the first Matsubara index, i.e. \f$ n_{\text{min}} \f$.
    [[nodiscard]] long first_index() const { return first_index_; }

    /// Get the last Matsubara index, i.e. \f$ n_{\text{max}} \f$.
    [[nodiscard]] long last_index() const { return last_index_; }

    /// Is the mesh restricted to positive Matsubara frequencies?
    [[nodiscard]] bool positive_only() const { return opt_ == option::positive_frequencies_only; }

    /// Get an iterator to the beginning of the mesh.
    [[nodiscard]] auto begin() const { return mesh_iterator<imfreq>{.mesh_ptr = this, .data_index = 0}; }

    /// Get a const iterator to the beginning of the mesh.
    [[nodiscard]] auto cbegin() const { return begin(); }

    /// Get an iterator to the end of the mesh.
    [[nodiscard]] auto end() const { return mesh_iterator<imfreq>{.mesh_ptr = this, .data_index = size()}; }

    /// Get a const iterator to the end of the mesh.
    [[nodiscard]] auto cend() const { return end(); }

    /**
     * @brief Write a triqs::mesh::imfreq mesh to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param m %Mesh to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, imfreq const &m) {
      auto stat_cstr = (m.stat_ == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("Imaginary frequency mesh with beta = {}, statistics = {}, N_iw = {}, positive_only = {}", m.beta_, stat_cstr,
                                 m.N_iw_, m.positive_only());
    }

    /**
     * @brief Serialize the mesh to a generic archive.
     * @param ar Archive to serialize to.
     */
    void serialize(auto &ar) const { ar & beta_ & stat_ & N_iw_ & opt_ & last_index_ & first_index_ & mesh_hash_; }

    /**
     * @brief Deserialize the mesh from a generic archive.
     * @param ar Archive to deserialize from.
     */
    void deserialize(auto &ar) { ar & beta_ & stat_ & N_iw_ & opt_ & last_index_ & first_index_ & mesh_hash_; }

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "MeshImFreq"; }

    /**
     * @brief Write a triqs::mesh::imfreq mesh to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be written.
     */
    friend void h5_write(h5::group g, std::string name, imfreq const &m) {
      h5::group gr = g.create_group(name);
      h5::write_hdf5_format(gr, m); // NOLINT (downcasting to base class)
      h5::write(gr, "beta", m.beta_);
      h5::write(gr, "statistic", (m.stat_ == Fermion ? "F" : "B"));
      h5::write(gr, "size", m.size());
      h5::write(gr, "positive_freq_only", (m.positive_only() ? 1 : 0));
    }

    /**
     * @brief Read a triqs::mesh::imfreq mesh from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be read into.
     */
    friend void h5_read(h5::group g, std::string name, imfreq &m) {
      h5::group gr = g.open_group(name);
      h5::assert_hdf5_format(gr, m, true);

      // enum option: positive_frequencies_only = 1, all_frequencies = 0
      int pos_freq = 0;
      if (gr.has_key("positive_freq_only")) h5::read(gr, "positive_freq_only", pos_freq);
      if (gr.has_key("start_at_0")) h5::read(gr, "start_at_0", pos_freq); // backward compatibility
      auto opt = (pos_freq == 1 ? option::positive_frequencies_only : option::all_frequencies);

      // size N and the number of positive frequencies N_iw
      long N    = h5::read<long>(gr, "size");
      long N_iw = (pos_freq ? N : (N + 1) / 2);

      // Matsubara frequency domain for backward compatibility
      if (gr.has_key("domain")) { gr = gr.open_group("domain"); }

      // beta and statistics
      auto beta = h5::read<double>(gr, "beta");
      auto stat = (h5::read<std::string>(gr, "statistic") == "F" ? Fermion : Boson);

      m = imfreq{beta, stat, N_iw, opt};
    }

    /// Return true if the given Matusbara index \f$ n \f$ is not valid (see is_index_valid()).
    bool eval_to_zero(index_t n) const { return !is_index_valid(n); }

    /// Return true if the Matusbara index of the given Matsubara frequency \f$ i\omega_n \f$ is not valid.
    bool eval_to_zero(matsubara_freq iw) const { return eval_to_zero(iw.n); }

    /// Return true if the Matusbara index of the given mesh point is not valid (see is_index_valid()).
    bool eval_to_zero(mesh_point_t mp) const { return eval_to_zero(mp.value()); }

    private:
    double beta_         = 1.0;
    statistic_enum stat_ = Fermion;
    long N_iw_           = 0;
    option opt_          = option::all_frequencies;
    long last_index_     = N_iw_ - 1;
    long first_index_    = -(last_index_ + ((stat_ == Fermion) ? 1 : 0));
    uint64_t mesh_hash_  = 0;
  };

  /**
   * @brief Evaluate a function \f$ f \f$ defined on a triqs::mesh::imfreq mesh at a given Matsubara frequency \f$
   * i\omega_n\f$.
   *
   * @param m triqs::mesh::imfreq mesh.
   * @param f Callable object \f$ f \f$ containing the function values \f$ f_n = f(i\omega_n) \f$ at the mesh points.
   * @param iw triqs::mesh::matsubara_freq \f$ i\omega_n \f$ at which to evaluate the function.
   * @return Function value \f$ f(i\omega_n) \f$.
   */
  auto evaluate([[maybe_unused]] imfreq const &m, auto const &f, matsubara_freq const &iw) {
    EXPECTS(m.beta() == iw.beta and m.statistic() == iw.statistic);
    return f(iw.n);
  }

  /** @} */

  // Check mesh concepts.
  static_assert(Mesh<imfreq>);
  static_assert(MeshWithValues<imfreq>);

} // namespace triqs::mesh
