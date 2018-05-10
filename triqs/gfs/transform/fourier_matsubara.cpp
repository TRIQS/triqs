/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by M. Ferrero, O. Parcollet
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
#include <fftw3.h>

namespace triqs::gfs {

  namespace {

    dcomplex oneFermion(dcomplex a, double b, double tau, double beta) {
      return -a * (b >= 0 ? exp(-b * tau) / (1 + exp(-beta * b)) : exp(b * (beta - tau)) / (1 + exp(beta * b)));
    }

    dcomplex oneBoson(dcomplex a, double b, double tau, double beta) {
      return a * (b >= 0 ? exp(-b * tau) / (exp(-beta * b) - 1) : exp(b * (beta - tau)) / (1 - exp(b * beta)));
    }
  } // namespace

  //-------------------------------------

  array<dcomplex, 2> fit_derivatives(gf_const_view<imtime, tensor_valued<1>> gt) {
    int fit_order = 8;
    auto d_vec    = matrix_t(fit_order, n_others);
    for (int m : range(1, fit_order + 1)) d_vec(m - 1, _) = (gt[m] - gt[0]) / gt.mesh()[m];

    // Inverse of the Vandermonde matrix V_{m,j} = m^{j-1}
    // FIXME : add here teh python code that generated it
    auto V_inv = matrix_t{{8.000000000008853, -28.000000000055913, 56.000000000151815, -70.00000000021936, 56.00000000020795, -28.000000000115904,
                           8.00000000003683, -1.0000000000049232},
                          {-13.742857142879107, 62.10000000013776, -133.5333333337073, 172.75000000055635, -141.00000000052023, 71.43333333362274,
                           -20.600000000091182, 2.592857142869304},
                          {9.694444444465196, -50.98055555568518, 119.55000000035201, -161.8888888894241, 135.86111111160685, -70.12500000027573,
                           20.494444444530682, -2.6055555555670558},
                          {-3.6555555555654573, 21.23472222228416, -53.60000000016842, 76.34027777803497, -66.27777777801586, 35.03750000013273,
                           -10.422222222263619, 1.3430555555610917},
                          {0.7986111111137331, -4.97222222223865, 13.312500000044732, -19.888888888957325, 17.923611111174495, -9.750000000035401,
                           2.9652777777888164, -0.38888888889036743},
                          {-0.1013888888892789, 0.6638888888913357, -1.862500000006671, 2.902777777787997, -2.71527777778725, 1.5250000000052975,
                           -0.47638888889054154, 0.06388888888911043},
                          {0.0069444444444749, -0.047222222222413485, 0.13750000000052204, -0.22222222222302282, 0.2152777777785205,
                           -0.12500000000041575, 0.04027777777790756, -0.00555555555557296},
                          {-0.0001984126984136688, 0.0013888888888949882, -0.00416666666668333, 0.006944444444470023, -0.006944444444468193,
                           0.004166666666679969, -0.0013888888888930434, 0.00019841269841325577}};

    /*auto V_inv = matrix_t{
      {7.000000000000351, -21.00000000000213, 35.00000000000503, -35.00000000000608, 21.000000000003606, -7.000000000001135, 1.000000000000174},
      {-11.150000000000887, 43.95000000000505, -79.08333333334492, 82.00000000001403, -50.25000000000872, 16.98333333333621, -2.450000000000441},
      {7.088888888889711, -32.74166666667111, 64.83333333334329, -70.69444444445642, 44.666666666674445, -15.408333333336019, 2.255555555555966},
      {-2.312500000000358, 11.833333333335242, -25.395833333337578, 29.333333333338373, -19.270833333336668, 6.83333333333452,
       -1.0208333333335127},
      {0.409722222222303, -2.25000000000043, 5.145833333334286, -6.277777777778903, 4.3125000000007505, -1.5833333333336026,
       0.24305555555559613},
      {-0.037500000000009116, 0.21666666666671525, -0.5208333333334408, 0.6666666666667935, -0.47916666666675145, 0.18333333333336377,
       -0.029166666666671254},
      {0.001388888888889295, -0.008333333333335498, 0.02083333333333812, -0.027777777777783428, 0.020833333333337107, -0.008333333333334688,
       0.0013888888888890932}};
   */

    // Calculate the 2nd
    matrix_t g_vec = V_inv * d_vec;
    array<dcomplex, 2> m23(2, second_dim(g_vec));
    m23(0, _) = g_vec(0, _);
    m23(1, _) = -g_vec(1, _) * 2 / gt.mesh().delta();
    return m23;
  }

