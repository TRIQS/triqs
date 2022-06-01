/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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

namespace triqs::mesh {

  class legendre_domain {

    public:
    double beta;
    statistic_enum statistic;
    size_t max_n;

    using point_t = long;
    size_t size() const { return max_n; };

    /**
     *  Construct a Mesh of Legendre polynomials with degrees in the interval [0,n_l]
     *
     *  @param beta Inverse temperature
     *  @param S Statistic, Fermion or Boson
     *  @param n_l Largest degree
     */
    legendre_domain(double beta = 1, statistic_enum S = Fermion, size_t max_n = 1) : beta(beta), statistic(S), max_n(max_n) {}

    bool operator==(legendre_domain const &D) const {
      return ((std::abs(beta - D.beta) < 1.e-15) && (statistic == D.statistic) && (max_n == D.max_n));
    }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string subgroup_name, legendre_domain const &d) {
      h5::group gr = fg.create_group(subgroup_name);
      h5_write(gr, "max_n", d.max_n);
      h5_write(gr, "beta", d.beta);
      h5_write(gr, "statistic", (d.statistic == Fermion ? "F" : "B"));
    }

    /// Read from HDF5
    friend void h5_read(h5::group fg, std::string subgroup_name, legendre_domain &d) {
      h5::group gr = fg.open_group(subgroup_name);
      size_t n;
      double beta;
      std::string statistic = " ";
      if (not h5_try_read(gr, "max_n", n)) h5_read(gr, "n_max", n);
      h5_read(gr, "beta", beta);
      h5_read(gr, "statistic", statistic);
      d = legendre_domain(beta, (statistic == "F" ? Fermion : Boson), n);
    }

    //  BOOST Serialization
    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, const unsigned int version) {
      ar &max_n;
      ar &beta;
      ar &statistic;
    }
  };
} // namespace triqs::mesh
