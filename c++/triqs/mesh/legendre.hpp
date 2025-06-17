// Copyright (c) 2013-2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2016 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2019-2023 Simons Foundation
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
// Authors: Philipp Dumitrescu, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides a mesh type for Legendre polynomials as basis functions.
 */

#pragma once

#include "./mesh_iterator.hpp"
#include "./utils.hpp"
#include "../utility/legendre.hpp"

#include <fmt/format.h>
#include <h5/h5.hpp>
#include <itertools/itertools.hpp>
#include <nda/nda.hpp>

#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

namespace triqs::mesh {

  /**
   * @addtogroup triqs-meshes-func
   * @{
   */

  /**
   * @brief Legendre mesh type.
   *
   * @details A Legendre mesh satisfies the triqs::mesh::Mesh concept. It is defined by the number of Legendre
   * polynomials \f$ N \f$ used in the series expansion, an inverse temperature \f$ \beta > 0 \f$ and its particle 
   * statistics (see triqs::mesh::statistic_enum).
   *
   * A Legendre mesh is similar to the triqs::mesh::discrete mesh and has the following properties:
   *
   * - Each mesh point is identified by a unique index \f$ n \in \{0, 1, \ldots, N-1\} \f$.
   * - An index \f$ n \f$ is mapped to the corresponding data index \f$ d \f$ by the identity function \f$ d(n) = n \f$
   * and vice versa.
   * - There is no explicit value associated with a mesh point, i.e. it does not satisfy the
   * triqs::mesh::MeshWithValues concept.
   *
   * @ref triqs-gfs containers that are based on a Legendre mesh store the coefficients \f$ f_n \f$ of the generalized 
   * Fourier series expansion of a function \f$ f(\tau) \f$ in terms of Legendre polynomials, which is defined on the 
   * imaginary time axis. To evaluate the function at an arbitrary imaginary time \f$ \tau \in [0, \beta] \f$, the GF
   * container sums up the Fourier series to the maximum degree \f$ N-1 \f$ (see triqs::mesh::evaluate(legendre const &, 
   * auto const &, double) for details). 
   *
   * @code
   * #include <fmt/base.h>
   * #include <triqs/mesh.hpp>
   * 
   * int main() {
   *   // initialize a Legendre mesh with beta = 10, fermionic statistics and 5 polynomials
   *   triqs::mesh::legendre m{10, triqs::mesh::Fermion, 5};
   * 
   *   // loop over all mesh points and print their index and data index
   *   for (int i = 0; auto mp : m) fmt::println("mesh point #{}: index = {}, data index = {}", i++, mp.index(), mp.data_index());
   * }
   * @endcode
   *
   * Output:
   *
   * ```
   * mesh point #0: index = 0, data index = 0
   * mesh point #1: index = 1, data index = 1
   * mesh point #2: index = 2, data index = 2
   * mesh point #3: index = 3, data index = 3
   * mesh point #4: index = 4, data index = 4
   * ```
   */
  class legendre {
    public:
    /// Index type.
    using index_t = long;

    /// Data index type.
    using data_index_t = long;

    /**
     * @brief %Mesh point of a triqs::mesh::legendre mesh.
     * @details It stores the index \f$ n \f$, the data index \f$ d \f$ and the hash value of the parent mesh.
     */
    class mesh_point_t {
      public:
      /// Parent mesh type.
      using mesh_t = legendre;

      /// Default constructor leaves the mesh point uninitialized.
      mesh_point_t() = default;

      /**
       * @brief Construct a mesh point with a given index \f$ n \f$, data index \f$ d \f$ and hash value of the parent 
       * mesh.
       *
       * @param n Index \f$ n \f$ of the mesh point.
       * @param d Data index \f$ d \f$ of the mesh point.
       * @param mhash Hash value of the parent mesh.
       */
      mesh_point_t(long n, long d, uint64_t mhash) : _index(n), _data_index(d), _mesh_hash(mhash) {}

      /// Get the index \f$ n \f$ of the mesh point.
      [[nodiscard]] long index() const { return _index; }

      /// Get the data index \f$ d \f$ of the mesh point.
      [[nodiscard]] long data_index() const { return _data_index; }

      /// Get the hash value of the parent mesh.
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }

