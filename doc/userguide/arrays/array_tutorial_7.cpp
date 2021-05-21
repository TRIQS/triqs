#include <nda/nda.hpp>
#include <nda/map.hpp>
using nda::array;

double f(int i) { return i * 10; }

int main() {
  auto F = nda::map(std::function<double(int)>(f));
  array<int, 2> A(2, 2);
  A() = 2;

  array<double, 2> B, C;

  A() = 2;
  B   = F(A);
  C   = F(2 * A); // works also with expressions of course

  std::cout << "A = " << A << std::endl;
  std::cout << "F(A) = " << B << std::endl;
  std::cout << "F(2*A) = " << C << std::endl;
}
