#include <nda/clef.hpp>
#include <nda/nda.hpp>
#include <iostream>
#include <algorithm>
int main() {
  // Declaring some placeholders (i.e. dummy variables).
  nda::clef::placeholder<0> i_;
  nda::clef::placeholder<1> j_;

  // Declaring a 3x3 matrix
  nda::matrix<double> A(3, 3);

  // Automatically filling the matrix
  // -> forget about the bounds, it is automatic
  // -> forget about the best order to order the for loops for performance, it is also automatic
  A(i_, j_) << i_ + 2 * j_;

  // Cheking the result
  std::cout << A << std::endl;

  // It also works for std container: we just have to add a call clef::make_expr function
  std::vector<double> V(10);
  double pi = std::acos(-1);

  // Automatically filling the vector with the evaluation of the expression in i_
  nda::clef::make_expr(V)[i_] << cos(2 * pi / 5.0 * i_);

  // -> by the way, the constant calculation is precomputed
  // (expressions are partially evaluated as soon as possible)
  // illustration :
  // the time_consuming_function will be called only once in the loop, while cos is called 10 times
  auto time_consuming_function = [](double x) {
    std::cout << "call time_consuming_function" << std::endl;
    return 2 * x;
  };
  nda::clef::make_expr(V)[i_] << cos(time_consuming_function(10) * i_);

  // If you insist using on more complex containers...
  std::vector<std::vector<double>> W(3, std::vector<double>(5));
  nda::clef::make_expr(W)[i_][j_] << i_ + cos(time_consuming_function(10) * j_ + i_);
}
