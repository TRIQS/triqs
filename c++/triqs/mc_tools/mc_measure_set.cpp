// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#include "./concepts.hpp"
#include "./mc_measure_set.hpp"

#include <fmt/format.h>
#include <mpi/communicator.hpp>

#include <complex>
#include <string>
#include <vector>

namespace triqs::mc_tools {

  template <DoubleOrComplex MCSignType> std::vector<std::string> measure_set<MCSignType>::names() const {
    std::vector<std::string> res;
    for (auto &[name, m] : measures_) {
      res.push_back(name);
      auto tmp_vec = m.names();
      res.insert(res.end(), tmp_vec.begin(), tmp_vec.end());
    }
    return res;
  }

  template <DoubleOrComplex MCSignType> void measure_set<MCSignType>::collect_results(const mpi::communicator &c) {
    for (auto &[name, m] : measures_) m.collect_results(c);
  }

  template <DoubleOrComplex MCSignType> std::string measure_set<MCSignType>::report() const {
    std::string res;
    for (auto &[name, m] : measures_) {
      auto str = m.report();
      if (!str.empty()) res += str;
    }
    return res;
  }

  template <DoubleOrComplex MCSignType> std::string measure_set<MCSignType>::get_timings(std::string const &prefix) const {
    std::string res;
    for (auto const &[name, m] : measures_) { res += m.get_timings(name, prefix); }
    return res;
  }

  // Explicit template instantiations.
  template class measure_set<double>;
  template class measure_set<std::complex<double>>;

} // namespace triqs::mc_tools
