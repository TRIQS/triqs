#include <iostream>

struct infty {};

template<typename T> struct wrap {
 T x;
 wrap(T const & x_): x(x_) {}
 operator infty() const { return infty();}
 operator T () const { return x;}
};


double g(infty) { return 3;}
double f(double) { return 30;}

int main() {

 wrap<double> w(2);
 std::cout << w  +1 <<std::endl;
 std::cout << g(w) <<std::endl;
 std::cout << f(w) <<std::endl;

}
