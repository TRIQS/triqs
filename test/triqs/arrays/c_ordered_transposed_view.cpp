#include "./common.hpp"

using namespace triqs::arrays;
using namespace triqs::clef;

template <typename... T> void test(T... x) {
  placeholder<0> i_;
  placeholder<1> j_;
  placeholder<2> k_;
  placeholder<3> l_;
 array<int, 4> a(1, 2, 3, 4, make_memory_layout(x...));
 a(i_, j_, k_, l_) << i_ + 10 * j_ + 100 * k_ + 1000 * l_;
 TEST_ERR(a);

 auto v = c_ordered_transposed_view(a());

 TEST(a.shape());
 TEST(a.indexmap().get_memory_layout());
 TEST(v.shape());
 TEST(v.indexmap().get_memory_layout());
 std::cerr << "----------------" << std::endl;
}

int main() {

 try {

 test(0, 1, 2, 3);
 test(1, 0, 2, 3);
 test(2, 0, 3, 1);
 }
 catch (std::exception const& e) {
  std::cerr << e.what() << std::endl;
 }
}

