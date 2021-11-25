/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once
#include "./details/mesh_tools.hpp"
#include "./domains/matsubara.hpp"
#include "./bases/linear.hpp"

namespace triqs::mesh {

  /**
   *  Imaginary-time Matsubara mesh
   *
   *  Mesh for the imaginary-time axis from $0$ to $\beta$.
   *
   *  The time points are defined as follows, for $n=0{\dots}N-1$: $$\tau_n=\frac{n}{N-1}\beta$$ where $N$ is the size of the mesh.
   *
   *  @figure ../../../triqs/mesh/matsubara_imtime.png: Pictorial representation of ``imtime{beta, Fermion/Boson, 4}``.
   */
  class imtime : public linear_mesh<matsubara_time_domain> {
    using B = linear_mesh<matsubara_time_domain>;

    public:
    ///
    imtime() = default;

    ///
    imtime(imtime const &x) = default;

    /**
     * Construct a Mesh of imaginary times on a Matsubara time domain
     *
     * @param dom Matsubara time domain
     * @param n_tau Number of mesh-points
     */
    imtime(matsubara_time_domain d, size_t n_tau) : B(d, 0, d.beta, n_tau) {}

    /**
     * Construct a Mesh of imaginary times on the interval [0,beta]
     * including points at both edges.
     *
     * @param beta Inverse temperature
     * @param S Statistic (Fermion or Boson)
     * @param n_tau Number of mesh-points
     */
    imtime(double beta, statistic_enum S, size_t n_tau) : imtime({beta, S}, n_tau) {}

    // -------------------- print -------------------

    friend std::ostream &operator<<(std::ostream &sout, imtime const &m) {
      return sout << "Imaginary Time Mesh of size " << m.size() << ", Domain: " << m.domain();
    }

    // -------------------- hdf5 -------------------

    static std::string hdf5_format() { return "MeshImTime"; }

    friend void h5_write(h5::group fg, std::string const &subgroup_name, imtime const &m) { h5_write_impl(fg, subgroup_name, m, "MeshImTime"); }

    friend void h5_read(h5::group fg, std::string const &subgroup_name, imtime &m) { h5_read_impl(fg, subgroup_name, m, "MeshImTime"); }
  };

} // namespace triqs::mesh
