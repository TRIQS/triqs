#include <nda/nda.hpp>
using nda::array;
namespace tql = nda::clef;

int main() {
  tql::placeholder<0> i_;
  tql::placeholder<1> j_;
  array<double, 2> A(2, 2);
  A(i_, j_) << i_ + j_;
  std::cout << "A = " << A << std::endl;
}
