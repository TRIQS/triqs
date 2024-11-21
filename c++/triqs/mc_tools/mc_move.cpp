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
#include "./mc_move.hpp"

#include <fmt/format.h>
#include <mpi/mpi.hpp>

#include <complex>
#include <cstdint>
#include <string>

namespace triqs::mc_tools {

  template <DoubleOrComplex MCSignType> void move<MCSignType>::collect_statistics(mpi::communicator const &c) {
    std::uint64_t nacc_tot  = mpi::all_reduce(nacc_, c);
    std::uint64_t nprop_tot = mpi::all_reduce(nprop_, c);
    acc_rate_               = static_cast<double>(nacc_tot) / static_cast<double>(nprop_tot);
    ptr_->collect_statistics(c);
  }

  template <DoubleOrComplex MCSignType> void move<MCSignType>::clear_statistics() {
    nacc_     = 0;
    nprop_    = 0;
    acc_rate_ = -1;
    ptr_->ms_clear_statistics();
  }

  template <DoubleOrComplex MCSignType> std::string move<MCSignType>::get_statistics(std::string const &name, std::string const &prefix) const {
    if (is_move_set_) {
      auto str = fmt::format("{}Move set {}: Proposed = {}, Accepted = {}, Rate = {:.4f}\n", prefix, name, nprop_, nacc_, acc_rate_);
      return str + ptr_->ms_get_statistics(prefix + "  ");
    } else {
      return fmt::format("{}Move {}: Proposed = {}, Accepted = {}, Rate = {:.4f}\n", prefix, name, nprop_, nacc_, acc_rate_);
    }
  }

  // Explicit template instantiations.
  template class move<double>;
  template class move<std::complex<double>>;

} // namespace triqs::mc_tools
