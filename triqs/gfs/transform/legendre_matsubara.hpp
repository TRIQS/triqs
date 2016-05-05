/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2014 by L. Boehnke, M. Ferrero, O. Parcollet
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

 namespace tags {
  struct legendre {};
 }

 inline gf_keeper<tags::legendre, legendre> legendre_to_imfreq(gf_const_view<legendre> gl) { return {gl}; }
 inline gf_keeper<tags::legendre, legendre> legendre_to_imtime(gf_const_view<legendre> gl) { return {gl}; }
 inline gf_keeper<tags::legendre, imfreq> imfreq_to_legendre(gf_const_view<imfreq> gw) { return {gw}; }
 inline gf_keeper<tags::legendre, imtime> imtime_to_legendre(gf_const_view<imtime> gt) { return {gt}; }

 void triqs_gf_view_assign_delegation(gf_view<imfreq> gw, gf_keeper<tags::legendre, legendre> const &L);
 void triqs_gf_view_assign_delegation(gf_view<imtime> gt, gf_keeper<tags::legendre, legendre> const &L);
 void triqs_gf_view_assign_delegation(gf_view<legendre> gl, gf_keeper<tags::legendre, imfreq> const &L);
 void triqs_gf_view_assign_delegation(gf_view<legendre> gl, gf_keeper<tags::legendre, imtime> const &L);
}
}
