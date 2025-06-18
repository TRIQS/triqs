// Copyright (c) 2023 Simons Foundation
// Copyright (c) 2023 Hugo Strand
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
// Authors: Alexander Hampel, Olivier Parcollet, Hugo Strand, Nils Wentzell

/**
 * @file
 * @brief Provides a mesh type for the discrete Lehmann representation in imaginary frequency space.
 */

#pragma once

#include "./matsubara_freq.hpp"
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
   * @brief Imaginary frequency discrete Lehmann representation (DLR) mesh type.
   *
   * @details An imaginary frequency DLR mesh satisfies the triqs::mesh::MeshWithValues concept and is defined by the
   * inverse temperature \f$ \beta > 0 \f$, the particle statistics (triqs::mesh::statistic_enum), a DLR energy cutoff 
   * \f$ \omega_{\text{max}} \f$, an error tolerance \f$ \epsilon \f$ and a boolean flag specifying if the mesh should 
   * be symmetric around \f$ i\omega_n = 0 \f$ (a symmetric mesh enforces the DLR rank to be even for Fermions and odd
   * for Bosons).
   *
   * An imaginary frequency DLR mesh has the following properties:
   *
   * - Each mesh point is identified by a unique index \f$ l \in \{0, 1, \ldots, N-1\} \f$.
   * - The size of the mesh \f$ N \f$ depends on \f$ \beta \f$ and the choice of \f$ \omega_{\text{max}} \f$ and \f$ 
   * \epsilon \f$. It is equal to the DLR rank \f$ r \f$ and the number of DLR basis functions \f$ K(i\omega_n, 
   * \omega_l) \f$.
   * - An index \f$ l \f$ is mapped to the corresponding data index \f$ d \f$ by the identity function \f$ d(l) = l \f$
   * and vice versa.
   * - An index \f$ l \f$ is mapped to the corresponding value \f$ i\omega_{n_l} \f$, where \f$ i\omega_{n_l} \f$ is the
   * l<sup>th</sup> DLR interpolation node in imaginary frequency space.
   * 
   * @note The index \f$ l \f$, which is used to access a certain mesh point, is different from the Matsubara index \f$ 
   * n_l \f$ that is stored in the mesh point object. That means, if `m` is an instance of triqs::mesh::dlr_imfreq, 
   * then `m(l).index() != l` in general.
   * 
   * @ref triqs-gfs containers that are based on an imaginary frequency DLR mesh store the function values at the 
   * discrete frequency points \f$ i\omega_{n_l} \f$, i.e. \f$ f_l = f(i\omega_{n_l}) \f$. In contrast to 
   * triqs::mesh::dlr and triqs::mesh::imfreq, the GF container cannot evaluate the function at an arbitrary Matsubara 
   * frequency \f$ i\omega_n \f$ (see the deleted triqs::mesh::evaluate(dlr_imfreq const &, ...)).
   *
   * @code
   * #include <fmt/std.h>
   * #include <triqs/mesh.hpp>
   * 
   * int main() {
   *   // initialize a fermionic imaginary frequency DLR mesh with beta = 10, omega_max = 0.5 and epsilon = 1e-6
   *   triqs::mesh::dlr_imfreq m{10, triqs::mesh::Fermion, 0.5, 1e-6};
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
   * mesh point #0: index = -5, data index = 0, value = -2.827433388230814i
   * mesh point #1: index = -3, data index = 1, value = -1.5707963267948966i
   * mesh point #2: index = -2, data index = 2, value = -0.9424777960769379i
   * mesh point #3: index = -1, data index = 3, value = -0.3141592653589793i
   * mesh point #4: index = 0, data index = 4, value = 0.3141592653589793i
   * mesh point #5: index = 1, data index = 5, value = 0.9424777960769379i
   * mesh point #6: index = 2, data index = 6, value = 1.5707963267948966i
   * mesh point #7: index = 4, data index = 7, value = 2.827433388230814i
   * ```
   */
  class dlr_imfreq {
    public:
    /// Value type.
    using value_t = matsubara_freq;

    /// Index type.
    using index_t = long;

    /// Data index type.
    using data_index_t = long;

    /**
     * @brief %Mesh point of a triqs::mesh::dlr_imfreq mesh.
     * 
     * @details It inherits from triqs::mesh::matsubara_freq and in addition to the Matsubara index \f$ n_l \f$, the
     * inverse temperature \f$ \beta \f$ and the particle statistics, it also stores the data index \f$ d \f$ and the
     * hash value of the parent mesh.
     */
    class mesh_point_t : public matsubara_freq {
      public:
      /// Parent mesh type.
      using mesh_t = dlr_imfreq;

      /// Default constructor leaves the mesh point uninitialized.
      mesh_point_t() = default;

      /**
       * @brief Construct a mesh point with the given parameters.
       *
       * @details The index \f$ n_l \f$, the inverse temperature \f$ \beta \f$ and the particle statistics are forwarded
       * to the constructor of triqs::mesh::matsubara_freq.
       *
       * @param b Inverse temperature \f$ \beta \f$.
       * @param stat Particle statistics.
       * @param n_l Matsubara index \f$ n_l \f$ of the mesh point.
       * @param d Data index \f$ d \f$ of the mesh point.
       * @param mhash Hash value of the parent mesh.
       */
      mesh_point_t(double b, statistic_enum stat, long n_l, long d, uint64_t mhash)
         : matsubara_freq(n_l, b, stat), _data_index(d), _mesh_hash(mhash) {}

      /// Get the Matsubara index \f$ n_l \f$ of the mesh point.
      [[nodiscard]] long index() const { return n; }

      /// Get the data index \f$ d \f$ of the mesh point.
      [[nodiscard]] long data_index() const { return _data_index; }

      /// Get the corresponding Matsubara frequency \f$ i\omega_{n_l} \f$.
      [[nodiscard]] matsubara_freq const &value() const { return *this; }

      /// Get the hash value of the parent mesh.
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }

      private:
      long _data_index    = 0;
      uint64_t _mesh_hash = 0;
    };

    private:
    // Construct an imaginary frequency DLR mesh with a given set of DLR frequencies.
    dlr_imfreq(double b, statistic_enum stat, double wmax, double epsilon, bool sym, nda::vector<double> const &dlr_freq)
       : dlr_imfreq(b, stat, wmax, epsilon, sym,
                    detail::dlr_ops{.freq = dlr_freq,
                                    .imt  = {wmax * b, dlr_freq, sym},
                                    .imf  = {wmax * b, dlr_freq, static_cast<cppdlr::statistic_t>(stat), sym}}) {}

    // Construct an imaginary frequency DLR mesh with given DLR operations.
    dlr_imfreq(double b, statistic_enum stat, double wmax, double epsilon, bool sym, detail::dlr_ops dlr)
       : _beta(b),
         _statistic(stat),
         _w_max(wmax),
         _eps(epsilon),
         _symmetrize(sym),
         _mesh_hash(hash(b, stat, wmax, epsilon, sym, nda::sum(dlr.imf.get_ifnodes()))),
         _dlr{std::make_shared<detail::dlr_ops>(std::move(dlr))} {}

    public:
    /// Default constructor constructs an empty mesh.
    dlr_imfreq() = default;

    /**
     * @brief Construct an imaginary frequency DLR mesh with a given energy cutoff \f$ \omega_{\text{max}} \f$ and error
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
     * @param sym Whether to choose the Matsubara frequencies symmetrically around \f$ i\omega_n = 0 \f$.
     */
    dlr_imfreq(double b, statistic_enum stat, double wmax, double epsilon, bool sym = false)
       : dlr_imfreq(b, stat, wmax, epsilon, sym, cppdlr::build_dlr_rf(wmax * b, epsilon, sym)) {}

    /**
     * @brief Construct an imaginary frequency DLR mesh from another DLR type mesh.
     *
     * @tparam M triqs::mesh::dlr or triqs::mesh::dlr_imtime type.
     * @param m Other mesh.
     */
    template <nda::AnyOf<dlr_imtime, dlr> M>
    explicit dlr_imfreq(M const &m)
       : _beta(m._beta),
         _statistic(m._statistic),
         _w_max(m._w_max),
         _eps(m._eps),
         _symmetrize(m._symmetrize),
         _mesh_hash(hash(_beta, _statistic, _w_max, _eps, _symmetrize, nda::sum(m._dlr->imf.get_ifnodes()))),
         _dlr(m._dlr) {}

    /// Equal-to comparison operator compares the hash values.
    bool operator==(dlr_imfreq const &m) const { return _mesh_hash == m._mesh_hash; }

    /// Not-equal-to comparison operator compares the hash values.
    bool operator!=(dlr_imfreq const &m) const { return !(operator==(m)); }

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
     * @return mesh_point_t with the Matsubara index \f$ n_{l(d)} \f$, data index \f$ d \f$ and same \f$ \beta \f$,
     * particle statistics and hash value as the current mesh.
     */
    [[nodiscard]] mesh_point_t operator[](long d) const { return (*this)(d); }

    /**
     * @brief Function call operator to access a mesh point by its index \f$ l \in \{0, 1, \ldots, N-1\} \f$.
     *
     * @param l Index \f$ l \f$ of the mesh point.
     * @return mesh_point_t with the Matsubara index \f$ n_l \f$, data index \f$ d(l) = l \f$ and same \f$ \beta \f$,
     * particle statistics and hash value as the current mesh.
     */
    [[nodiscard]] mesh_point_t operator()(long l) const {
      EXPECTS(is_index_valid(l));
      return {_beta, _statistic, _dlr->imf.get_ifnodes()[l], l, _mesh_hash};
    }

    /**
     * @brief Map an index \f$ l \in \{0, 1, \ldots, N-1\} \f$ to its corresponding triqs::mesh::matsubara_freq \f$
     * i\omega_{n_l} \f$.
     *
     * @param l Index \f$ l \f$ to map.
     * @return Matsubara frequency \f$ i\omega_{n_l}\f$.
     */
    [[nodiscard]] matsubara_freq to_value(long l) const noexcept {
      EXPECTS(is_index_valid(l));
      return {_dlr->imf.get_ifnodes()[l], _beta, _statistic};
    }

    /// Get the inverse temperature \f$ \beta \f$.
    [[nodiscard]] double beta() const noexcept { return _beta; }

    /// Get the particle statistics.
    [[nodiscard]] statistic_enum statistic() const noexcept { return _statistic; }

    /// Get the DLR energy cutoff \f$ \omega_{\text{max}} = \Lambda / \beta \f$.
    [[nodiscard]] double w_max() const noexcept { return _w_max; }

    /// Get the DLR error tolerance \f$ \epsilon \f$.
    [[nodiscard]] double eps() const noexcept { return _eps; }

    /// Is the mesh symmetric around \f$ i\omega_n = 0  \f$?
    [[nodiscard]] bool symmetrize() const noexcept { return _symmetrize; }

    /// Get the `nda::vector` of DLR frequencies \f$ \omega_l \f$.
    [[nodiscard]] auto const &dlr_freq() const { return _dlr->freq; }

    /// Get the imaginary time DLR operations object (see also `cppdlr::imtime_ops`).
    [[nodiscard]] auto const &dlr_it() const { return _dlr->imt; }

    /// Get the Matsubara frequency DLR operations object (see also `cppdlr::imfreq_ops`).
    [[nodiscard]] auto const &dlr_if() const { return _dlr->imf; }

    /// Get the hash value of the mesh.
    [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }

    /// Get a `std::pair` containing the smallest and largest Matsubara frequency in the mesh.
    [[nodiscard]] auto min_max_frequencies() const noexcept {
      return std::pair<matsubara_freq, matsubara_freq>{(*this)(0).value(), (*this)(size() - 1).value()};
    }

    /// Get the size \f$ N \f$ of the mesh, i.e. the DLR rank \f$ r \f$.
    [[nodiscard]] long size() const noexcept { return (_dlr ? _dlr->imf.get_ifnodes().size() : 0); }

    /// Get an iterator to the beginning of the mesh.
    [[nodiscard]] auto begin() const { return mesh_iterator<dlr_imfreq>{.mesh_ptr = this, .data_index = 0}; }

    /// Get a const iterator to the beginning of the mesh.
    [[nodiscard]] auto cbegin() const { return begin(); }

    /// Get an iterator to the end of the mesh.
    [[nodiscard]] auto end() const { return mesh_iterator<dlr_imfreq>{.mesh_ptr = this, .data_index = size()}; }

    /// Get a const iterator to the end of the mesh.
    [[nodiscard]] auto cend() const { return end(); }

    /**
     * @brief Write a triqs::mesh::dlr_imfreq mesh to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param m %Mesh to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, dlr_imfreq const &m) {
      auto stat_cstr = (m._statistic == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("DLR imaginary frequency mesh of size {} with beta = {}, statistics = {}, w_max = {}, eps = {}", m.size(), m._beta,
                                 stat_cstr, m._w_max, m._eps);
    }

    /**
     * @brief Serialize the mesh to a generic archive.
     * @param ar Archive to serialize to.
     */
    void serialize(auto &ar) const {
      EXPECTS(_dlr);
      ar & _beta & _statistic & _w_max & _eps & _symmetrize & _mesh_hash & _dlr->freq;
      _dlr->imt.serialize(ar);
      _dlr->imf.serialize(ar);
    }

    /**
     * @brief Deserialize the mesh from a generic archive.
     * @param ar Archive to deserialize from.
     */
    void deserialize(auto &ar) {
      nda::vector<double> freq;
      cppdlr::imtime_ops imt;
      cppdlr::imfreq_ops imf;
      ar & _beta & _statistic & _w_max & _eps & _symmetrize & _mesh_hash & freq;
      imt.deserialize(ar);
      imf.deserialize(ar);
      _dlr = std::make_shared<detail::dlr_ops>(freq, imt, imf);
    }

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "MeshDLRImFreq"; }

    /**
     * @brief Write a triqs::mesh::dlr_imfreq mesh to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, dlr_imfreq const &m) {
      h5::group gr = g.create_group(name);
      h5::write_hdf5_format(gr, m); // NOLINT (downcasting to base class)
      h5::write(gr, "beta", m._beta);
      h5::write(gr, "statistic", (m._statistic == Fermion ? "F" : "B"));
      h5::write(gr, "w_max", m._w_max);
      h5::write(gr, "eps", m._eps);
      h5::write(gr, "symmetrize", m._symmetrize);
      h5::write(gr, "dlr_freq", m.dlr_freq());
      h5::write(gr, "dlr_it", m.dlr_it());
      h5::write(gr, "dlr_if", m.dlr_if());
    }

    /**
     * @brief Read a triqs::mesh::dlr_imfreq mesh from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, dlr_imfreq &m) {
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
      m         = dlr_imfreq(b, stat, wmax, epsilon, sym, {.freq = freq, .imt = imt, .imf = imf});
    }

    // Friend declarations.
    friend struct dlr_imtime;
    friend struct dlr;

    private:
    double _beta                                = 1.0;
    statistic_enum _statistic                   = Fermion;
    double _w_max                               = 0.0;
    double _eps                                 = 1e-10;
    bool _symmetrize                            = false;
    uint64_t _mesh_hash                         = 0;
    std::shared_ptr<const detail::dlr_ops> _dlr = {};
  };

  /**
   * @brief Evaluating a function \f$ f \f$ at a given Matsubara frequency \f$ i\omega_n \f$ is deleted for 
   * triqs::mesh::dlr_imfreq meshes.
   */
  double evaluate(dlr_imfreq const &m, ...) = delete;

  /** @} */

  // Check mesh concepts.
  static_assert(Mesh<dlr_imfreq>);
  static_assert(MeshWithValues<dlr_imfreq>);

} // namespace triqs::mesh
