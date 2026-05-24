// Copyright (c) 2022-2023 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides functions to create adjoint meshes.
 */

#pragma once

#include "./imtime.hpp"
#include "./imfreq.hpp"
#include "./dlr_imtime.hpp"
#include "./dlr_imfreq.hpp"
#include "./retime.hpp"
#include "./refreq.hpp"
#include "./cyclat.hpp"
#include "./brzone.hpp"

#include <numbers>

namespace triqs::mesh {

  /**
   * @ingroup triqs-meshes-imag
   * @brief Create the adjoint imaginary-frequency mesh to a given imaginary-time mesh.
   *
   * @details If \f$ N_{i\omega_n} = -1 \f$, the number of positive Matsubara frequencies is set to \f$ N_{i\omega_n} =
   * N / 6 \f$, where \f$ N \f$ is the size of the given imaginary time mesh.
   *
   * @param m Input mesh.
   * @param n_iw Number of positive Matsubara frequencies, i.e. \f$ N_{i\omega_n} \f$.
   * @return Imaginary frequency mesh with the same \f$ \beta \f$ and particle statistics as the given imaginary time
   * mesh and \f$ N_{i\omega_n} \f$ positive Matsubara frequencies.
   */
  [[nodiscard]] inline imfreq make_adjoint_mesh(imtime const &m, long n_iw = -1) {
    if (n_iw == -1) n_iw = (m.size() - 1) / 6;
    return {m.beta(), m.statistic(), n_iw};
  }

  /**
   * @ingroup triqs-meshes-imag
   * @brief Create the adjoint imaginary-time mesh to a given imaginary-frequency mesh.
   *
   * @details If \f$ N = -1 \f$, the size of the imaginary time mesh is set to \f$ N = 6 (n_{\text{max}} + 1) + 1 \f$,
   * where \f$ n_{\text{max}} \f$ is the largest positive Matsubara index in the given imaginary frequency mesh.
   *
   * @param m Input mesh.
   * @param n_tau Size of the imaginary time mesh.
   * @return Imaginary time mesh with the same \f$ \beta \f$ and particle statistics as the given imaginary frequency
   * mesh and size \f$ N \f$.
   */
  [[nodiscard]] inline imtime make_adjoint_mesh(imfreq const &m, long n_tau = -1) {
    if (n_tau == -1) n_tau = 6 * (m.last_index() + 1) + 1;
    return {m.beta(), m.statistic(), n_tau};
  }

  /**
   * @ingroup triqs-meshes-imag
   * @brief Create the adjoint imaginary-frequency DLR mesh to a given imaginary-time DLR mesh.
   *
   * @details It constructs the imaginary-frequency DLR mesh from the given imaginary-time DLR mesh.
   *
   * @param m Input mesh.
   * @return Imaginary frequency DLR mesh.
   */
  [[nodiscard]] inline dlr_imfreq make_adjoint_mesh(dlr_imtime const &m) { return dlr_imfreq{m}; }

  /**
   * @ingroup triqs-meshes-imag
   * @brief Create the adjoint imaginary-time DLR mesh to a given imaginary-frequency DLR mesh.
   *
   * @details It constructs the imaginary-time DLR mesh from the given imaginary-frequency DLR mesh.
   *
   * @param m Input mesh.
   * @return Imaginary time DLR mesh.
   */
  [[nodiscard]] inline dlr_imtime make_adjoint_mesh(dlr_imfreq const &m) { return dlr_imtime{m}; }

  /**
   * @ingroup triqs-meshes-real
   * @brief Create the adjoint real-frequency mesh to a given real-time mesh.
   *
   * @details The resulting frequency mesh is defined on the interval \f$ [\omega_{\text{min}}, \omega_{\text{max}}]
   * \f$ with \f$ \omega_{\text{max}} = \pi (N - 1) / (N \Delta) \f$ and \f$ \omega_{\text{min}} = -\omega_{\text{max}}
   * \f$, where \f$ N \f$ and \f$ \Delta \f$ are the size and step size of the given real time mesh, respectively.
   *
   * If `shift_half_bin` is true, the frequency mesh is shifted by half a bin to the right, i.e. by \f$ \pi / (N \Delta)
   * \f$.
   *
   * @param m Input mesh.
   * @param shift_half_bin If true, shift the frequency mesh by half a bin to the right.
   * @return Real frequency mesh on the interval \f$ [\omega_{\text{min}}, \omega_{\text{max}}] \f$ with \f$ N \f$
   * equally spaced mesh points.
   */
  [[nodiscard]] inline refreq make_adjoint_mesh(retime const &m, bool shift_half_bin = false) {
    using std::numbers::pi;
    auto const N    = static_cast<double>(m.size());
    auto const wmax = pi * (N - 1) / (N * m.delta());
    if (shift_half_bin) return {-wmax + pi / N / m.delta(), wmax + pi / N / m.delta(), m.size()};
    return {-wmax, wmax, m.size()};
  }

  /**
   * @ingroup triqs-meshes-real
   * @brief Create the adjoint real-time mesh to a given real-frequency mesh.
   *
   * @details The resulting time mesh is defined on the interval \f$ [t_{\text{min}}, t_{\text{max}}] \f$ with
   * \f$ t_{\text{max}} = \pi (N - 1) / (N \Delta) \f$ and \f$ t_{\text{min}} = -t_{\text{max}} \f$, where \f$ N \f$ and
   * \f$ \Delta \f$ are the size and step size of the given real frequency mesh, respectively.
   *
   * If `shift_half_bin` is true, the time mesh is shifted by half a bin to the right, i.e. by \f$ \pi / (N \Delta) \f$.
   *
   * @param m Input mesh.
   * @param shift_half_bin If true, shift the time mesh by half a bin to the right.
   * @return Real time mesh on the interval \f$ [t_{\text{min}}, t_{\text{max}}] \f$ with \f$ N \f$ equally spaced mesh
   * points.
   */
  [[nodiscard]] inline retime make_adjoint_mesh(refreq const &m, bool shift_half_bin = false) {
    using std::numbers::pi;
    auto const N    = static_cast<double>(m.size());
    auto const tmax = pi * (N - 1) / (N * m.delta());
    if (shift_half_bin) return {-tmax + pi / N / m.delta(), tmax + pi / N / m.delta(), m.size()};
    return {-tmax, tmax, m.size()};
  }

  /**
   * @ingroup triqs-meshes-lattice
   * @brief Create the adjoint Brillouin-zone mesh to a given cyclic-lattice mesh.
   *
   * @param m Input mesh.
   * @return Brillouin zone mesh compatible with the given cyclic lattice mesh and its periodic boundary conditions.
   */
  [[nodiscard]] inline brzone make_adjoint_mesh(cyclat const &m) { return {brillouin_zone{m.lattice()}, m.dims()}; }

  /**
   * @ingroup triqs-meshes-lattice
   * @brief Create the adjoint cyclic-lattice mesh to a given Brillouin-zone mesh.
   *
   * @param m Input mesh.
   * @return Cyclic lattice mesh compatible with the given BZ mesh and its periodic boundary conditions.
   */
  [[nodiscard]] inline cyclat make_adjoint_mesh(brzone const &m) { return {m.bz().lattice(), m.dims()}; }

} // namespace triqs::mesh