  /**
   * Makes a copy of the array in matrix, whose first dimension is the n-th dimension of a
   * and the second dimension are the flattening of the other dimensions, in the original order
   *
   * @param a : array
   * @param n : the dimension to preserve.
   *
   * @return : a matrix, copy of the data
   * */
  template <typename T, int R> matrix<T> flatten_2d(array_const_view<T, R> a, int n) {

    a.rebind(rotate_index_view(a, n)); // Swap relevant dim to front. The view is passed by value, we modify it.
    long nrows = first_dim(a);         // # rows of the result, i.e. n-th dim, which is now at 0.
    long ncols = a.size() / nrows;     // # columns of the result. Everything but n-th dim.
    matrix_t mat(first_dim(a), ncols); // result

    auto a_0 = a(0, ellipsis()); // FIXME for_each should take only the lengths ...
    for (long n : range(first_dim(a))) {
      if constexpr (R == 1)
        mat(n, 0) = a(n);
      else
        foreach (a_0, [&a, &mat, c = long{0} ](auto &&... i) mutable { mat(n, c++) = a(n, i...); })
          ;
    }
    return std::move(mat);
  }

  //template <typename T, int R> matrix<T> flatten_2d(array<T, R> const &a, int n) { return flatten_2d(a(), n); }

  //-------------------------------------

  template <int N, typename... Ms, typename Target> auto flatten_2d(gf_const_view<cartesian_product<Ms...>, Target> g) {
    return gf{std::get<N>(g.mesh()), flatten_2d(g.data(), N), {}};
  }

  template <int N, typename Var, typename Target> gf<Var, tensor_valued<1>> flatten_2d(gf_const_view<Var, Target> g) {
    static_assert(N == 0, "Internal error");
    return {g.mesh(), flatten_2d(g.data(), 0), {}};
  }

  //-------------------------------------

  using matrix_t = arrays::matrix<dcomplex>;

  // ----------------------------- General mechanism ------------------------------------------------------

  // We need a mechanism to tell which meshes are related by Fourier op, e.g. imtime, imfreq...
  // Or the error will happens as _fourier_impl is not implemented, which is not clear...
  // A little constexpr will do the job
  template <typename Var> constexpr int _fourier_category() {
    if constexpr (std::is_same_v<Var, imtime>) return 0;
    if constexpr (std::is_same_v<Var, imfreq>) return 0;
    if constexpr (std::is_same_v<Var, retime>) return 1;
    if constexpr (std::is_same_v<Var, refreq>) return 1;
    // continue with cyclic///
    return -1;
  }

  // this function just regroups the function, and calls the vector_valued gf core implementation
  template <int N, typename Var, typename Var2, typename Target, int R = get_n_variables<Var>::value - 1 + Target::rank>
  void _fourier_impl(gf_view<Var2, Target> gout, gf_const_view<Var, Target> gin, array_const_view<dcomplex, R + 1> moment_two_three = {}) {
    static_assert(_fourier_category<Var>() == _fourier_category<Var2>(), "There is no Fourier transform between these two meshes");
    auto const &mesh = std::get<N>(gout.mesh());
    auto lambda_res  = _fourier_impl_impl(mesh, flatten_2d<N>(gin), flatten_2d<N>(moment_two_three));
    if constexpr (R == 0)
      for (auto const &mp : mesh) gout[mp] = lambda_res(mp, 0);
    else {
      // inverse operation as flatten_2d, exactly
      auto g_data_view = rotate_index_view(gout.data(), N);
      auto a_0         = g_data_view(0, ellipsis());
      for (auto const &mp : mesh)
        foreach (a_0, [&g_data_view, &lambda_res, c = long{0} ](auto &&... i) mutable { g_data_view(mp.index(), i...) = lambda_res(mp, c++); })
          ;
    }
  }

  // implements the maker of the fourier transform
  template <int N, typename Var, typename Var2, typename Target, int R = get_n_variables<Var>::value - 1 + Target::rank>
  gf<Var2, Target> _make_fourier_impl(gf_const_view<Var, Target> gin, gf_mesh<Var2> const &mesh,
                                      array_const_view<dcomplex, R + 1> moment_two_three = {}) {
    static_assert(_fourier_category<Var>() == _fourier_category<Var2>(), "There is no Fourier transform between these two meshes");
    gf<Var2, Target> gout{mesh, gin.target_shape()};
    _fourier_impl(gw(), gin, moment_two_three);
    return gout;
  }

