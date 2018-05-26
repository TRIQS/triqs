#include <triqs/utility/tuple_tools.hpp>
#include <iostream>

int main() {
  auto t = std::make_tuple(1, 2.3, 4.3, 8);
  auto l = [](double x) { return -x; };

  auto res = triqs::tuple::map(l, t);

  std::cout << res << std::endl;
}
