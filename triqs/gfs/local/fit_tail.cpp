#include "./fit_tail.hpp"
namespace triqs { namespace gfs {  

 tail fit_tail_impl(gf_view<imfreq> gf, const tail_view known_moments, int n_moments, int n_min, int n_max) {

  tail res(get_target_shape(gf));
  if (known_moments.size())
   for (int i = known_moments.order_min(); i <= known_moments.order_max(); i++) res(i) = known_moments(i);

  // if known_moments.size()==0, the lowest order to be obtained from the fit is determined by order_min in known_moments
  // if known_moments.size()==0, the lowest order is the one following order_max in known_moments

  int n_unknown_moments = n_moments - known_moments.size();
  if (n_unknown_moments < 1) return known_moments;

  // get the number of even unknown moments: it is n_unknown_moments/2+1 if the first
  // moment is even and n_moments is odd; n_unknown_moments/2 otherwise
  int omin = known_moments.size() == 0 ? known_moments.order_min() : known_moments.order_max() + 1; // smallest unknown moment
  int omin_even = omin % 2 == 0 ? omin : omin + 1;
  int omin_odd = omin % 2 != 0 ? omin : omin + 1;
  int size_even = n_unknown_moments / 2;
  if (n_unknown_moments % 2 != 0 && omin % 2 == 0) size_even += 1;
  int size_odd = n_unknown_moments - size_even;

  int size1 = n_max - n_min + 1;
  // size2 is the number of moments

  arrays::matrix<double> A(size1, std::max(size_even, size_odd), FORTRAN_LAYOUT);
  arrays::matrix<double> B(size1, 1, FORTRAN_LAYOUT);
  arrays::vector<double> S(std::max(size_even, size_odd));
  const double rcond = 0.0;
  int rank;

  for (int i = 0; i < get_target_shape(gf)[0]; i++) {
   for (int j = 0; j < get_target_shape(gf)[1]; j++) {

    // fit the odd moments
    S.resize(size_odd);
    A.resize(size1,size_odd); //when resizing, gelss segfaults
    for (int k = 0; k < size1; k++) {
     auto n = n_min + k;
     auto iw = std::complex<double>(gf.mesh().index_to_point(n));

     B(k, 0) = imag(gf.data()(gf.mesh().index_to_linear(n), i, j));
     // subtract known tail if present
     if (known_moments.size() > 0)
      B(k, 0) -= imag(evaluate(slice_target(known_moments, arrays::range(i, i + 1), arrays::range(j, j + 1)), iw)(0, 0));

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
     if (known_moments.size() > 0)
      B(k, 0) -= real(evaluate(slice_target(known_moments, arrays::range(i, i + 1), arrays::range(j, j + 1)), iw)(0, 0));

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
  res.mask()()=n_moments;
  return res; // return tail
 }

 void fit_tail(gf_view<imfreq> gf, tail_view known_moments, int n_moments, int n_min, int n_max,
   bool replace_by_fit ) {
  if (get_target_shape(gf) != known_moments.shape()) TRIQS_RUNTIME_ERROR << "shape of tail does not match shape of gf";
  gf.singularity() = fit_tail_impl(gf, known_moments, n_moments, n_min, n_max);
  if (replace_by_fit) { // replace data in the fitting range by the values from the fitted tail
   int i = 0;
   for (auto iw : gf.mesh()) { // (arrays::range(n_min,n_max+1)) {
    if (i >= n_min) gf[iw] = evaluate(gf.singularity(),iw);
    i++;
   }
   }
  }

  void fit_tail(gf_view<block_index, gf<imfreq>> block_gf, tail_view known_moments, int n_moments, int n_min,
    int n_max, bool replace_by_fit ) {
   // for(auto &gf : block_gf) fit_tail(gf, known_moments, n_moments, n_min, n_max, replace_by_fit);
   for (int i = 0; i < block_gf.mesh().size(); i++)
    fit_tail(block_gf[i], known_moments, n_moments, n_min, n_max, replace_by_fit);
  }

  void fit_tail(gf_view<imfreq, scalar_valued> gf, tail_view known_moments, int n_moments, int n_min, int n_max, bool replace_by_fit ) {
   fit_tail(reinterpret_scalar_valued_gf_as_matrix_valued(gf), known_moments, n_moments, n_min, n_max, replace_by_fit );
  }

 }} // namespace
