#include <triqs/mesh.hpp>
#include <iostream>

// Dummy domain.
struct domain {};

// Overload evaluate for a single dummy domain.
auto evaluate(domain d, auto f, double x) { return f(x); }

int main() {
  // function to evaluate by currying
  auto f = [](double x, double y, double z) { return x + y + z; };

  // calculate f(1, 2, 3) = 6 using triqs::mesh::evaluate
  std::cout << triqs::mesh::evaluate(std::make_tuple(domain{}, domain{}, domain{}), f, 1, 2, 3) << std::endl;
}
