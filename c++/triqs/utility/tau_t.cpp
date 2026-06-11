// Copyright (c) 2017--present, The Simons Foundation
// This file is part of TRIQS and is licensed under the terms of GPLv3 or later.
// SPDX-License-Identifier: GPL-3.0-or-later
// See LICENSE in the root of this distribution for details.

#include "./tau_t.hpp"
#include <cmath>

namespace triqs::utility {

  std::pair<double, double> cyclic_difference(double tau1, double tau2) {
    double dtau  = tau1 - tau2;
    int nshifts  = static_cast<int>(std::floor(dtau / tau_t::get_beta_value()));
    double sign  = (nshifts % 2 == 0) ? 1.0 : -1.0;
    double value = dtau - nshifts * tau_t::get_beta_value();
    return {sign, value};
  }

} // namespace triqs::utility
