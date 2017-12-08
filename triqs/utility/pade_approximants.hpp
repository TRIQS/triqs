
/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet, I. Krivenko
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

#ifndef TRIQS_UTILITY_PADE_APPROXIMANTS_H
#define TRIQS_UTILITY_PADE_APPROXIMANTS_H

#include "pade_approximants.hpp"
#include <triqs/utility/exceptions.hpp>
#include <triqs/arrays.hpp>
#include <gmpxx.h>

namespace triqs { namespace utility {

typedef std::complex<double> dcomplex;

// This implementation is based on a Fortran code written by
// A. Poteryaev <Alexander.Poteryaev _at_ cpht.polytechnique.fr>
//
// The original algorithm is described in
// H. J. Vidberg, J. W. Serene. J. Low Temp. Phys. 29, 3-4, 179 (1977)

struct gmp_complex {
  mpf_class re, im;
  gmp_complex operator* (const gmp_complex &rhs){ return { rhs.re*re-rhs.im*im, rhs.re*im+rhs.im*re }; }
  friend gmp_complex inverse (const gmp_complex &rhs){
   mpf_class d=rhs.re*rhs.re + rhs.im*rhs.im;
   if(d == 0) TRIQS_RUNTIME_ERROR << "pade_approximant: GMP division by zero";
   return { rhs.re/d, -rhs.im/d};
  }
  gmp_complex operator/ (const gmp_complex &rhs){ return (*this)*inverse(rhs); }
  gmp_complex operator+ (const gmp_complex &rhs){ return { rhs.re + re, rhs.im + im }; }
  gmp_complex operator- (const gmp_complex &rhs){ return { re - rhs.re, im - rhs.im }; }
  friend mpf_class real(const gmp_complex &rhs) { return rhs.re; }
  friend mpf_class imag(const gmp_complex &rhs) { return rhs.im; }
  gmp_complex& operator= (const std::complex<double> &rhs) { re = real(rhs); im = imag(rhs); return *this; }
  friend std::ostream & operator << (std::ostream & out,gmp_complex const & r) { return out << " gmp_complex("<<r.re<<","<<r.im<<")"<<std::endl ;}
};

class pade_approximant {

 arrays::vector<dcomplex> z_in; // Input complex frequency points
 arrays::vector<dcomplex> a; // Pade coefficients

 public:

 static const int GMP_default_prec = 256; // Precision of GMP floats to use during a Pade coefficients calculation.

 pade_approximant(const arrays::vector<dcomplex> & z_in_, const arrays::vector<dcomplex> & u_in):
   z_in(z_in_), a(z_in.size()) {

  int N = z_in.size();

  // Change the default precision of GMP floats.
  unsigned long old_prec = mpf_get_default_prec();
  mpf_set_default_prec(GMP_default_prec);  // How do we determine it?

  arrays::array<gmp_complex,2> g(N,N);
  gmp_complex MP_0 = {0.0, 0.0};
  g() = MP_0;
  for (int f = 0; f<N; ++f) { g(0,f) = u_in(f); };

  gmp_complex MP_1 = {1.0, 0.0};

  for(int p=1; p<N; ++p)
    for(int j=p; j<N; ++j) {
      gmp_complex x = g(p-1,p-1)/g(p-1,j) - MP_1;
      gmp_complex y; y = z_in(j)-z_in(p-1);
      g(p,j) = x/y;
    }

  for(int j=0; j<N; ++j) {
    gmp_complex gj = g(j,j);
    a(j) = dcomplex(real(gj).get_d(), imag(gj).get_d());
  }

  // Restore the precision.
  mpf_set_default_prec(old_prec);

 }

 // give the value of the pade continued fraction at complex number e
 dcomplex operator()(dcomplex e) const {

   dcomplex A1(0);
   dcomplex A2 = a(0);
   dcomplex B1(1.0), B2(1.0);

   int N = a.size();
   for(int i=0; i<=N-2; ++i){
     dcomplex Anew = A2 + (e - z_in(i))*a(i+1)*A1;
     dcomplex Bnew = B2 + (e - z_in(i))*a(i+1)*B1;
     A1 = A2/Bnew; A2 = Anew/Bnew;
     B1 = B2/Bnew; B2 = 1.0;
   }

   return A2/B2;

 }

};

}}

#endif
