// Copyright (c) 2014-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2017 Centre national de la recherche scientifique (CNRS)
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
 * @brief Provides a mesh type on the real time axis.
 */

#pragma once

#include "./bases/linear.hpp"
#include "../utility/macros.hpp"

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
   * @brief Real time mesh type.
   *
   * @details A real time mesh is defined by its size \f$ N \geq 0 \f$ and a time interval \f$ [t_{\mathrm{min}},
   * t_{\mathrm{max}}] \f$. It contains \f$ N \f$ equally spaced mesh points on the interval \f$ [t_{\mathrm{min}},
   * t_{\mathrm{max}}] \f$ such that the distance between two consecutive mesh points (step size) is constant.
   *
   * A real time mesh has the following properties:
   *
   * - Each mesh point is identified by a unique index \f$ n \in \{0, 1, \ldots, N-1\} \f$.
   * - An index \f$ n \f$ is mapped to the corresponding data index \f$ d \f$ by the identity function \f$ d(n) = n \f$
   *   and vice versa.
   * - An index \f$ n \f$ is mapped to the corresponding value \f$ t \f$ by the linear function
   *   \f$ t(n) = t_{\mathrm{min}} + n \cdot \Delta \f$ such that \f$ t(0) = t_{\mathrm{min}} \f$ and
   *   \f$ t(N - 1) = t_{\mathrm{max}} \f$. The step size of
   *   the mesh is \f$ \Delta = \frac{t_{\mathrm{max}} - t_{\mathrm{min}}}{N - 1} \f$ for \f$ N > 1 \f$, otherwise it is
   *   undefined. For implementation purposes, we set \f$ \Delta = 0 \f$ and \f$ \Delta^{-1} = 0 \f$ for \f$ N = 0 \f$
   *   and \f$ \Delta = 0 \f$ and \f$ \Delta^{-1} = \infty \f$ for \f$ N = 1 \f$.
   * - An arbitrary value \f$ t \in [t_{\mathrm{min}}, t_{\mathrm{max}}] \f$ is mapped to the closest mesh point with
   *   index \f$ n \f$ by the function
   *   \f$ n(t) = \left\lfloor \frac{t - t_{\mathrm{min}}}{\Delta} + 0.5 \right\rfloor \f$.
   *
   * Green's function containers that are based on a real time mesh store the function values at the discrete time
   * points \f$ t(n) \f$, i.e. \f$ f_n = f(t(n)) \f$, and use linear interpolation to evaluate the function at an
   * arbitrary time \f$ t \in [t_{\mathrm{min}}, t_{\mathrm{max}}] \f$.
   */
  class C2PY_RENAME(MeshReTime) retime : public detail::linear<retime, double> {
    public:
    /// %Mesh point type of a triqs::mesh::retime mesh (see triqs::mesh::detail::linear::mesh_point_t).
    using mesh_point_t = detail::linear<retime, double>::mesh_point_t;

    /**
     * @brief Construct a real time mesh on the interval \f$ [t_{\mathrm{min}}, t_{\mathrm{max}}] \f$ with \f$ N \geq 0
     * \f$ equally spaced mesh points.
     *
     * @param t_min Lower bound \f$ t_{\mathrm{min}} \f$ of the time interval.
     * @param t_max Upper bound \f$ t_{\mathrm{max}} \f$ of the time interval.
     * @param n_t Size of the mesh.
     */
    C2PY_DEPRECATED_PARAMETER_NAME(n_max : n_t)
    retime(double t_min = 0.0, double t_max = 0.0, long n_t = 0) : linear(t_min, t_max, n_t) {}

    /**
     * @brief Construct a real time mesh on the interval \f$ [t_{\mathrm{min}}, t_{\mathrm{max}}] \f$ with \f$ N \geq 0
     * \f$ equally spaced mesh points.
     *
     * @param window Pair containing the lower and upper bounds of the time interval.
     * @param n_t Size of the mesh.
     */
    retime(std::pair<double, double> window, int n_t) : retime(window.first, window.second, n_t) {}

    /// Get the lower bound of the interval \f$ t_{\mathrm{min}} \f$, i.e. the value of the first mesh point.
    [[nodiscard]] C2PY_PROPERTY_GET(t_min) double t_min() const noexcept { return a_; }

    /// Get the upper bound of the interval \f$ t_{\mathrm{max}} \f$, i.e. the value of the last mesh point.
    [[nodiscard]] C2PY_PROPERTY_GET(t_max) double t_max() const noexcept { return b_; }

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "MeshReTime"; }

    /**
     * @brief Write a triqs::mesh::retime mesh to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, retime const &m) { m.h5_write_impl(g, name, "MeshReTime"); }

    /**
     * @brief Read a triqs::mesh::retime mesh from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, retime &m) { m.h5_read_impl(g, name, "MeshReTime"); }

    /**
     * @brief Write a triqs::mesh::retime mesh to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param m %Mesh to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, retime const &m) {
      return sout << fmt::format("Real time mesh with t_min = {}, t_max = {}, N = {}", m.a_, m.b_, m.N_);
    }
  };

  /**
   * @brief Linear interpolation of a function \f$ f \f$ defined on a triqs::mesh::retime mesh at a real time point \f$
   * t \in [t_{\mathrm{min}}, t_{\mathrm{max}}] \f$.
   *
   * @details It simply calls the triqs::mesh::retime::evaluate method of the mesh.
   *
   * @param m triqs::mesh::retime mesh.
   * @param f Callable object \f$ f \f$ containing the function values \f$ f_n = f(t(n)) \f$ at the mesh points.
   * @param t Real time point \f$ t \f$ at which to interpolate the function.
   * @return Linear interpolation of \f$ f(t) \f$.
   */
  auto evaluate(retime const &m, auto const &f, double t) { return m.evaluate(f, t); }

  /** @} */

} // namespace triqs::mesh