      private:
      long _index         = 0;
      long _data_index    = 0;
      uint64_t _mesh_hash = 0;
    };

    /// Default constructor constructs an empty mesh.
    legendre() = default;

    /**
     * @brief Construct a mesh of Legendre polynomials with degrees \f$ n = 0, 1, \ldots, N - 1 \f$ on the interval \f$ 
     * [0, \beta] \f$ and the given particle statistics.
     *
     * @param beta Inverse temperature \f$ \beta > 0 \f$.
     * @param stat Particle statistics (see triqs::mesh::statistic_enum).
     * @param N Size of the mesh, i.e. the number of Legendre polynomial used in the series expansion.
     */
    legendre(double beta, statistic_enum stat, long N) : _beta(beta), _statistic(stat), _max_n(N), _mesh_hash(hash(beta, stat, N)) {
      EXPECTS(_beta > 0);
      EXPECTS(_max_n >= 0);
    }

    /// Equal-to comparison operator compares \f$ N \f$, \f$ \beta \f$ and the particle statistics.
    bool operator==(legendre const &) const = default;

    /// Not-equal-to comparison operator compares \f$ N \f$, \f$ \beta \f$ and the particle statistics.
    bool operator!=(legendre const &) const = default;

    /**
     * @brief Check if an index \f$ n \f$ is valid.
     *
     * @param n Index \f$ n \f$ to check.
     * @return True if \f$ 0 \leq n < N \f$, false otherwise.
     */
    [[nodiscard]] bool is_index_valid(index_t n) const noexcept { return 0 <= n and n < _max_n; }

    /**
     * @brief Map an index \f$ n \in \{0, 1, \ldots, N-1\} \f$ to its corresponding data index \f$ d(n) \f$.
     *
     * @param n Index \f$ n \f$ to map.
     * @return Data index \f$ d(n) = n \f$.
     */
    [[nodiscard]] data_index_t to_data_index(index_t n) const noexcept {
      EXPECTS(is_index_valid(n));
      return n;
    }

    /**
     * @brief Map a data index \f$ d \in \{0, 1, \ldots, N-1\} \f$ to the corresponding index \f$ n(d) \f$.
     *
     * @param d Data index \f$ d \f$ to map.
     * @return Index \f$ n(d) = d \f$.
     */
    [[nodiscard]] index_t to_index(long d) const noexcept {
      EXPECTS(is_index_valid(d));
      return d;
    }

    /**
     * @brief Subscript operator to access a mesh point by its data index \f$ d \in \{0, 1, \ldots, N-1\} \f$.
     *
     * @param d Data index \f$ d \f$ of the mesh point.
     * @return mesh_point_t with the index \f$ n(d) = d \f$, data index \f$ d \f$ and hash value of the current mesh.
     */
    [[nodiscard]] mesh_point_t operator[](long d) const { return {to_index(d), d, _mesh_hash}; }

    /**
     * @brief Function call operator to access a mesh point by its index \f$ n \in \{0, 1, \ldots, N-1\} \f$.
     *
     * @param n Index \f$ n \f$ of the mesh point.
     * @return mesh_point_t with the index \f$ n \f$, data index \f$ d(n) = n \f$ and hash value of the current mesh.
     */
    [[nodiscard]] mesh_point_t operator()(long n) const { return {n, to_data_index(n), _mesh_hash}; }

    /// Get the inverse temperature \f$ \beta \f$.
    [[nodiscard]] double beta() const noexcept { return _beta; }

    /// Get the particle statistics.
    [[nodiscard]] auto statistic() const noexcept { return _statistic; }

    /// Get the hash value of the mesh.
    [[nodiscard]] uint64_t mesh_hash() const { return _mesh_hash; }

    /// Get the size \f$ N \f$ of the mesh, i.e. the number of mesh points or polynomials in the series expansion.
    [[nodiscard]] long size() const { return _max_n; }

    /// Get an iterator to the beginning of the mesh.
    [[nodiscard]] auto begin() const { return mesh_iterator<legendre>{.mesh_ptr = this, .data_index = 0}; }

    /// Get a const iterator to the beginning of the mesh.
    [[nodiscard]] auto cbegin() const { return begin(); }

