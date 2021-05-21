#include <nda/clef.hpp>
#include <iostream>

template <typename Domain> struct sum_impl {
  Domain d;

  // C++14
  // double operator() (NotClefExpression const & f) const { double s=0; for (int u=0; u<10; ++u) s += f(u/10.0); return s;}

  // C++11 form
  template <typename F> typename std::enable_if<!nda::clef::is_clef_expression<F>, double>::type operator()(F const &f) const {
    double s = 0;
    for (int u = 0; u < 10; ++u) s += f(u / 10.0);
    return s;
  }

  CLEF_IMPLEMENT_LAZY_CALL(sum_impl);

  friend std::ostream &operator<<(std::ostream &out, sum_impl const &x) { return out << "sum"; }
};

// a little factory ...
template <typename Domain> sum_impl<Domain> sum_functional(Domain d) { return {d}; }

struct DOM {};

int main() {
  nda::clef::placeholder<1> x_;
  nda::clef::placeholder<2> y_;
  DOM d;

  // integrate_on_d is the integration functional
  auto integrate_on_d = sum_functional(d);

  // This is a simple application of the sum to a function
  std::cout << integrate_on_d(x_ >> 2 * x_ + 1) << std::endl;

  // A function y -> y_ + integrate (x -> 2*x + y)
  auto e1 = y_ + integrate_on_d(x_ >> 2 * x_ + y_);
  std::cout << e1 << std::endl;
  std::cout << eval(e1, y_ = 0) << std::endl;
}
