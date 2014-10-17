#include <triqs/utility/tuple_tools.hpp>
#include <iostream>

int main() {
 auto t = std::make_tuple(1, 2.3, 4.3, 8);

 // a simple example : make a sum
 auto f = [](double x, double r) { return x + r; };
 auto res = triqs::tuple::fold(f, t, 0); // <-- R is int but res is a double (from f's return type).

 std::cout << res << std::endl;

 // the types of x and r may be different
 triqs::tuple::fold([](double x, std::ostream & os)->std::ostream & { return os << "|" << x; }, t, std::cout);

 // example with zip
 auto t0 = std::make_tuple(1, 2, 3, 4);
 auto t1 = std::make_tuple(1, -1, 1, -1);

 auto res2 = triqs::tuple::fold([](double x, double y, double r) { return y * x + r; }, t0, t1, 0);

 std::cout << "\n " << res2 << std::endl;
}

