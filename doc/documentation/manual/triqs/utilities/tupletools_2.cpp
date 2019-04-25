#include <triqs/utility/tuple_tools.hpp>
#include <iostream>

int main() {
  auto t1 = std::make_tuple(1, 2.3, 4.3, 8);
  auto t2 = std::make_tuple('|', '|', '|', '|');
  auto l  = [](double x, char c) { std::cout << c << " " << x << " "; };

  triqs::tuple::for_each_zip(l, t1, t2);
}
