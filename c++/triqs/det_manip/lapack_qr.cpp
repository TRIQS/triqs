// Copyright (c) 2024 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt

// Definition of the LAPACK `trtrs` f77 layer declared in det_manip/lapack_qr.hpp. Kept in a .cpp
// (compiled into libtriqs, which links LAPACK via nda::nda_c) so the raw LAPACK symbol is resolved
// inside the library and not in every translation unit that includes the header. This mirrors nda's
// own interface/cxx_interface.cpp and is the natural place to lift into nda later.

#include "./lapack_qr.hpp"

#include <complex>

namespace nda::lapack::f77 {

  // Reference-LAPACK Fortran symbols (lowercase + trailing underscore). The hidden CHARACTER-length
  // arguments are not needed for these length-1 character flags.
  extern "C" {
  void dtrtrs_(const char *uplo, const char *trans, const char *diag, const int *n, const int *nrhs, const double *a, const int *lda, double *b,
               const int *ldb, int *info);
  void ztrtrs_(const char *uplo, const char *trans, const char *diag, const int *n, const int *nrhs, const std::complex<double> *a, const int *lda,
               std::complex<double> *b, const int *ldb, int *info);
  }

  void trtrs(char uplo, char trans, char diag, int n, int nrhs, double const *a, int lda, double *b, int ldb, int &info) {
    dtrtrs_(&uplo, &trans, &diag, &n, &nrhs, a, &lda, b, &ldb, &info);
  }

  void trtrs(char uplo, char trans, char diag, int n, int nrhs, std::complex<double> const *a, int lda, std::complex<double> *b, int ldb, int &info) {
    ztrtrs_(&uplo, &trans, &diag, &n, &nrhs, a, &lda, b, &ldb, &info);
  }

} // namespace nda::lapack::f77
