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
#include "./utils.hpp"
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
  struct imtime : public details::linear<imtime, double> {

    // -------------------- Data -------------------

    double beta;
    statistic_enum statistic;

    // -------------------- Constructors -------------------

    /**
     * Construct a Mesh of imaginary times on the interval [0,beta]
     * including points at both edges.
     *
     * @param beta Inverse temperature
     * @param statistic Statistic (Fermion or Boson)
     * @param n_tau Number of mesh-points
     */
    imtime(double beta = 0.0, statistic_enum statistic = Fermion, long n_tau = 2) : linear(0, beta, n_tau), beta(beta), statistic(statistic) {}

    /**
     * Construct a Mesh of imaginary times on a Matsubara time domain
     *
     * @param dom Matsubara time domain
     * @param n_tau Number of mesh-points
     */
    [[deprecated("matsubara_time_domain is deprecated")]] imtime(matsubara_time_domain d, long n_tau) : imtime(d.beta, d.statistic, n_tau) {}

    // -------------------- Comparison -------------------

    bool operator==(imtime const &) const = default;
    bool operator!=(imtime const &) const = default;

    // -------------------- Accessors -------------------

    /// The associated domain
    [[deprecated("matsubara_time_domain is deprecated")]] [[nodiscard]] matsubara_time_domain domain() const noexcept { return {beta, statistic}; }

    // -------------------- HDF5 -------------------

    [[nodiscard]] static std::string hdf5_format() { return "MeshImTime"; }

    friend void h5_write(h5::group fg, std::string const &subgroup_name, imtime const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, m); //NOLINT

      h5::write(gr, "beta", m.beta);
      h5::write(gr, "statistic", (m.statistic == Fermion ? "F" : "B"));
      h5::write(gr, "n_tau", m.size());
    }

    friend void h5_read(h5::group fg, std::string const &subgroup_name, imtime &m) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format(gr, m, true);

      long n_tau;                                                            // NOLINT
      if (not h5::try_read(gr, "n_tau", n_tau)) h5::read(gr, "size", n_tau); // Backward Compat

      if (gr.has_key("domain")) { gr = gr.open_group("domain"); } // Backward Compat
      auto beta      = h5::read<double>(gr, "beta");
      auto statistic = (h5::read<std::string>(gr, "statistic") == "F" ? Fermion : Boson);

      m = imtime(beta, statistic, n_tau);
    }

    // -------------------- Print -------------------

    friend std::ostream &operator<<(std::ostream &sout, imtime const &m) {
      auto stat_cstr = (m.statistic == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("Imaginary Time Mesh with beta = {}, statistic = {}, n_tau = {}", m.beta, stat_cstr, m.size());
    }
  };

  ///
  auto evaluate(imtime const &m, auto const &f, double x) { return m.evaluate(f, x); }

  // check concept
  static_assert(MeshWithValues<imtime>);

} // namespace triqs::mesh
