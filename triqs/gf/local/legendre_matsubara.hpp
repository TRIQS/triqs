
/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by L. Boehnke, M. Ferrero, O. Parcollet
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
#ifndef TRIQS_GF_LOCAL_LEGENDRE_MATSU_H 
#define TRIQS_GF_LOCAL_LEGENDRE_MATSU_H

#include <triqs/gf/imfreq.hpp>
#include <triqs/gf/imtime.hpp>
#include <triqs/gf/legendre.hpp>

namespace triqs { namespace gf {

  void legendre_matsubara_direct (gf_view<imfreq> &gw, gf_view<legendre> const &gl);
  void legendre_matsubara_inverse (gf_view<legendre> &gl, gf_view<imfreq> const &gw);

  void legendre_matsubara_direct (gf_view<imtime> &gt, gf_view<legendre> const &gl);
  void legendre_matsubara_inverse (gf_view<legendre> &gl, gf_view<imtime> const &gt);

  namespace tags { struct legendre{}; }

  gf_keeper<tags::legendre,legendre> lazy_legendre_imfreq (gf_view<legendre> const & gl);
  gf_keeper<tags::legendre,legendre> lazy_legendre_imtime (gf_view<legendre> const & gl);
  gf_keeper<tags::legendre,imfreq>   lazy_imfreq_legendre (gf_view<imfreq> const & gw);
  gf_keeper<tags::legendre,imtime>   lazy_imtime_legendre (gf_view<imtime> const & gt);

  void triqs_gf_view_assign_delegation( gf_view<imfreq> &gw, gf_keeper<tags::legendre,legendre> const & L);
  void triqs_gf_view_assign_delegation( gf_view<imtime> &gt, gf_keeper<tags::legendre,legendre> const & L);
  void triqs_gf_view_assign_delegation( gf_view<legendre> &gl, gf_keeper<tags::legendre,imfreq> const & L);
  void triqs_gf_view_assign_delegation( gf_view<legendre> &gl, gf_keeper<tags::legendre,imtime> const & L);


}}
#endif
