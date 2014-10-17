#include <triqs/arrays.hpp>
#include <iostream>
using namespace triqs::arrays;
int main() {
 auto a = array<double, 2>(2, 3);
 std::cout << get_shape(a) << std::endl;
 std::cout << first_dim(a) << std::endl;
 std::cout << second_dim(a) << std::endl;
}

