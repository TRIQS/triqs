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
#include "./mc_move_set.hpp"

#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <itertools/itertools.hpp>
#include <mpi/communicator.hpp>

#include <algorithm>
#include <cmath>
#include <complex>
#include <map>
#include <numeric>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace triqs::mc_tools {

  template <DoubleOrComplex MCSignType> double move_set<MCSignType>::attempt() {
    assert(std::abs(acc_probs_.back() - 1.0) < 1e-13);

    // choose a move
    current_ = std::ranges::lower_bound(acc_probs_, rng_()) - acc_probs_.begin();
    assert(current_ < moves_.size());

    // attempt the move and return the absolute value of the acceptance ratio
    return check_ratio(moves_[current_].attempt());
  }

  template <DoubleOrComplex MCSignType> void move_set<MCSignType>::clear_statistics() {
    for (auto &m : moves_) m.clear_statistics();
  }

  template <DoubleOrComplex MCSignType> void move_set<MCSignType>::collect_statistics(mpi::communicator const &c) {
    for (auto &m : moves_) m.collect_statistics(c);
  }

  template <DoubleOrComplex MCSignType> void move_set<MCSignType>::calibrate(mpi::communicator const &c) {
    for (auto &m : moves_) m.calibrate(c);
  }

  template <DoubleOrComplex MCSignType> std::map<std::string, double> move_set<MCSignType>::get_acceptance_rates() const {
    std::map<std::string, double> res;
    for (auto const &[m, name] : itertools::zip(moves_, names_)) {
      res.insert({name, m.acceptance_rate()});
      auto tmp_map = m.get_acceptance_rates();
      res.insert(tmp_map.begin(), tmp_map.end());
    }
    return res;
  }

  template <DoubleOrComplex MCSignType> std::string move_set<MCSignType>::get_statistics(std::string const &prefix) const {
    std::string str;
    for (auto const &[m, name] : itertools::zip(moves_, names_)) { str += m.get_statistics(name, prefix); }
    return str;
  }

  template <DoubleOrComplex MCSignType> void move_set<MCSignType>::initialize() {
    // initialize is called in add, so we need to resize the vectors
    probs_.resize(weights_.size());
    acc_probs_.resize(weights_.size());

    // normalize weights to get probabilities
    auto norm = std::accumulate(weights_.begin(), weights_.end(), 0.0);
    std::transform(weights_.begin(), weights_.end(), probs_.begin(), [norm](auto w) { return w / norm; });

    // partial sum probabilities to get accumulated probabilities
    std::partial_sum(probs_.begin(), probs_.end(), acc_probs_.begin());
  }

  template <DoubleOrComplex MCSignType> double move_set<MCSignType>::check_ratio(MCSignType ratio) {
    // handle infinities in case of double MC weights
    if constexpr (std::is_same_v<MCSignType, double>) {
      if (std::isinf(ratio)) {
        attempt_sign_ = (std::signbit(ratio) ? -1 : 1);
        return 100;
      }
    }

    // throw an exception, if we the absolute ratio is still non-finite
    const auto abs_ratio = std::abs(ratio);
    if (!std::isfinite(abs_ratio)) {
      const auto cplx_ratio = std::complex<double>{ratio};
      throw std::runtime_error(fmt::format("Error in move_set::check_ratio: Non-finite absolute ratio in move {}: ({},{})", names_[current_],
                                           std::real(cplx_ratio), std::imag(cplx_ratio)));
    }

    // set the sign of the current attempt (why can't we always use ratio / abs_ratio?)
    attempt_sign_ = (abs_ratio > 1e-14 ? ratio / abs_ratio : 1);
    return abs_ratio;
  }

  // Explicit template instantiations.
  template class move_set<double>;
  template class move_set<std::complex<double>>;

} // namespace triqs::mc_tools
