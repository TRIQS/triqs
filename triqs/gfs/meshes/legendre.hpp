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
#include "../domains/legendre.hpp"
#include "./discrete.hpp"

namespace triqs {
namespace gfs {

 struct legendre {};

 // mesh type and its factories
 template <> struct gf_mesh<legendre> : discrete_mesh<legendre_domain> {
  typedef discrete_mesh<legendre_domain> B;
  gf_mesh() = default;
  gf_mesh(double beta, statistic_enum S, size_t n_leg) : B(typename B::domain_t(beta, S, n_leg)) {}
 };
}
}

