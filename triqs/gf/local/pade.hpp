
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

#ifndef TRIQS_GF_LOCAL_PADE_H
#define TRIQS_GF_LOCAL_PADE_H

#include <triqs/gf/imfreq.hpp>
#include <triqs/gf/refreq.hpp>

namespace triqs { namespace gf {

  void pade (gf_view<refreq> &gr, gf_view<imfreq> const &gw, int n_points, double freq_offset);

  //namespace tags { struct pade{}; }

  //gf_keeper<tags::pade,imfreq> lazy_pade(gf_view<imfreq> const & gw);
  //void triqs_gf_view_assign_delegation(gf_view<refreq> &gr, gf_keeper<tags::pade,imfreq> const & L);

}}

#endif
