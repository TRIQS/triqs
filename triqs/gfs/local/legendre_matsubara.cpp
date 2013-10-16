
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

#include "legendre_matsubara.hpp"
#include "fourier_matsubara.hpp"
#include "functions.hpp"
#include <triqs/utility/legendre.hpp>

using namespace triqs::utility;

namespace triqs { namespace gfs {

void legendre_matsubara_direct(gf_view<imfreq> & gw, gf_view<legendre> const & gl) {

  gw() = 0.0;
  triqs::arrays::range R;

  // Use the transformation matrix
  for (auto om: gw.mesh()) {
    for (auto l: gl.mesh()) {
      gw[om] += legendre_T(om.index(),l.index()) * gl[l];
    }
  }

  gw.singularity() = get_tail(gl, gw.singularity().size(), gw.singularity().order_min());

}

void legendre_matsubara_inverse (gf_view<legendre> & gl, gf_view<imfreq> const & gw) {

  gl() = 0.0;

  // Construct a temporary imaginary-time Green's function gt
  // I set Nt time bins. This is ugly, one day we must code the direct
  // transformation without going through imaginary time
  int Nt = 50000;
  auto gt = gf<imtime>{ {gw.domain(),Nt, half_bins}, gw.data().shape().front_pop() };

  // We first transform to imaginary time because it's been coded with the knowledge of the tails
  gt() = lazy_inverse_fourier(gw);
  legendre_matsubara_inverse(gl, gt());

}


void legendre_matsubara_direct (gf_view<imtime> & gt, gf_view<legendre> const & gl) {

  gt() = 0.0;
  legendre_generator L;

  for (auto t : gt.mesh()) {
    L.reset( 2*t/gt.domain().beta-1 );
    for (auto l : gl.mesh()) {
      gt[t] += sqrt(2*l.index()+1) / gt.domain().beta * gl[l] * L.next();
    }
  }

  gt.singularity() = get_tail(gl, gt.singularity().size(), gt.singularity().order_min());

}

void legendre_matsubara_inverse (gf_view<legendre> & gl, gf_view<imtime> const & gt) {

  gl() = 0.0;
  legendre_generator L;

  // Do the integral over imaginary time
  for (auto t : gt.mesh()) {
    L.reset( 2*t/gt.domain().beta-1 );
    for (auto l : gl.mesh()) {
      gl[l] += sqrt(2*l.index()+1) * L.next() * gt[t];
    }
  }
  gl.data() *= gt.mesh().delta();

}


gf_keeper<tags::legendre,legendre> lazy_legendre_imfreq (gf_view<legendre> const & gl) { return gl; }
gf_keeper<tags::legendre,legendre> lazy_legendre_imtime (gf_view<legendre> const & gl) { return gl; }
gf_keeper<tags::legendre,imfreq>   lazy_imfreq_legendre (gf_view<imfreq> const & gw) { return gw; }
gf_keeper<tags::legendre,imtime>   lazy_imtime_legendre (gf_view<imtime> const & gt) { return gt; }

void triqs_gf_view_assign_delegation( gf_view<imfreq> &gw, gf_keeper<tags::legendre,legendre> const & L) {
  legendre_matsubara_direct(gw, L.g);
}
void triqs_gf_view_assign_delegation( gf_view<imtime> &gt, gf_keeper<tags::legendre,legendre> const & L) {
  legendre_matsubara_direct(gt, L.g);
}
void triqs_gf_view_assign_delegation( gf_view<legendre> &gl, gf_keeper<tags::legendre,imfreq> const & L) {
  legendre_matsubara_inverse(gl, L.g);
}
void triqs_gf_view_assign_delegation( gf_view<legendre> &gl, gf_keeper<tags::legendre,imtime> const & L) {
  legendre_matsubara_inverse(gl, L.g);
}

}}
