#include <nda/clef.hpp>
#include <iostream>
int main() {
  nda::clef::placeholder<3> x_;
  std::cout << 2.0 + std::cos(2.0) << std::endl;
  std::cout << eval(x_ + cos(x_), x_ = 2) << std::endl; // NB : note the absence of std::
}
