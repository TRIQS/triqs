/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2016 by M. Ferrero, O. Parcollet
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

namespace triqs {
namespace gfs {

 //-------------------------------------------------------
 // Density
 // ------------------------------------------------------

 arrays::matrix<dcomplex> density(gf_const_view<imfreq> g);
 dcomplex density(gf_const_view<imfreq, scalar_valued> g);

 arrays::matrix<dcomplex> density(gf_const_view<legendre> g);
}

namespace clef {
 TRIQS_CLEF_MAKE_FNT_LAZY(density);
}
}

