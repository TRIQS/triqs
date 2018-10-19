/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2017 by O. Parcollet
 * Copyright (C) 2018 by Simons Foundation
 *   author : O. Parcollet, P. Dumitrescu, N. Wentzell
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
#pragma once

#include <complex>
#include <triqs/utility/itertools.hpp>
#include "f77/cxx_interface.hpp"
#include "tools.hpp"
#include "qcache.hpp"
#include "gesvd.hpp"

namespace triqs::arrays::lapack {

  using namespace blas_lapack_tools;

  /**
  * Calls gelss on a matrix or view
  * Takes care of making temporary copies if necessary
  */
  template <typename MTA, typename MTB, typename VCS>
  typename std::enable_if_t<is_blas_lapack_type<typename MTA::value_type>::value && is_blas_lapack_type<typename MTB::value_type>::value
                               && is_blas_lapack_type<typename VCS::value_type>::value,
                            int>
  gelss(MTA const &A, MTB &B, VCS &S, double rcond, int &rank) {

    int info;

    if (!A.memory_layout_is_fortran()) {
      auto A_FL = typename MTA::regular_type{A, FORTRAN_LAYOUT};
      info      = gelss(A_FL, B, S, rcond, rank);
      return info;
    }

    if (!B.memory_layout_is_fortran()) {
      auto B_FL = typename MTB::regular_type{B, FORTRAN_LAYOUT};
      info      = gelss(A, B_FL, S, rcond, rank);
      B()       = B_FL();
      return info;
    }

    // Copy A, since it is altered by gelss
    typename MTA::regular_type _A{A, FORTRAN_LAYOUT};

    reflexive_qcache<MTA> Ca(_A);
    reflexive_qcache<MTB> Cb(B);

    auto dm = std::min(get_n_rows(Ca()), get_n_cols(Ca()));
    if (S.size() < dm) S.resize(dm);

    reflexive_qcache<VCS> Cs(S);

    int nrhs = get_n_cols(Cb());

    if constexpr (std::is_same<typename MTA::value_type, double>::value) {

      // first call to get the optimal lwork
      typename MTA::value_type work1[1];
      f77::gelss(get_n_rows(Ca()), get_n_cols(Ca()), nrhs, Ca().data_start(), get_ld(Ca()), Cb().data_start(), get_ld(Cb()), Cs().data_start(), rcond,
                 rank, work1, -1, info);

      int lwork = r_round(work1[0]);
      arrays::vector<typename MTA::value_type> work(lwork);

      f77::gelss(get_n_rows(Ca()), get_n_cols(Ca()), nrhs, Ca().data_start(), get_ld(Ca()), Cb().data_start(), get_ld(Cb()), Cs().data_start(), rcond,
                 rank, work.data_start(), lwork, info);

    } else if constexpr (std::is_same<typename MTA::value_type, std::complex<double>>::value) {

      auto rwork = array<double, 1>(5 * dm);

      // first call to get the optimal lwork
      typename MTA::value_type work1[1];
      f77::gelss(get_n_rows(Ca()), get_n_cols(Ca()), nrhs, Ca().data_start(), get_ld(Ca()), Cb().data_start(), get_ld(Cb()), Cs().data_start(), rcond,
                 rank, work1, -1, rwork.data_start(), info);

      int lwork = r_round(work1[0]);
      arrays::vector<typename MTA::value_type> work(lwork);

      f77::gelss(get_n_rows(Ca()), get_n_cols(Ca()), nrhs, Ca().data_start(), get_ld(Ca()), Cb().data_start(), get_ld(Cb()), Cs().data_start(), rcond,
                 rank, work.data_start(), lwork, rwork.data_start(), info);
    } else {
      TRIQS_RUNTIME_ERROR << "Error in gelss : only implemented for value_type double and std::complex<double>";
    }

    if (info) TRIQS_RUNTIME_ERROR << "Error in gesvd : info = " << info;
    return info;
  }

