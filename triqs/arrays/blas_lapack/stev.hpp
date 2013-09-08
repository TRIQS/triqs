/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by O. Parcollet
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
#ifndef TRIQS_ARRAYS_BLAS_LAPACK_DSTEV_H
#define TRIQS_ARRAYS_BLAS_LAPACK_DSTEV_H
#include "./tools.hpp"

namespace triqs { namespace arrays { namespace blas { 

 using namespace blas_lapack_tools;
 namespace f77 { // overload

  extern "C" { 
   void TRIQS_FORTRAN_MANGLING(dstev)(const char *, const int &N, const double [], const double [], const double [],  const int &, double [], int &);
  }

  inline void stev (char J, const int & N, double* D, double* E, double* Z,  int ldz, double * work, int & info)  { 
   TRIQS_FORTRAN_MANGLING(dstev)(&J,N,D,E,Z,ldz,work,info);
  }
 }

 struct tridiag_worker { 
  arrays::vector<double> D,E,W;
  matrix<double> Z;
  size_t s;
  tridiag_worker (size_t n){ s=0; resize(n);}
  void resize(size_t n) { if (s<n) {D.resize(n); E.resize(n); W.resize(2*n+1); Z.resize(n,n);} s = n; }
  template<typename VT> 
   void operator()(VT const & d, VT const & e) { 
    if (e.size() != d.size() -1) TRIQS_RUNTIME_ERROR << " Error in tridiagonal matrix diagonalization size mismatch"; 
    resize(d.size());
    D(range(0,s)) = d; 
    E(range(0,s-1)) = e;
    int info;
    f77::stev ('V',d.size(),D.data_start(),E.data_start(), Z.data_start(), Z.shape(0),W.data_start(), info);
    if (info !=0) TRIQS_RUNTIME_ERROR << " Error in tridiagonal matrix diagonalization "<< info;
   }
  arrays::vector_view<double> values() const { return D(range(0,s));}
  matrix_view<double> vectors() const { return Z(range(0,s),range(0,s));}
 };

}}}// namespace
#endif

