#include <nda/clef.hpp>
#include <iostream>

// a simple foo function
double foo(double x) { return x / 2; }
int foo(int x) { return x * 2; }

// a more complex case : bar is already a template
// we have to disable it for CLEF expression to avoid ambiguity

// C++14 clean syntax will be (using concepts)
// template<NotClefExpression T>
// T bar (T const & x) { return x+1;}

// C++11 workaround
template <typename T> typename std::enable_if<!nda::clef::is_clef_expression<T>::value, T>::type bar(T const &x) { return x + 1; }

namespace triqs {
  namespace clef {
    CLEF_MAKE_FNT_LAZY(foo);
    CLEF_MAKE_FNT_LAZY(bar);
  } // namespace clef
} // namespace triqs

int main() {
  nda::clef::placeholder<3> x_;
  std::cout << foo(2.0) << " " << eval(x_ + foo(x_), x_ = 3) << " " << eval(x_ + foo(x_), x_ = 3.5) << std::endl;
  std::cout << bar(2.0) << " " << eval(x_ + bar(x_), x_ = 3) << " " << eval(x_ + bar(x_), x_ = 3.5) << std::endl;
}
