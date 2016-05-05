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
#include "../../gfs.hpp"
//#include "legendre_matsubara.hpp"
//#include "fourier_matsubara.hpp"
//#include "../functions/functions.hpp"
#include <triqs/utility/legendre.hpp>

using namespace triqs::utility;

namespace triqs {
namespace gfs {

 // ----------------------------

 void legendre_matsubara_direct(gf_view<imfreq> gw, gf_const_view<legendre> gl) {

  gw() = 0.0;
  triqs::arrays::range R;

  // Use the transformation matrix
  for (auto om : gw.mesh()) {
   for (auto l : gl.mesh()) {
    gw[om] += legendre_T(om.index(), l.index()) * gl[l];
   }
  }

  gw.singularity() = get_tail(gl, gw.singularity().n_valid_orders(), gw.singularity().order_min());
 }

 // ----------------------------

 void legendre_matsubara_direct(gf_view<imtime> gt, gf_const_view<legendre> gl) {

  gt() = 0.0;
  legendre_generator L;

  for (auto t : gt.mesh()) {
   L.reset(2 * t / gt.domain().beta - 1);
   for (auto l : gl.mesh()) {
    gt[t] += sqrt(2 * l.index() + 1) / gt.domain().beta * gl[l] * L.next();
   }
  }

  gt.singularity() = get_tail(gl, gt.singularity().n_valid_orders(), gt.singularity().order_min());
 }

 // ----------------------------

 void legendre_matsubara_inverse(gf_view<legendre> gl, gf_const_view<imtime> gt) {

  gl() = 0.0;
  legendre_generator L;
  auto N = gt.mesh().size() - 1;
  double coef;

  // Do the integral over imaginary time
  for (auto t : gt.mesh()) {
   if (t.index()==0 || t.index()==N) coef = 0.5;
   else coef = 1.0;
   L.reset(2 * t / gt.domain().beta - 1);
   for (auto l : gl.mesh()) {
    gl[l] += coef * sqrt(2 * l.index() + 1) * L.next() * gt[t];
   }
  }
  gl.data() *= gt.mesh().delta();
 }

 // ----------------------------

 void legendre_matsubara_inverse(gf_view<legendre> gl, gf_const_view<imfreq> gw) {

  gl() = 0.0;

  // Construct a temporary imaginary-time Green's function gt
  // I set Nt time bins. This is ugly, one day we must code the direct
  // transformation without going through imaginary time
  int Nt = 50000;
  auto gt = gf<imtime>{{gw.domain(), Nt}, gw.data().shape().front_pop()};

  // We first transform to imaginary time because it's been coded with the knowledge of the tails
  gt() = inverse_fourier(gw);
  legendre_matsubara_inverse(gl, gt());
 }


 void triqs_gf_view_assign_delegation(gf_view<imfreq> gw, gf_keeper<tags::legendre, legendre> const& L) {
  legendre_matsubara_direct(gw, L.g);
 }
 void triqs_gf_view_assign_delegation(gf_view<imtime> gt, gf_keeper<tags::legendre, legendre> const& L) {
  legendre_matsubara_direct(gt, L.g);
 }
 void triqs_gf_view_assign_delegation(gf_view<legendre> gl, gf_keeper<tags::legendre, imfreq> const& L) {
  legendre_matsubara_inverse(gl, L.g);
 }
 void triqs_gf_view_assign_delegation(gf_view<legendre> gl, gf_keeper<tags::legendre, imtime> const& L) {
  legendre_matsubara_inverse(gl, L.g);
 }
}
}
