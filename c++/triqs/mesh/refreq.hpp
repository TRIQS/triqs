// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Philipp Dumitrescu, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides a mesh type on the real frequency axis.
 */

#pragma once

#include "./bases/linear.hpp"
#include "./tail_fitter.hpp"

#include <fmt/format.h>
#include <h5/h5.hpp>

#include <iostream>
#include <string>
#include <utility>

namespace triqs::mesh {

  /**
   * @addtogroup triqs-meshes-real
   * @{
   */

  /**
   * @brief Real frequency mesh type.
   *
   * @details A real frequency mesh satisfies the triqs::mesh::MeshWithValues concept and is defined by its size \f$ N 
   * \geq 0 \f$ and a frequency interval \f$ [\omega_{\text{min}}, \omega_{\text{max}}] \f$.
   *
   * It is a triqs::mesh::detail::linear mesh with \f$ N \f$ equally spaced mesh points on the interval \f$ 
   * [\omega_{\text{min}}, \omega_{\text{max}}] \f$ such that
   * - \f$ \omega(n) = \omega_{\text{min}} + n \cdot \Delta \f$ with \f$ \Delta = \frac{\omega_{\text{max}} -
   * \omega_{\text{min}}}{N - 1} \f$ for \f$ N > 1 \f$,
   * - \f$ \omega(0) = \omega_{\text{min}} \f$ and
   * - \f$ \omega(N - 1) = \omega_{\text{max}} \f$.
   * 
   * @ref triqs-gfs containers that are based on a real frequency mesh store the function values at the discrete 
   * frequency points \f$ \omega(n) \f$, i.e. \f$ f_n = f(\omega(n)) \f$, and use linear interpolation to evaluate the 
   * function at an arbitrary frequency \f$ \omega \in [\omega_{\mathrm{min}}, \omega_{\mathrm{max}}] \f$ (see 
   * triqs::mesh::evaluate(refreq const &, auto const &, double) for details). 
   *
   * @code
   * #include <fmt/base.h>
   * #include <triqs/mesh.hpp>
   * 
   * int main() {
   *   // initialize a real frequency mesh with 5 points on the interval [-5, 5]
   *   triqs::mesh::refreq m{-5, 5, 5};
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
   * mesh point #0: index = 0, data index = 0, value = -5
   * mesh point #1: index = 1, data index = 1, value = -2.5
   * mesh point #2: index = 2, data index = 2, value = 0
   * mesh point #3: index = 3, data index = 3, value = 2.5
   * mesh point #4: index = 4, data index = 4, value = 5
   * ```
   */
  class refreq : public detail::linear<refreq, double>, public tail_fitter_handle {
    public:
    /// %Mesh point type of a triqs::mesh::refreq mesh (see triqs::mesh::detail::linear::mesh_point_t).
    using mesh_point_t = detail::linear<refreq, double>::mesh_point_t;

    /**
     * @brief Construct a real frequency mesh on the interval \f$ [\omega_{\text{min}}, \omega_{\text{max}}] \f$ with 
     * \f$ N \geq 0 \f$ equally spaced mesh points.
     *
     * @param w_min Lower bound \f$ \omega_{\mathrm{min}} \f$ of the frequency interval.
     * @param w_max Upper bound \f$ \omega_{\mathrm{max}} \f$ of the frequency interval.
     * @param N Size of the mesh.
     */
    refreq(double w_min = 0.0, double w_max = 0.0, long N = 0) : linear(w_min, w_max, N) {}

    /**
     * @brief Construct a real frequency mesh on the interval \f$ [\omega_{\text{min}}, \omega_{\text{max}}] \f$ with
     * \f$ N \geq 0  \f$ equally spaced mesh points.
     *
     * @param interval `std::pair` containing the lower and upper bounds of the frequency interval.
     * @param N Size of the mesh.
     */
    refreq(std::pair<double, double> interval, int N) : refreq(interval.first, interval.second, N) {}

    /// Is the mesh restricted to positive frequencies?
    static constexpr bool positive_only() { return false; }

    /// Get the lower bound of the interval \f$ \omega_{\text{min}} \f$, i.e. the value of the first mesh point.
    [[nodiscard]] double w_min() const { return a_; }

    /// Get the upper bound of the interval \f$ \omega_{\text{max}} \f$, i.e. the value of the last mesh point.
    [[nodiscard]] double w_max() const { return b_; }

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "MeshReFreq"; }

    /**
     * @brief Write a triqs::mesh::refreq mesh to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, refreq const &m) { m.h5_write_impl(g, name, "MeshReFreq"); }

    /**
     * @brief Read a triqs::mesh::refreq mesh from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, refreq &m) { m.h5_read_impl(g, name, "MeshReFreq"); }

    /**
     * @brief Write a triqs::mesh::refreq mesh to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param m %Mesh to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, refreq const &m) {
      return sout << fmt::format("Real frequency mesh with w_min = {}, w_max = {}, N = {}", m.a_, m.b_, m.N_);
    }
  };

  /**
   * @brief Linear interpolation of a function \f$ f \f$ defined on a triqs::mesh::refreq mesh at a real frequency \f$
   * \omega \in [\omega_{\text{min}}, \omega_{\text{max}}] \f$.
   *
   * @details It simply calls the triqs::mesh::refreq::evaluate method of the mesh.
   *
   * @param m triqs::mesh::refreq mesh.
   * @param f Callable object \f$ f \f$ containing the function values \f$ f_n = f(\omega(n)) \f$ at the mesh points.
   * @param w Real frequency \f$ \omega \f$ at which to interpolate the function.
   * @return Linear interpolation of \f$ f(\omega) \f$.
   */
  auto evaluate(refreq const &m, auto const &f, double w) { return m.evaluate(f, w); }

  /** @} */

  // Check mesh concepts.
  static_assert(Mesh<refreq>);
  static_assert(MeshWithValues<refreq>);

} // namespace triqs::mesh
