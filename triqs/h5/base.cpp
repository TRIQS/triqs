/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2014 by O. Parcollet
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
#include "./base.hpp"

namespace triqs {
  namespace h5 {

    //------------------------------------------------

    // dataspace from lengths and strides. Correct for the complex. strides must be >0
    dataspace dataspace_from_LS(int R, bool is_complex, hsize_t const *Ltot, hsize_t const *L, hsize_t const *S, hsize_t const *offset) {
      int rank = R + (is_complex ? 1 : 0);
      hsize_t totdimsf[rank], dimsf[rank], stridesf[rank], offsetf[rank]; // dataset dimensions
      for (size_t u = 0; u < R; ++u) {
        offsetf[u]  = (offset ? offset[u] : 0);
        dimsf[u]    = L[u];
        totdimsf[u] = Ltot[u];
        stridesf[u] = S[u];
      }
      if (is_complex) {
        offsetf[rank - 1]  = 0;
        dimsf[rank - 1]    = 2;
        totdimsf[rank - 1] = 2;
        stridesf[rank - 1] = 1;
      }

      dataspace ds = H5Screate_simple(rank, totdimsf, NULL);
      if (!ds.is_valid()) TRIQS_RUNTIME_ERROR << "Cannot create the dataset";

      herr_t err = H5Sselect_hyperslab(ds, H5S_SELECT_SET, offsetf, stridesf, dimsf, NULL);
      if (err < 0) TRIQS_RUNTIME_ERROR << "Cannot set hyperslab";

      return ds;
    }
  } // namespace h5
} // namespace triqs
