#include <triqs/clef/clef.hpp>
#include <triqs/clef/io.hpp>
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> " << (X) << std::endl;

struct F1 {
  double v;
  F1(double v_) : v(v_) {}
  F1(F1 const &) = delete; // non copyable
  F1(F1 &&x) : v(x.v) { std::cerr << "Moving F1 " << v << std::endl; }

  double operator()(double x) const { return 10 * x; }

  TRIQS_CLEF_IMPLEMENT_LAZY_CALL(F1);

  template <typename Fnt> friend void triqs_clef_auto_assign(F1 &x, Fnt f) {
    x.v++;
    std::cerr << " called triqs_clef_auto_assign " << f(100) << std::endl;
  }

  friend std::ostream &operator<<(std::ostream &out, F1 const &x) { return out << "F1"; }
};

struct F2 {

  double v;
  F2() { v = 0; }

  double operator()(double x, double y) const { return 10 * x + y; }

  TRIQS_CLEF_IMPLEMENT_LAZY_CALL(F2);

  template <typename Fnt> friend void triqs_clef_auto_assign(F2 const &x, Fnt f) {
    std::cerr << " called F2 triqs_clef_auto_assign " << f(10, 20) << std::endl;
  }

  friend std::ostream &operator<<(std::ostream &out, F2 const &x) { return out << "F2"; }
};

using namespace triqs::clef;

triqs::clef::placeholder<1> x_;
triqs::clef::placeholder<2> y_;
triqs::clef::placeholder<3> z_;
namespace tql = triqs::clef;
