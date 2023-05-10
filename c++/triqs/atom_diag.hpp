/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2017 by O. Parcollet, T. Ayral.
 * Copyright (C) 2018-2019 by Simons Foundation
 *   author : O. Parcollet, H. Strand, P. Dumitrescu
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

#include "./atom_diag/atom_diag.hpp"
#include "./atom_diag/functions.hpp"
#include "./atom_diag/gf.hpp"

#ifdef C2PY_INCLUDED 
#include <c2py/c2py.hpp>
template <> static constexpr bool c2py::is_wrapped<triqs::atom_diag::atom_diag<true>> = true;
template <> static constexpr bool c2py::is_wrapped<triqs::atom_diag::atom_diag<false>> = true;
#endif


