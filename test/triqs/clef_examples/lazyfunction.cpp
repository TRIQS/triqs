#include <triqs/clef.hpp>
#include <iostream>
namespace tql = triqs::clef;

int main() { 
 // two placeholders
 tql::placeholder <1> x_; 
 tql::placeholder <2> y_; 

 // a function of one variable
 triqs::clef::function<double(double)>  h;
 h(x_) = 2 + 2*x_;
 std::cout<<h(3)<<std::endl;

 // expression can be given at construction
 triqs::clef::function<double(double)>  h2 ( 2 + 2*x_ ,x_);
 std::cout<<h2(3)<<std::endl;

 // two variable functions
 triqs::clef::function<double(double,double)>  f,g;
 
 f(x_,y_) = x_ + y_;
 
 std::cout<<f(2,3)<<std::endl;
 
 // affecting a new expression to f
 f(x_,y_) = x_ + 2*y_;
 
 std::cout<<f(2,3)<<std::endl;
 
 // f can be called with lazy expressions too to form more complex expressions.
 g(x_,y_) = x_ - y_ + f(x_,2*y_);
 
 std::cout<<g(2,3)<<std::endl;

 // lazy function are interoperable with std::function
 std::function<double(double,double)> bf(f);
 std::cout<<bf(2,3)<<std::endl;

 }