  // ------------------------ DIRECT TRANSFORM --------------------------------------------

  auto _fourier_impl_impl(gf_mesh<imfreq> const &iw_mesh, gf<imtime, tensor_valued<1>> &&gt, arrays::array_const_view<dcomplex, 2> moment_two_three) {
    if (moment_two_three.is_empty()) moment_two_three.rebind(fit_derivatives(gt));

    double beta = gt.mesh().domain().beta;
    auto L      = gt.mesh().size() - 1;
    if (L < 2 * (gw.mesh().last_index() + 1))
      TRIQS_RUNTIME_ERROR << "Fourier: The time mesh mush be at least twice as long as the number of positive frequencies :\n gt.mesh().size() =  "
                          << gt.mesh().size() << " gw.mesh().last_index()" << gw.mesh().last_index();

    long n_others = second_dim(gt.data());
    long nw       = gw.mesh().size();
    matrix_t _gout(L, n_others); // FIXME Why do we need this dimension to be one less than gt.mesh().size() ?
    matrix_t _gin(L + 1, n_others);

    bool is_fermion = (gw.domain().statistic == Fermion);
    double fact     = beta / L;
    dcomplex iomega = M_PI * 1_j / beta;

    auto _ = range();
    double b1, b2, b3;
    array<dcomplex, 1> m1, a1, a2, a3;

    if (is_fermion) {
      m1 = -(gt[0] + gt[L]);
      b1 = 0;
      b2 = 1;
      b3 = -1;
      a1 = m1 - m3;
      a2 = (m2 + m3) / 2;
      a3 = (m3 - m2) / 2;

      for (auto const &t : gt.mesh())
        for (long i = 0; i < n_others; ++i) {
          _gin(t.index(), i) = fact * exp(iomega * t)
             * (gt.data()(t.index(), i)
                - (oneFermion(a1(i), b1(i), t, beta) + oneFermion(a2(i), b2(i), t, beta) + oneFermion(a3(i), b3(i), t, beta)));
        }

    } else {
      m1 = -(gt[0] - gt[L]);
      b1 = -0.5;
      b2 = -1;
      b3 = 1;
      a1 = 4 * (m1 - m3) / 3;
      a2 = m3 - (m1 + m2) / 2;
      a3 = m1 / 6 + m2 / 2 + m3 / 3;

      for (auto const &t : gt.mesh())
        for (long i = 0; i < n_others; ++i) {
          _gin(t.index(), i) =
             fact * (gt.data()(t.index(), i) - (oneBoson(a1(i), b1(i) t, beta) + oneBoson(a2(i), b2(i), t, beta) + oneBoson(a3(i), b3(i), t, beta)));
        }
    }

    //g_in[L] = 0;

    auto g_in_fft  = reinterpret_cast<fftw_complex *>(_gin.data_start());
    auto g_out_fft = reinterpret_cast<fftw_complex *>(_gout.data_start());

    int rank   = 1;
    int dims[] = {L};
    // use the general routine that can do all the matrices at once.
    auto p = fftw_plan_many_dft(rank,                          // rank
                                dims,                          // the dimension
                                n_others,                      // how many FFT : here 1
                                g_in_fft,                      // in data
                                NULL,                          // embed : unused. Doc unclear ?
                                _gin.indexmap().strides()[0],  // stride of the in data
                                1,                             // in : shift for multi fft.
                                g_out_fft,                     // out data
                                NULL,                          // embed : unused. Doc unclear ?
                                _gout.indexmap().strides()[0], // stride of the out data
                                1,                             // out : shift for multi fft.
                                FFTW_BACKWARD, FFTW_ESTIMATE);

    fftw_execute(p);
    fftw_destroy_plan(p);

    return [ L, a1 = std::move(a1), a2 = std::move(a2), a3 = std::move(a3), b1, b2, b3, gout = std::move(gout) ](auto &&w, long i) {
      return gout((w.index() + L) % L, i) + a1(i) / (w - b1) + a2(i) / (w - b2) + a3(i) / (w - b3);
    };
  }

  //-------------------------------------

