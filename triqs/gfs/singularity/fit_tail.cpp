//#define TRIQS_EXCEPTION_SHOW_CPP_TRACE
//#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include "./fit_tail.hpp"
namespace triqs { namespace gfs {  

 __tail<matrix_valued> fit_real_tail_impl(gf_view<imfreq> gf, __tail_const_view<matrix_valued> known_moments, int max_moment, int n_min, int n_max) {

  // precondition : check that n_max is not too large
  n_max = std::min(n_max, int(gf.mesh().last_index()));
  n_min = std::max(n_min, int(gf.mesh().first_index()));

  const int known_moments_omin = known_moments.backwd_omin();
  const int known_moments_omax = known_moments.largest_non_nan() - 0;
  const int known_moments_size = known_moments_omax - known_moments_omin + 0;

  //tail res(gf.target_shape());
  //if (known_moments_size)
  // for (int i = known_moments_omin; i <= known_moments_omax; i++) res(i) = known_moments(i);
  __tail<matrix_valued> res = known_moments;

  // if known_moments_size==0, the lowest order to be obtained from the fit is determined by order_min in known_moments
  // if known_moments_size==0, the lowest order is the one following order_max() in known_moments

  int n_unknown_moments = (max_moment-known_moments_omin+1) - known_moments_size;

  //std::cout  << known_moments_omax << " "<< known_moments_omin <<" "<< known_moments_size << std::endl;
  //std::cout << " n_unknown_moments " << n_unknown_moments << std::endl;

  if (n_unknown_moments < 1) return known_moments;

  // get the number of even unknown moments: it is n_unknown_moments/2+1 if the first
  // moment is even and max_moment is odd; n_unknown_moments/2 otherwise
  int omin = known_moments_size == 0 ? known_moments_omin : known_moments_omax + 0; // smallest unknown moment
  int omin_even = omin % 2 == 0 ? omin : omin + 1;
  int omin_odd = omin % 2 != 0 ? omin : omin + 1;
  int size_even = n_unknown_moments / 2;
  if (n_unknown_moments % 2 != 0 && omin % 2 == 0) size_even += 1;
  int size_odd = n_unknown_moments - size_even;

  int size1 = n_max - n_min + 1;
  if (size1 < 0) TRIQS_RUNTIME_ERROR << "n_max - n_min + 1 <0";
  // size2 is the number of moments

  arrays::matrix<double> A(size1, std::max(size_even, size_odd), FORTRAN_LAYOUT);
  arrays::matrix<double> B(size1, 1, FORTRAN_LAYOUT);
  arrays::vector<double> S(std::max(size_even, size_odd));
  const double rcond = 0.0;
  int rank;

  for (int i = 0; i < gf.target_shape()[0]; i++) {
   for (int j = 0; j < gf.target_shape()[1]; j++) {

    // fit the odd moments
    S.resize(size_odd);
    A.resize(size1,size_odd); //when resizing, gelss segfaults
    for (int k = 0; k < size1; k++) {
     auto n = n_min + k;
     auto iw = std::complex<double>(gf.mesh().index_to_point(n));

     B(k, 0) = imag(gf.data()(gf.mesh().index_to_linear(n), i, j));
     // subtract known tail if present
     if (known_moments_size > 0)
      B(k, 0) -= imag(evaluate(slice_target_sing(known_moments, arrays::range(i, i + 1), arrays::range(j, j + 1)), iw)(0, 0));

     for (int l = 0; l < size_odd; l++) {
      int order = omin_odd + 2 * l;
      A(k, l) = imag(pow(iw, -1.0 * order)); // set design matrix for odd moments
     }
    }

    arrays::lapack::gelss(A, B, S, rcond, rank);
    for (int m = 0; m < size_odd; m++) {
     res(omin_odd + 2 * m)(i, j) = B(m, 0);
    }

    // fit the even moments
    S.resize(size_even);
    A.resize(size1,size_even); //when resizing, gelss segfaults
    for (int k = 0; k < size1; k++) {
     auto n = n_min + k;
     auto iw = std::complex<double>(gf.mesh().index_to_point(n));

     B(k, 0) = real(gf.data()(gf.mesh().index_to_linear(n), i, j));
     // subtract known tail if present
     if (known_moments_size > 0)
      B(k, 0) -= real(evaluate(slice_target_sing(known_moments, arrays::range(i, i + 1), arrays::range(j, j + 1)), iw)(0, 0));

     for (int l = 0; l < size_even; l++) {
      int order = omin_even + 2 * l;
      A(k, l) = real(pow(iw, -1.0 * order)); // set design matrix for odd moments
     }
    }

    arrays::lapack::gelss(A, B, S, rcond, rank);
    for (int m = 0; m < size_even; m++) {
     res(omin_even + 2 * m)(i, j) = B(m, 0);
    }
   }
  }
  return res; // return tail
 }

