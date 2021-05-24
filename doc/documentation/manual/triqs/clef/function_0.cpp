#include <nda/clef.hpp>
#include <iostream>
using namespace nda::clef;

int main() {
  placeholder<0> x_;
  placeholder<1> y_;
  { // with one variable
    auto f = make_function(2 * x_ + 1, x_);
    std::cout << f(3) << std::endl;
    std::function<double(double)> F(f);
  }
  { // with two variables
    auto f = make_function(2 * x_ + y_ + 1, x_, y_);
    std::cout << f(3, 4) << std::endl;
    std::function<double(double, double)> F(f);
  }
  { // Make a function partially
    auto f = make_function(2 * x_ + y_ + 1, x_);
    // f is a lazy expression expression with placeholder y_, returning a function...
    auto f1 = eval(f, y_ = 1); // f1 is a function x-> 2*x + 2
    std::cout << f1(10) << std::endl;
  }
}
