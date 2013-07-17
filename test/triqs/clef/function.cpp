#include "./common.hpp"

int main() { 
 F1 f(7);

 TEST((x_ >> (2*x_ +1) )(10));

 triqs::clef::function<double(double,double)>  f2;
 f2(x_,y_) = x_ + y_;
 TEST(f2(2,3));
 f2(x_,y_) << x_ + 2*y_;
 TEST(f2(2,3));

 std::function<double(double,double)> ff2(f2);
 TEST(ff2(2,3));

 triqs::clef::function<double(double)>  f1 ( f(x_) + 2*x_ ,x_);
 TEST(f1(3));

 triqs::clef::function<double(double,double)> g2;
 g2(x_,y_) << x_ - y_ + f2(x_,2*y_);
 TEST(g2(2,3));

 std::function<double(double,double)> ff8 = make_function( x_ + y_, x_, y_); 
 TEST(ff8(2,3));
 
 std::function<double(double)>  f3;
 f3 = x_>> f(x_) + 2*x_;
 TEST(f3(3));

 auto h = make_function( 2*x_ + y_ + 1, x_);
 //  is a lazy expression expression with placeholder y_, returning a function... 
 std::cout << tql::eval(h, y_=1) << std::endl; 
 TEST(tql::eval( h, y_=1) (10)); 
 TEST(h); 

}

