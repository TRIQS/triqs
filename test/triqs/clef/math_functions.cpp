#include "./common.hpp"
#include <triqs/clef/adapters/vector.hpp>

double foo(double x) { return x / 2; }
int foo(int x) { return x / 2; }

double bar(double x, double y) { return x + y; }

namespace triqs {
  namespace clef {

    using ::bar;
    using ::foo;

    template <typename T> typename std::enable_if<!triqs::clef::is_any_lazy<T>::value, T>::type inc(T const &x) { return x + 1; }
    // using ::inc;
    // moving the declaration up and using using:: does not work on gcc4.4
    // however not using using:: does not work on icc 11 !

    TRIQS_CLEF_MAKE_FNT_LAZY(bar);
    TRIQS_CLEF_MAKE_FNT_LAZY(inc);
    TRIQS_CLEF_MAKE_FNT_LAZY(foo);
  } // namespace clef
} // namespace triqs

namespace tql = triqs::clef;
int main() {
  using std::cout;
  using std::endl;
  TEST(tql::eval(cos(x_), x_ = 2));
  TEST(tql::eval(cos(2 * x_ + 1), x_ = 2));
  TEST(tql::eval(abs(2 * x_ - 1), x_ = 2));
  TEST(tql::eval(abs(2 * x_ - 1), x_ = -2));
  TEST(tql::eval(floor(2 * x_ - 1), x_ = 2.3));
  TEST(tql::eval(pow(2 * x_ + 1, 2), x_ = 2.0));
  TEST(tql::eval(foo(2 * x_ + 1), x_ = 2));
  TEST(tql::eval(foo(2 * x_ + 1), x_ = 2.0));
  TEST(tql::eval(bar(2 * x_ + 1, x_ - 1), x_ = 2));
  TEST(tql::eval(inc(2 * x_ + 1), x_ = 2));
}
