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
#include "./mc_measure.hpp"

#include <fmt/format.h>
#include <mpi/communicator.hpp>

#include <complex>
#include <string>

namespace triqs::mc_tools {

  template <DoubleOrComplex MCSignType> void measure<MCSignType>::collect_results(mpi::communicator const &c) {
    if (enable_timer_) timer_.start();
    ptr_->collect_results(c);
    if (enable_timer_) timer_.stop();
  }

  template <DoubleOrComplex MCSignType> std::string measure<MCSignType>::report() const {
    if (enable_report_) return ptr_->report();
    return {};
  }

  template <DoubleOrComplex MCSignType> std::string measure<MCSignType>::get_timings(std::string const &name, std::string const &prefix) const {
    if (is_measure_set_) {
      auto str = fmt::format("{}Measure set {}: Duration = {:.4f}\n", prefix, name, duration());
      return str + ptr_->ms_get_timings(prefix + "  ");
    } else {
      return fmt::format("{}Measure {}: Duration = {:.4f}\n", prefix, name, duration());
    }
  }

  template <DoubleOrComplex MCSignType> std::vector<std::string> measure<MCSignType>::names() const {
    if (is_measure_set_) return ptr_->ms_names();
    return {};
  }

  template <DoubleOrComplex MCSignType> double measure<MCSignType>::duration() const {
    if (enable_timer_) return static_cast<double>(timer_);
    return 0.0;
  }

  // Explicit template instantiations.
  template class measure<double>;
  template class measure<std::complex<double>>;

} // namespace triqs::mc_tools
