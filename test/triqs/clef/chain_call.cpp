#include "./common.hpp"

#include <vector>

struct F2_vec {
  std::vector<F2> vec;
  F2_vec(size_t siz = 0) : vec(siz) {}
  F2_vec(F2_vec const &x) : vec(x.vec) {}

  F2 const &operator()(size_t i) const { return vec[i]; }

  TRIQS_CLEF_IMPLEMENT_LAZY_CALL(F2_vec);

  template <typename Fnt> friend void triqs_clef_auto_assign(F2_vec const &x, Fnt f) {
    for (size_t i = 0; i < x.vec.size(); ++i) triqs_clef_auto_assign(x.vec[i], f(i));
  }

  friend std::ostream &operator<<(std::ostream &out, F2_vec const &x) { return out << "F2_vec"; }
};

triqs::clef::placeholder<4> i_;

using namespace triqs::clef;
int main() {

  F2_vec V(3);

  //double x=1,y=2;

  {
    auto expr = V(i_)(x_, y_);
    std::cout << "expr = " << expr << std::endl;

    TEST(tql::eval(expr, x_ = 2, y_ = 3, i_ = 0));
    TEST(tql::eval(expr, x_ = 2));
    TEST(tql::eval(expr, x_ = 2, i_ = 1));
    TEST(tql::eval(expr, x_ = 2, y_ = 3));
    TEST(tql::eval(tql::eval(expr, x_ = 2, y_ = 3), i_ = 0));
    std::cout << "-------------" << std::endl;
  }

  {
    // test assign
    V(i_)(x_, y_) << x_ + i_; // + 10*y_ + 100*i_;
  }
}
