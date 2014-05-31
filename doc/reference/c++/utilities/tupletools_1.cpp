#include <triqs/utility/tuple_tools.hpp>
#include <iostream>

struct print_t {
 template <typename T> void operator()(T x) { std::cout << x << " "; }
};

int main() {
 auto t = std::make_tuple(1, 2.3, 4.3, 8, "hello");
 triqs::tuple::for_each(t, print_t());

 // C++14 solution : with generic lambda, there is no need to define a print_t ...
 // triqs::tuple::for_each(t, [](auto x) { std::cout<<x<<" ";});
}