  // FIXME Generalize to matrix / tensor_valued gf
  // FIXME : doc : we assume that the fuction is at least 1/omega
  void inverse(gf_view<imtime, scalar_valued> gt, gf_const_view<imfreq, scalar_valued> gw) {
    if (gw.mesh().positive_only())
      TRIQS_RUNTIME_ERROR << "Fourier is only implemented for g(i omega_n) with full mesh (positive and negative frequencies)";

    auto[tail, error] = get_tail(gw, array<dcomplex, 1>{0});
    if (error > 1e-6) std::cerr << "WARNING: High frequency moments have an error greater than 1e-6.\n Error = " << error;
    if (error > 1e-3) TRIQS_RUNTIME_ERROR << "ERROR: High frequency moments have an error greater than 1e-3.\n  Error = " << error;
    if (first_dim(tail) < 3) TRIQS_RUNTIME_ERROR << "ERROR: Inverse Fourier implementation requires at least a proper 2nd high-frequency moment\n";

    // FIXME
    //if (std::abs(tail(0)) > 1e-10) TRIQS_RUNTIME_ERROR << "ERROR: Inverse Fourier implementation requires vanishing 0th moment\n  error is :" << std::abs(tail(0));

    double beta = gw.domain().beta;
    long L      = gt.mesh().size() - 1;
    if (L < 2 * (gw.mesh().last_index() + 1))
      TRIQS_RUNTIME_ERROR << "Inverse Fourier: The time mesh mush be at least twice as long as the freq mesh :\n gt.mesh().size() =  "
                          << gt.mesh().size() << " gw.mesh().last_index()" << gw.mesh().last_index();

    g_in.resize(L); // L>=2*(gw.mesh().last_index()+1) , we will fill the middle array with 0
    g_out.resize(L + 1);
    g_in() = 0;

    bool is_fermion = (gw.domain().statistic == Fermion);
    double fact     = 1.0 / beta;
    dcomplex iomega = M_PI * 1_j / beta;

    double b1, b2, b3;
    dcomplex a1, a2, a3;
    dcomplex m1 = tail(1), m2 = tail(2), m3 = (first_dim(tail) == 3) ? 0 : tail(3);

    if (is_fermion) {
      b1 = 0;
      b2 = 1;
      b3 = -1;
      a1 = m1 - m3;
      a2 = (m2 + m3) / 2;
      a3 = (m3 - m2) / 2;
    } else {
      b1 = -0.5;
      b2 = -1;
      b3 = 1;
      a1 = 4 * (m1 - m3) / 3;
      a2 = m3 - (m1 + m2) / 2;
      a3 = m1 / 6 + m2 / 2 + m3 / 3;
    }

    for (auto const &w : gw.mesh()) g_in[(w.index() + L) % L] = fact * (gw[w] - (a1 / (w - b1) + a2 / (w - b2) + a3 / (w - b3)));

    auto g_in_fft  = reinterpret_cast<fftw_complex *>(g_in.data_start());
    auto g_out_fft = reinterpret_cast<fftw_complex *>(g_out.data_start());

    // FIXME Factorize plan into worker
    auto p = fftw_plan_dft_1d(L, g_in_fft, g_out_fft, FFTW_FORWARD, FFTW_ESTIMATE); // in our convention backward is inverse FFT
    fftw_execute(p);
    fftw_destroy_plan(p);

    if (is_fermion) {
      for (auto const &t : gt.mesh()) {
        gt[t] = g_out(t.index()) * exp(-iomega * t) + oneFermion(a1, b1, t, beta) + oneFermion(a2, b2, t, beta) + oneFermion(a3, b3, t, beta);
      }
    } else {
      for (auto const &t : gt.mesh()) gt[t] = g_out(t.index()) + oneBoson(a1, b1, t, beta) + oneBoson(a2, b2, t, beta) + oneBoson(a3, b3, t, beta);
    }
    double pm = (is_fermion ? -1 : 1);
    gt[L]     = pm * (gt[0] + m1);
  }

  //template <typename Target, int R = Target::rank> void _fourier_impl(gf_view<imfreq, Target> gw, gf_const_view<imtime, Target> gt, arrays::array_const_view<dcomplex, R+1> moment_two_three = {}) {
  //auto lambda_res = _fourier_impl_impl(gw.mesh(), flatten_2d(gt), flatten_2d(moment_two_three));
  //if constexpr( R == 0 )
  //for(auto const & iw : iw_mesh) gw[iw] = lambda_res(iw, 0);;
  //else{
  //auto a_0 = gw[0];
  //for(auto const & iw : iw_mesh)
  //foreach (a_0, [&gw, &lambda_res, c = long{0}](auto &&... i) mutable { gw.data()(iw.index(), i...) = lambda_res(iw, c++); });
  //}
  //}

} // namespace triqs::gfs
