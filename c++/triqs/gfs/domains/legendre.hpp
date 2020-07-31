// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
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

#pragma once

namespace triqs {
  namespace gfs {

    class legendre_domain {

      public:
      double beta;
      statistic_enum statistic;
      size_t Nmax;

      using point_t = long;
      size_t size() const { return Nmax; };

      legendre_domain(double beta_ = 1, statistic_enum stat_ = Fermion, size_t Nmax_ = 1) : beta(beta_), statistic(stat_), Nmax(Nmax_) {}

      bool operator==(legendre_domain const &D) const {
        return ((std::abs(beta - D.beta) < 1.e-15) && (statistic == D.statistic) && (Nmax == D.Nmax));
      }

      /// Write into HDF5
      friend void h5_write(h5::group fg, std::string subgroup_name, legendre_domain const &d) {
        h5::group gr = fg.create_group(subgroup_name);
        h5_write(gr, "n_max", d.Nmax);
        h5_write(gr, "beta", d.beta);
        h5_write(gr, "statistic", (d.statistic == Fermion ? "F" : "B"));
      }

      /// Read from HDF5
      friend void h5_read(h5::group fg, std::string subgroup_name, legendre_domain &d) {
        h5::group gr = fg.open_group(subgroup_name);
        size_t n;
        double beta;
        std::string statistic = " ";
        h5_read(gr, "n_max", n);
        h5_read(gr, "beta", beta);
        h5_read(gr, "statistic", statistic);
        d = legendre_domain(beta, (statistic == "F" ? Fermion : Boson), n);
      }

      //  BOOST Serialization
      friend class boost::serialization::access;
      template <class Archive> void serialize(Archive &ar, const unsigned int version) {
        ar &Nmax;
        ar &beta;
        ar &statistic;
      }
    };
  } // namespace gfs
} // namespace triqs
