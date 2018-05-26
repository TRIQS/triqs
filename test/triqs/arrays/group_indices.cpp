#include "./common.hpp"
#include <triqs/arrays.hpp>

using namespace triqs::arrays;
using namespace triqs::clef;

int main() {

  try {
    placeholder<0> i_;
    placeholder<1> j_;
    placeholder<2> k_;
    placeholder<3> l_;

    { // a simple test
      array<int, 4> A(2, 2, 2, 2);
      A(i_, j_, k_, l_) << i_ + 10 * j_ + 100 * k_ + 1000 * l_;
      TEST_ERR(A);
      TEST_ERR(group_indices_view(A, {0, 1}, {2, 3}));
    }

    // more complex : inversing a tensor product of matrices...
    matrix<double> B(2, 2), C(3, 3), Binv, Cinv;
    C(i_, j_) << 1.7 / (3.4 * i_ - 2.3 * j_ + 1);
    B(i_, j_) << 2 * i_ + j_;
    TEST_ERR(B);
    TEST_ERR(C);
    Binv = inverse(B);
    Cinv = inverse(C);
    TEST_ERR(Binv);
    TEST_ERR(Cinv);

    std::cerr << " --- (1) ---" << std::endl;
    {
      array<double, 4> A(2, 3, 2, 3);
      A(i_, j_, k_, l_) << B(i_, k_) * C(j_, l_);

      TEST_ERR(A);

      auto M = make_matrix_view(group_indices_view(A, {0, 1}, {2, 3}));
      M      = inverse(M);

      // checking
      array<double, 4> R(A.shape());
      R(i_, j_, k_, l_) << Binv(i_, k_) * Cinv(j_, l_);

      TEST_ERR(R);
      TEST_ERR(A);
      assert_all_close(R, A, 5.e-15);
    }

    std::cerr << " --- (2) ---" << std::endl;
    {
      array<double, 4> A(2, 3, 2, 3, make_memory_layout(1, 0, 3, 2));
      A(i_, j_, k_, l_) << B(i_, k_) * C(j_, l_);

      TEST_ERR(A);

      auto M = make_matrix_view(group_indices_view(A, {0, 1}, {2, 3}));
      M      = inverse(M);

      // checking
      array<double, 4> R(A.shape());
      R(i_, j_, k_, l_) << Binv(i_, k_) * Cinv(j_, l_);

      TEST_ERR(R);
      TEST_ERR(A);
      assert_all_close(R, A, 5.e-15);
    }

    std::cerr << " --- (3) ---" << std::endl;
    {
      array<double, 4> A(2, 2, 3, 3, make_memory_layout(0, 2, 1, 3));
      A(i_, k_, j_, l_) << B(i_, k_) * C(j_, l_);

      TEST_ERR(A);

      auto M = make_matrix_view(group_indices_view(A, {0, 2}, {1, 3}));
      M      = inverse(M);

      // checking
      array<double, 4> R(A.shape());
      R(i_, k_, j_, l_) << Binv(i_, k_) * Cinv(j_, l_);

      TEST_ERR(R);
      TEST_ERR(A);
      assert_all_close(R, A, 5.e-15);
    }
  } catch (std::exception const &e) { std::cerr << e.what() << std::endl; }
}
