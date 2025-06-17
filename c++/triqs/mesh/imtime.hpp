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
 * @brief Provides a mesh type on the imaginary time axis.
 */

#pragma once

#include "./bases/linear.hpp"
#include "./domains/matsubara.hpp"
#include "./utils.hpp"

#include <fmt/format.h>
#include <h5/h5.hpp>

#include <iostream>
#include <string>

namespace triqs::mesh {

  /**
   * @addtogroup triqs-meshes-imag
   * @{
   */

  /**
   * @brief Imaginary time mesh type.
   *
   * @details An imaginary time mesh satisfies the triqs::mesh::MeshWithValues concept and is defined by its size
   * \f$ N \geq 0 \f$, an inverse temperature \f$ \beta > 0 \f$ and its particle statistics (see 
   * triqs::mesh::statistic_enum).
   *
   * It is a triqs::mesh::detail::linear mesh with \f$ N \f$ equally spaced mesh points on the interval \f$ [0, \beta] 
   * \f$ such that
   * - \f$ \tau(n) = n \cdot \Delta \f$ with \f$ \Delta = \frac{\beta}{N - 1} \f$ for \f$ N > 1 \f$,
   * - \f$ \tau(0) = 0 \f$ and
   * - \f$ \tau(N - 1) = \beta \f$.
   * 
   * @ref triqs-gfs containers that are based on an imaginary time mesh store the function values at the discrete time 
   * points \f$ \tau(n) \f$, i.e. \f$ f_n = f(\tau(n)) \f$, and use linear interpolation to evaluate the function at an 
   * arbitrary imaginary time \f$ \tau \in [0, \beta] \f$ (see triqs::mesh::evaluate(imtime const &, auto const &, 
   * double) for details). 
   *
   * @code
   * #include <fmt/base.h>
   * #include <triqs/mesh.hpp>
   * 
   * int main() {
   *   // initialize a fermionic imaginary time mesh with 5 points and beta = 10
   *   triqs::mesh::imtime m{10, triqs::mesh::Fermion, 5};
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
   * mesh point #0: index = 0, data index = 0, value = 0
   * mesh point #1: index = 1, data index = 1, value = 2.5
   * mesh point #2: index = 2, data index = 2, value = 5
   * mesh point #3: index = 3, data index = 3, value = 7.5
   * mesh point #4: index = 4, data index = 4, value = 10
   * ```
   */
  class imtime : public detail::linear<imtime, double> {
    public:
    /// %Mesh point type of a triqs::mesh::imtime mesh (see triqs::mesh::detail::linear::mesh_point_t).
    using mesh_point_t = detail::linear<imtime, double>::mesh_point_t;

    /**
     * @brief Construct an imaginary time mesh on the interval \f$ [0, \beta] \f$ with \f$ N \geq 0 \f$ equally spaced
     * mesh points and the given particle statistics.
     *
     * @param beta Inverse temperature \f$ \beta > 0 \f$.
     * @param stat Particle statistics (see triqs::mesh::statistic_enum).
     * @param N Size of the mesh.
     */
    imtime(double beta = 1.0, statistic_enum stat = Fermion, long N = 0) : linear(0, beta, N), beta_(beta), stat_(stat) {}

    /**
     * @brief Construct an imaginary time mesh on a given `triqs::mesh::matsubara_time_domain` with \f$ N \geq 0 \f$
     * equally spaced mesh points.
     *
     * @deprecated Use imtime(double, statistic_enum, long) instead.
     * 
     * @param dom `triqs::mesh::matsubara_time_domain` object.
     * @param N Size of the mesh.
     */
    [[deprecated("matsubara_time_domain is deprecated")]] imtime(matsubara_time_domain dom, long N) : imtime(dom.beta, dom.statistic, N) {}

    /// Equal-to comparison operator compares \f$ N \f$, \f$ \beta \f$ and the particle statistics.
    bool operator==(imtime const &) const = default;

    /// Not-equal-to comparison operator compares \f$ N \f$, \f$ \beta \f$ and the particle statistics.
    bool operator!=(imtime const &) const = default;

    /// Get the inverse temperature \f$ \beta \f$.
    [[nodiscard]] double beta() const noexcept { return beta_; }

    /// Get the particle statistics.
    [[nodiscard]] statistic_enum statistic() const noexcept { return stat_; }

    /**
     * @brief Get the Matsubara time domain.
     * @deprecated `triqs::mesh::matsubara_time_domain` is deprecated.
     */
    [[deprecated("matsubara_time_domain is deprecated")]] [[nodiscard]] matsubara_time_domain domain() const noexcept { return {beta_, stat_}; }

    /**
     * @brief Serialize the mesh to a generic archive.
     * @param ar Archive to serialize to.
     */
    void serialize(auto &ar) const {
      static_cast<detail::linear<imtime, double> const &>(*this).serialize(ar);
      ar & beta_ & stat_;
    }

    /**
     * @brief Deserialize the mesh from a generic archive.
     * @param ar Archive to deserialize from.
     */
    void deserialize(auto &ar) {
      static_cast<detail::linear<imtime, double> &>(*this).deserialize(ar);
      ar & beta_ & stat_;
    }

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "MeshImTime"; }

    /**
     * @brief Write a triqs::mesh::imtime mesh to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, imtime const &m) {
      h5::group gr = g.create_group(name);
      h5::write_hdf5_format(gr, m); // NOLINT (downcasting to base class)
      h5::write(gr, "beta", m.beta_);
      h5::write(gr, "statistic", (m.stat_ == Fermion ? "F" : "B"));
      h5::write(gr, "n_tau", m.N_);
    }

    /**
     * @brief Read a triqs::mesh::imtime mesh from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, imtime &m) {
      h5::group gr = g.open_group(name);
      h5::assert_hdf5_format(gr, m, true);

      // for backward compatibility
      long N = 0;
      if (not h5::try_read(gr, "n_tau", N)) h5::read(gr, "size", N);
      if (gr.has_key("domain")) gr = gr.open_group("domain");

      auto beta      = h5::read<double>(gr, "beta");
      auto statistic = (h5::read<std::string>(gr, "statistic") == "F" ? Fermion : Boson);
      m              = imtime(beta, statistic, N);
    }

    /**
     * @brief Write a triqs::mesh::imtime mesh to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param m %Mesh to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, imtime const &m) {
      auto stat_cstr = (m.stat_ == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("Imaginary time mesh with beta = {}, statistics = {}, N = {}", m.beta_, stat_cstr, m.N_);
    }

    private:
    double beta_;
    statistic_enum stat_;
  };

  /**
   * @brief Linear interpolation of a function \f$ f \f$ defined on a triqs::mesh::imtime mesh at an imaginary time
   * point \f$ \tau \in [0, \beta] \f$.
   *
   * @details It simply calls the triqs::mesh::imtime::evaluate method of the mesh.
   *
   * @param m triqs::mesh::imtime mesh.
   * @param f Callable object \f$ f \f$ containing the function values \f$ f_n = f(\tau(n)) \f$ at the mesh points.
   * @param tau Imaginary time point \f$ \tau \f$ at which to interpolate the function.
   * @return Linear interpolation of \f$ f(\tau) \f$.
   */
  auto evaluate(imtime const &m, auto const &f, double tau) { return m.evaluate(f, tau); }

  /** @} */

  // Check mesh concepts.
  static_assert(Mesh<imtime>);
  static_assert(MeshWithValues<imtime>);

} // namespace triqs::mesh
