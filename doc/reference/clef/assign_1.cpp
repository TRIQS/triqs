#include <triqs/clef.hpp>
#include <iostream>
using namespace triqs::clef;

struct Obj {
 double v;
 TRIQS_CLEF_IMPLEMENT_LAZY_CALL();
 //
 template <typename Fnt> friend void triqs_clef_auto_assign(Obj& x, Fnt f) {
  std::cout << " called triqs_clef_auto_assign " << f(x.v++) << std::endl;
 }
};

int main() {
 Obj f{2};
 placeholder<3> x_;
 std::cout << f.v << std::endl;
 f(x_) << 8 * x_;
 std::cout << f.v << std::endl;
}

