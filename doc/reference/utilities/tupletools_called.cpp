#include <triqs/utility/tuple_tools.hpp>
#include <iostream>
int main() {
  auto fun = [](int i, double x, double y, int k) { return 6 * k + i - 1.3 * x + 2 * y; };
  auto t   = std::make_tuple(1, 2.3, 4.3, 8);

  auto res = triqs::tuple::called_on_tuple(fun)(t);

  std::cout << " f(t) =" << res << std::endl;
}
