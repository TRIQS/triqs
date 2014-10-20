/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#ifndef TRIQS_GFS_ALL_H
#define TRIQS_GFS_ALL_H

#define TRIQS_GF_INCLUDED

#include <triqs/gfs/block.hpp>
#include <triqs/gfs/imtime.hpp>
#include <triqs/gfs/imfreq.hpp>
#include <triqs/gfs/retime.hpp>
#include <triqs/gfs/refreq.hpp>
#include <triqs/gfs/legendre.hpp>
#include <triqs/gfs/bz.hpp>

// multivariable gf in C++14 only
#ifndef TRIQS_C11
#include <triqs/gfs/product.hpp>
#include <triqs/gfs/curry.hpp>
#include <triqs/gfs/m_tail.hpp>
#endif

#include <triqs/gfs/local/fourier_matsubara.hpp>
#include <triqs/gfs/local/fourier_real.hpp>
#include <triqs/gfs/local/legendre_matsubara.hpp>
#endif

