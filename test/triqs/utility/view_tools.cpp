/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#include <triqs/gfs.hpp>
#include <triqs/test_tools/gfs.hpp>

using namespace triqs;
using namespace triqs::gfs;
using namespace std::complex_literals;

template <typename T> struct check {
  static_assert(std::is_same<typename T::view_type, view_or_type_t<T>>::value, "err");
  static_assert(std::is_same<typename T::regular_type, regular_t<T>>::value, "err");
};

TEST(ViewTools, TypeTraits) {
  check<array<int, 1>>();
  check<array_view<int, 1>>();
  check<matrix<int>>();
  check<matrix_view<int>>();
}

TEST(ViewTools, PosFreqView) {

  double beta  = 1;
  int n_iw     = 100;
  auto iw_mesh = gf_mesh<imfreq>{beta, Fermion, n_iw};

  // -- Build and Init Green function
  auto g_iw    = gf<imfreq, matrix_valued>{iw_mesh, {1, 1}};
  triqs::clef::placeholder<0> iw_;
  g_iw(iw_) << 1.0 / (iw_ + 1.0 + 1i);

  // -- Test positive_freq_view()
  auto g_iw_cv = gf_const_view(g_iw);

  auto g_iw_pfv = positive_freq_view(g_iw);
  auto g_iw_pfv_v = positive_freq_view(g_iw()); // From rvalue view
  //auto g_iw_pfv_v = positive_freq_view(gf{g_iw}); // From temporary gf: This should fail compilation
  auto g_iw_pfv_cv = positive_freq_view(g_iw_cv); // From lvalue view

  auto pos_freq_data_view = g_iw.data()(range(n_iw,2*n_iw),ellipsis());
  EXPECT_ARRAY_NEAR(pos_freq_data_view, g_iw_pfv.data());
  EXPECT_ARRAY_NEAR(pos_freq_data_view, g_iw_pfv_v.data());
  EXPECT_ARRAY_NEAR(pos_freq_data_view, g_iw_pfv_cv.data());
}

MAKE_MAIN;
