/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2023 by Hugo U. R. Strand
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
#include "./bases/linear.hpp"

#include <cppdlr/dlr_imtime.hpp>
#include <cppdlr/dlr_basis.hpp>
#include <cppdlr/utils.hpp>
#include <cppdlr/dlr_kernels.hpp>

namespace triqs::mesh {
  /**
   *  Imaginary-time Discrete Lehmann representation mesh
   *
   *  Mesh for the imaginary-time axis from $0$ to $\beta$.
   *
   *  The time points are computed using the cppdlr library
   */
  class dlr_imtime : tag::mesh {

    using domain_t       = matsubara_time_domain;
    using index_t        = long;
    using linear_index_t = long;
    using domain_pt_t    = typename domain_t::point_t;

    public:
    ///
    dlr_imtime() = default;

    ///
    dlr_imtime(dlr_imtime const &x) = default;

    /**
     * Construct a Mesh of Discrete Lehmann imaginary times
     * on a Matsubara time domain
     *
     * @param dom Matsubara time domain
     * @param lambda Lambda energy over beta parameter
     * @param eps Representation accuracy
     */
    dlr_imtime(matsubara_time_domain d, double lambda, double eps) : _dom(d), _lambda(lambda), _eps(eps), dlr(lambda, cppdlr::dlr_freq(lambda, eps)) {}

    /**
     * Construct a Mesh of imaginary times on the interval [0,beta]
     * including points at both edges.
     *
     * @param beta Inverse temperature
     * @param S Statistic (Fermion or Boson)
     * @param lambda Lambda energy over beta parameter
     * @param eps Representation accuracy
     */
    dlr_imtime(double beta, statistic_enum S, double lambda, double eps) : dlr_imtime({beta, S}, lambda, eps) {}

    // -------------------- Accessors (other) -------------------

    /// Lambda energy over beta parameter
    double lambda() const { return _lambda; }

    /// Representation accuracy
    double eps() const { return _eps; }
    
    // -------------------- Accessors (from concept) -------------------

    /// The corresponding domain
    domain_t const &domain() const { return _dom; }

    /// Size (linear) of the mesh of the window
    long size() const { return dlr.rank(); } // FIXME!
    
    // -------------------- print -------------------

    friend std::ostream &operator<<(std::ostream &sout, dlr_imtime const &m) {
      return sout << "DLR Imaginary Time Mesh of size " << m.size() << ", Domain: " << m.domain();
    }

    // -------------------- hdf5 -------------------

    static std::string hdf5_format() { return "MeshDLRImTime"; }

    friend void h5_write(h5::group fg, std::string const &subgroup_name, dlr_imtime const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format_as_string(gr, "MeshDLRImTime");
      h5_write(gr, "domain", m.domain());
      h5_write(gr, "lambda", m.lambda());
      h5_write(gr, "eps", m.eps());
    }

    friend void h5_read(h5::group fg, std::string const &subgroup_name, dlr_imtime &m) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format_as_string(gr, "MeshDLRImTime", true);
      typename dlr_imtime::domain_t dom;
      double lambda, eps;
      h5_read(gr, "domain", dom);
      h5_read(gr, "lambda", lambda);
      h5_read(gr, "eps", eps);
      m = dlr_imtime(std::move(dom), lambda, eps);
    }

    // ------------------------------------------------
    private:
    domain_t _dom;
    double _lambda, _eps;
    cppdlr::imtime_ops dlr;
  };

} // namespace triqs::mesh