 __tail<matrix_valued> fit_complex_tail_impl(gf_view<imfreq> gf, __tail_const_view<matrix_valued> known_moments, int max_moment, int n_min, int n_max) {

  // precondition : check that n_max is not too large
  n_max = std::min(n_max, int(gf.mesh().last_index()));
  n_min = std::max(n_min, int(gf.mesh().first_index()));

  const int known_moments_omin = known_moments.backwd_omin();
  const int known_moments_omax = known_moments.largest_non_nan() -0;
  const int known_moments_size = known_moments_omax - known_moments_omin + 0;

  __tail<matrix_valued> res(gf.target_shape());
  if (known_moments_size)
   for (int i = known_moments_omin; i <= known_moments_omax; i++) res(i) = known_moments(i);

  int n_unknown_moments = (max_moment-known_moments_omin+1) - known_moments_size;
  if (n_unknown_moments < 1) return known_moments;

  // if known_moments_size==0, the lowest order to be obtained from the fit is determined by order_min() in known_moments
  // if known_moments_size!=0, the lowest order is the one following order_max() in known_moments
  int omin = known_moments_size == 0 ? known_moments_omin : known_moments_omax + 0;
  int n_freq = n_max - n_min + 1;
  if (n_freq < 0) TRIQS_RUNTIME_ERROR << "n_max - n_min + 1 <0";

  arrays::matrix<double> A(n_freq, n_unknown_moments, FORTRAN_LAYOUT);
  arrays::matrix<double> B(n_freq, 1, FORTRAN_LAYOUT);
  arrays::vector<double> S(n_unknown_moments);
  const double rcond = 0.0;
  int rank;

  // fit both real and imaginary parts at the same time
  for (int i = 0; i < gf.target_shape()[0]; i++) {
   for (int j = 0; j < gf.target_shape()[1]; j++) {

    // IMAGINARY PART
    // k is a label for the matsubara frequency
    for (int k = 0; k < n_freq; k++) {
     auto n = n_min + k;
     auto iw = std::complex<double>(gf.mesh().index_to_point(n));

     // construct data to be fitted - subtract known tail if present
     B(k, 0) = imag(gf.data()(gf.mesh().index_to_linear(n), i, j));
     if (known_moments_size > 0)
      B(k, 0) -= imag(evaluate(slice_target_sing(known_moments, arrays::range(i, i + 1), arrays::range(j, j + 1)), iw)(0, 0));

     // set design matrix
     // if the order is odd the fit yields the real coefficient of the moment
     // if the order is even the fit yields the imaginary coefficient of the moment
     for (int l = 0; l < n_unknown_moments; l++) {
      int order = omin + l;
      A(k, l) = imag( (order%2==1 ? 1.0 : dcomplex{0,1})*pow(iw, -1.0 * order) );
     }
    }

    arrays::lapack::gelss(A, B, S, rcond, rank);
    for (int m = 0; m < n_unknown_moments; m++) {
     res(omin+m)(i, j) = ((omin+m)%2==1 ? B(m,0) : dcomplex{0,1}*B(m, 0));
    }

    // REAL PART
    // k is a label for the matsubara frequency
    for (int k = 0; k < n_freq; k++) {
     auto n = n_min + k;
     auto iw = std::complex<double>(gf.mesh().index_to_point(n));

     // construct data to be fitted - subtract known tail if present
     B(k, 0) = real(gf.data()(gf.mesh().index_to_linear(n), i, j));
     if (known_moments_size > 0)
      B(k, 0) -= real(evaluate(slice_target_sing(known_moments, arrays::range(i, i + 1), arrays::range(j, j + 1)), iw)(0, 0));

     // set design matrix
     // if the order is even the fit yields the real coefficient of the moment
     // if the order is odd the fit yields the imaginary coefficient of the moment
     for (int l = 0; l < n_unknown_moments; l++) {
      int order = omin + l;
      A(k, l) = real( (order%2==0 ? 1.0 : dcomplex{0,1})*pow(iw, -1.0 * order) );
     }
    }

    arrays::lapack::gelss(A, B, S, rcond, rank);
    for (int m = 0; m < n_unknown_moments; m++) {
     res(omin+m)(i, j) += ((omin+m)%2==0 ? B(m, 0) : dcomplex{0,1}*B(m,0));
    }

   }
  }

  return res; // return tail
 }