  template <typename value_type> struct gelss_cache {

    // cf. Notation in https://math.stackexchange.com/questions/772039/how-does-the-svd-solve-the-least-squares-problem

    private:
    // Number of rows (M) and columns (N) of the Matrix A
    size_t M, N;

    // The matrix to be decomposed by SVD
    matrix<value_type> A;

    // The (pseudo) inverse of A, i.e. V * Diag(S_vec)^{-1} * UT, for the least square procedure
    matrix<value_type> V_x_InvS_x_UT;

    // The part of UT fixing the error of the LLS
    matrix<value_type> UT_NULL;

    // Vector containing the singular values
    vector<double> _S_vec;

    public:
    int n_var() const { return second_dim(A); }
    matrix<value_type> const &A_mat() const { return A; }
    vector<double> const &S_vec() const { return _S_vec; }

    gelss_cache(matrix_const_view<value_type> _A) : M(first_dim(_A)), N(second_dim(_A)), A(_A), _S_vec(std::min(M, N)) {

      if (N > M) TRIQS_RUNTIME_ERROR << "ERROR: Matrix A for linear least square procedure cannot have more columns than rows";

      matrix<value_type> A_FL{_A, FORTRAN_LAYOUT};
      matrix<value_type> U{M, M, FORTRAN_LAYOUT};
      matrix<value_type> VT{N, N, FORTRAN_LAYOUT};

      // Calculate the SVD A = U * Diag(S_vec) * VT
      gesvd(A_FL, _S_vec, U, VT);

      // Calculate the matrix V * Diag(S_vec)^{-1} * UT for the least square procedure
      matrix<double> S_inv{N, M, FORTRAN_LAYOUT};
      S_inv() = 0.;
      for (int i : range(std::min(M, N))) S_inv(i, i) = 1.0 / _S_vec(i);
      V_x_InvS_x_UT = dagger(VT) * S_inv * dagger(U);

      // Read off U_Null for defining the error of the least square procedure
      if (N < M) UT_NULL = dagger(U)(range(N, M), range(M));
    }

    // Solve the least-square problem that minimizes || A * x - B ||_2 given A and B
    std::pair<matrix<value_type>, double> operator()(matrix_const_view<value_type> B, std::optional<long> inner_matrix_dim = {} /*unused*/) const {
      double err = 0.0;
      if (M != N) {
        std::vector<double> err_vec;
        for (int i : range(B.shape()[1])) err_vec.push_back(frobenius_norm(UT_NULL * B(range(), range(i, i + 1))) / sqrt(B.shape()[0]));
        err = *std::max_element(err_vec.begin(), err_vec.end());
      }
      return std::make_pair(V_x_InvS_x_UT * B, err);
    }
  };

  // Least square solver version specific for hermitian tail-fitting.
  // Restrict the resulting vector of moment matrices to one of hermitian matrices
  struct gelss_cache_hermitian {

    using dcomplex = std::complex<double>;

    private:
    // The matrix to be decomposed by SVD
    matrix<dcomplex> A;

    // Solver for the associated real-valued least-squares problem
    gelss_cache<dcomplex> _lss;

    // Solver for the associated real-valued least-squares problem imposing hermiticity
    gelss_cache<dcomplex> _lss_matrix;

    public:
    int n_var() const { return second_dim(A); }
    matrix<dcomplex> const &A_mat() const { return A; }
    vector<double> const &S_vec() const { return _lss.S_vec(); }

    gelss_cache_hermitian(matrix_const_view<dcomplex> _A) : A(_A), _lss(A), _lss_matrix(vstack(A, conj(A))) {}

    // Solve the least-square problem that minimizes || A * x - B ||_2 given A and B with a real-valued vector x
    std::pair<matrix<dcomplex>, double> operator()(matrix_const_view<dcomplex> B, std::optional<long> inner_matrix_dim = {}) const {

      if (not inner_matrix_dim.has_value()) TRIQS_RUNTIME_ERROR << "Inner matrix dimension required for hermitian least square fitting\n";

      unsigned d = inner_matrix_dim.value();
      // Construction of an inner 'adjoint' matrix by performing the following steps
      // * reshape B from (M, M1) to (M, N, d, d)
      // * for each M and N take the adjoint matrix (d, d)
      // * reshape to (M, M)
      auto inner_adjoint = [&d](matrix_view<dcomplex> M) {
        auto idx_map = M.indexmap();
        auto l       = idx_map.lengths();
        auto s       = idx_map.strides();

        TRIQS_ASSERT2(l[1] % (d * d) == 0, "ERROR in hermitian least square fitting: Data shape incompatible with given dimension");
        size_t N = l[1] / (d * d);

        // We reshape the Matrix into a dim=4 array and swap the two innermost indices
	// FIXME We would like to write: tranpose(reshape(idx_map, {l[0], N, d, d}), {0, 1, 3, 2})
	auto idx_map_inner_transpose = array_view<dcomplex, 4>::indexmap_type{
	   {l[0], N, d, d}, {s[0], d * d * s[1], s[1], d * s[1]}, static_cast<ptrdiff_t>(idx_map.start_shift())};

        // Deep copy
        array<dcomplex, 4> arr_dag = conj(array_view<dcomplex, 4>{idx_map_inner_transpose, M.storage()});
        return matrix_view<dcomplex>{array_view<dcomplex, 2>{idx_map, std::move(arr_dag).storage()}};
      };

      // Solve the enlarged system vstack(A, A*) * x = vstack(B, B_dag)
      matrix<dcomplex> B_dag = inner_adjoint(B);
      auto [x, err]          = _lss_matrix(vstack(B, B_dag));

      // Resymmetrize results to cure small hermiticity violations
      return {0.5 * (x + inner_adjoint(x)), err};
    }
  };

} // namespace triqs::arrays::lapack
