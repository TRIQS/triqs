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
#include "../../gfs.hpp"
#include <triqs/utility/legendre.hpp>

namespace triqs { namespace gfs {

 using arrays::array;

 //-------------------------------------------------------
 // For Legendre functions
 // ------------------------------------------------------

 // compute a tail from the Legendre GF
 // this is Eq. 8 of our paper
 array<dcomplex, 3> get_tail(gf_const_view<legendre> gl, int order) {

   auto _ = ellipsis{};
   auto sh = gl.data().shape();
   sh[0] = order;
   array<dcomplex, 3> t{sh};
   t() = 0.0;

   for (int p=0; p< order; p++)
     for (auto l : gl.mesh())
       t(p, _) += (triqs::utility::legendre_t(l.index(),p)/std::pow(gl.domain().beta,p)) * gl[l];

   return t;
 }

 // Impose a discontinuity G(\tau=0)-G(\tau=\beta)
 void enforce_discontinuity(gf_view<legendre> & gl, arrays::array_view<double,2> disc) {

   double norm = 0.0;
   arrays::vector<double> t(gl.data().shape()[0]);
   for (int i=0; i<t.size(); ++i) {
     t(i) = triqs::utility::legendre_t(i,1) / gl.domain().beta;
     norm += t(i)*t(i);
   }

   arrays::array<dcomplex, 2> corr(disc.shape());
   corr() = 0;
   for (auto const &l : gl.mesh()) corr += t(l.index()) * gl[l];

   auto _ = arrays::range{};
   for (auto const& l : gl.mesh()) gl.data()(l.index(), _, _) += (disc - corr) * t(l.index()) / norm;
 }


}}
