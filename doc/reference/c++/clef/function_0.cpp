#include <triqs/clef.hpp>
#include <iostream>
using namespace triqs::clef;

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
 { // Currying a function
  // auto f = make_function ( make_function( 2*x_ + y_ + 1, x_), y_);
  auto f = y_ >> (x_ >> 2 * x_ + y_ + 1);
  // f a function y-> x-> 2x+y+1
  // f(y) returns a function x-> 2x+y+1
  auto g = f(3);
  std::cout << g(10) << std::endl;
 }
 { // playing with clef::function and std::function
  triqs::clef::function<double(double, double)> f2, g2;
  f2(x_, y_) = x_ + y_;
  std::cout << f2(2, 3) << std::endl;

  std::function<double(double, double)> sf2 = f2;
  std::cout << sf2(2, 3) << std::endl;

  g2(x_, y_) = x_ - y_ + f2(x_, 2 * y_);

  std::function<double(double)> sf = x_ >> 2 * x_ + 1;
  std::cout << sf(3) << std::endl;
 }
}