 void fit_tail(gf_view<imfreq> gf, __tail_const_view<matrix_valued> known_moments, int max_moment, int n_min, int n_max, bool replace_by_fit) {

  // FIXME
  //if (gf.target_shape() != known_moments.shape()) TRIQS_RUNTIME_ERROR << "shape of tail does not match shape of gf";
  if (n_min <= 0) TRIQS_RUNTIME_ERROR << "n_min must be larger than 0";
  if (n_max <= n_min) TRIQS_RUNTIME_ERROR << "n_max must be larger than n_min";
  if (!is_gf_real_in_tau(gf, 1e-8)) TRIQS_RUNTIME_ERROR << "more arguments are needed to fit a complex gf";

  gf.singularity() = fit_real_tail_impl(gf, known_moments, max_moment, n_min, n_max);
  if (replace_by_fit) { // replace data in the fitting range by the values from the fitted tail
   for (auto iw : gf.mesh()) {
    if ((iw.n >= n_min) or (iw.n <= -n_min-(gf.mesh().domain().statistic==Fermion? 1 : 0))) gf[iw] = evaluate(gf.singularity(), iw);
   }
  }

 }

 void fit_tail(gf_view<imfreq> gf, __tail_const_view<matrix_valued> known_moments, int max_moment, int neg_n_min, int neg_n_max, int pos_n_min, int pos_n_max, bool replace_by_fit) {

  //FIXME 
  //if (gf.target_shape() != known_moments.shape()) TRIQS_RUNTIME_ERROR << "shape of tail does not match shape of gf";
  if (pos_n_min <= 0) TRIQS_RUNTIME_ERROR << "pos_n_min ("<<pos_n_min<<") must be larger than 0";
  if (pos_n_max <= pos_n_min) TRIQS_RUNTIME_ERROR << "pos_n_max ("<<pos_n_max<<") must be larger than pos_n_min ("<<pos_n_min<<")";
  if (neg_n_max >= 0) TRIQS_RUNTIME_ERROR << "neg_n_max ("<< neg_n_max <<") must be smaller than 0";
  if (neg_n_min >= neg_n_max) TRIQS_RUNTIME_ERROR << "neg_n_min ("<<neg_n_min <<") must be smaller than neg_n_max ("<<neg_n_max<<")";

  gf.singularity()  = fit_complex_tail_impl(gf, known_moments, max_moment, neg_n_min, neg_n_max);
  gf.singularity() += fit_complex_tail_impl(gf, known_moments, max_moment, pos_n_min, pos_n_max);
  gf.singularity() *= 0.5;
  if (replace_by_fit) { // replace data in the fitting range by the values from the fitted tail
   for (auto iw : gf.mesh()) {
    if ((iw.n >= neg_n_min and iw.n <= neg_n_max) or (iw.n >= pos_n_min and iw.n <= pos_n_max)) gf[iw] = evaluate(gf.singularity(), iw);
   }
  }
 }

 void fit_tail(block_gf_view<imfreq> block_gf, __tail_view<matrix_valued> known_moments, int max_moment, int n_min,
    int n_max, bool replace_by_fit ) {
   // for(auto &gf : block_gf) fit_tail(gf, known_moments, max_moment, n_min, n_max, replace_by_fit);
   for (int i = 0; i < block_gf.size(); i++)
    fit_tail(block_gf[i], known_moments, max_moment, n_min, n_max, replace_by_fit);
  }

  void fit_tail(gf_view<imfreq, scalar_valued> gf, __tail_const_view<scalar_valued> known_moments, int max_moment, int n_min,
                int n_max, bool replace_by_fit) {
   fit_tail(reinterpret_scalar_valued_gf_as_matrix_valued(gf), reinterpret_as_matrix_valued_sing(known_moments), max_moment, n_min,
            n_max, replace_by_fit);
  }

  void fit_tail(gf_view<imfreq, scalar_valued> gf, __tail_const_view<scalar_valued> known_moments, int max_moment, int neg_n_min,
                int neg_n_max, int pos_n_min, int pos_n_max, bool replace_by_fit) {
   fit_tail(reinterpret_scalar_valued_gf_as_matrix_valued(gf), reinterpret_as_matrix_valued_sing(known_moments), max_moment, neg_n_min,
            neg_n_max, pos_n_min, pos_n_max, replace_by_fit);
  }


 ///fit tail of tensor_valued Gf, rank 3
 array<__tail<scalar_valued>, 3> fit_tail(gf_const_view<imfreq, tensor_valued<3>> g, array_const_view<__tail<scalar_valued>,3> known_moments, int max_moment, int n_min, int n_max){

  gf<imfreq, scalar_valued> g_scal(g.mesh(), {});
  array<__tail<scalar_valued>, 3> tail(known_moments.shape());
  for(int u=0;u<known_moments.shape()[0];u++) 
   for(int v=0;v<known_moments.shape()[1];v++) 
    for(int w=0;w<known_moments.shape()[2];w++) {
     for(auto const & om : g_scal.mesh()) g_scal[om] = g[om](u,v,w);
     fit_tail(g_scal,known_moments(u,v,w), max_moment, n_min, n_max);
     tail(u,v,w) = g_scal.singularity();
    }
  return tail;
 }

 }} // namespace
