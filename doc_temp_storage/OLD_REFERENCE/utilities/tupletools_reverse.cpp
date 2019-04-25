#include <triqs/utility/tuple_tools.hpp>
#include <iostream>

int main() {
  auto t = std::make_tuple(1, 2.3, 4.3, 8);
  auto l = [](double x) { std::cout << x << " "; };

  triqs::tuple::for_each(t, l);
  std::cout << std::endl;
  triqs::tuple::for_each(reverse(t), l);
}