    /// Get an iterator to the end of the mesh.
    [[nodiscard]] auto end() const { return mesh_iterator<legendre>{.mesh_ptr = this, .data_index = size()}; }

    /// Get a const iterator to the end of the mesh.
    [[nodiscard]] auto cend() const { return end(); }

    /**
     * @brief Write a triqs::mesh::legendre mesh to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param m %Mesh to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, legendre const &m) {
      auto stat_cstr = (m._statistic == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("Legendre mesh with beta = {}, statistics = {}, N = {}", m._beta, stat_cstr, m.size());
    }

    /**
     * @brief Serialize the mesh to a generic archive.
     * @param ar Archive to serialize to.
     */
    void serialize(auto &ar) const { ar & _beta & _statistic & _max_n & _mesh_hash; }

    /**
     * @brief Deserialize the mesh from a generic archive.
     * @param ar Archive to deserialize from.
     */
    void deserialize(auto &ar) { ar & _beta & _statistic & _max_n & _mesh_hash; }

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "MeshLegendre"; }

    /**
     * @brief Write a triqs::mesh::legendre mesh to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, legendre const &m) {
      h5::group gr = g.create_group(name);
      h5::write_hdf5_format(gr, m); // NOLINT (downcasting to base class)
      h5::write(gr, "beta", m._beta);
      h5::write(gr, "statistic", (m._statistic == Fermion ? "F" : "B"));
      h5::write(gr, "max_n", m._max_n);
    }

    /**
     * @brief Read a triqs::mesh::legendre mesh from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, legendre &m) {
      h5::group gr = g.open_group(name);
      h5::assert_hdf5_format(gr, m, true);

      // for backward compatibility
      if (gr.has_key("domain")) { gr = gr.open_group("domain"); }

      auto beta      = h5::read<double>(gr, "beta");
      auto statistic = (h5::read<std::string>(gr, "statistic") == "F" ? Fermion : Boson);

      // for backward compatibility
      long max_n{};
      if (not h5::try_read(gr, "max_n", max_n)) max_n = static_cast<long>(h5::read<size_t>(gr, "n_max"));

      m = legendre(beta, statistic, max_n);
    }

    private:
    double _beta              = 1.0;
    statistic_enum _statistic = Fermion;
    long _max_n               = 0;
    uint64_t _mesh_hash       = 0;
  };

  /**
   * @brief Evaluate the generalized Fourier approximation of a function \f$ f \f$ at a given imaginary time point \f$
   * \tau \in [0, \beta] \f$ in terms of Legendre polynomials.
   *
   * @details We calculate
   * \f[
   *   f(\tau) \approx \sum_{n=0}^{N-1} \frac{\sqrt{2n + 1}}{\beta} f_n P_n(x(\tau)) \; ,
   * \f]
   * where \f$ P_n \f$ denotes the Legendre polynomial of degree \f$ n \f$, \f$ x(\tau) = 2\tau / \beta - 1 \f$ is a
   * linear map from \f$ [0, \beta] \f$ to \f$ [-1, 1] \f$ and
   * \f[
   *   f_n = \sqrt{2n + 1} \int_0^\beta d\tau \, f(\tau) P_n(x(\tau)) \; ,
   * \f]
   * are the expansion coefficients.
   *
   * @param m triqs::mesh::legendre mesh.
   * @param f Callable object \f$ f \f$ containing the expansion coefficients \f$ f_n \f$.
   * @param tau Imaginary time point \f$ \tau \in [0, \beta] \f$ at which to approximate the function.
   * @return Generalized Fourier approximation of \f$ f(\tau) \f$.
   */
  inline auto evaluate(legendre const &m, auto const &f, double tau) {
    EXPECTS(m.size() > 0 and tau >= 0 and tau <= m.beta());
    utility::legendre_generator gen{};
    gen.reset(2 * tau / m.beta() - 1);
    return detail::sum_to_regular(nda::range(m.size()), [&](auto n) { return f(n) * std::sqrt(2 * n + 1) * gen.next() / m.beta(); });
  }

  /** @} */

  // Check mesh concepts.
  static_assert(Mesh<legendre>);
  static_assert(!MeshWithValues<legendre>);

} // namespace triqs::mesh
