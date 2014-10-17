#include <triqs/clef.hpp>
using namespace triqs::clef;
int main() {
 placeholder<1> x_;
 placeholder<2> y_;
 std::cout << eval(x_ + 2 * y_, x_ = 1, y_ = 2) << std::endl;
 std::cout << eval(x_ + 2 * y_, y_ = 2, x_ = 1) << std::endl;
}

