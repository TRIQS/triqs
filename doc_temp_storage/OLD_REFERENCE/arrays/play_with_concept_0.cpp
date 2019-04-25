#include <triqs/arrays.hpp>
#include <triqs/arrays/make_immutable_array.hpp>
using namespace triqs::arrays;
using triqs::clef::placeholder;
int main() {
  placeholder<0> i_;
  placeholder<1> j_;

  auto a = make_immutable_array(1.0 / (2 + i_ + j_), i_ = range(0, 2), j_ = range(0, 2));

  std::cout << "a = " << a << std::endl;
  std::cout << "a = " << array<double, 2>(a) << std::endl;

  // or if you prefer using a lambda...
  auto b = make_immutable_array([](int i, int j) { return i - j; }, range(0, 2), range(0, 2));

  std::cout << "b = " << b << std::endl;
  std::cout << "b = " << array<double, 2>(b) << std::endl;
}
