#include <triqs/test_tools/arrays.hpp>

TEST(Array, InverseTensor){

 array<dcomplex,3> a(2,2,2);
 a() = a() + 2;

 a() = inverse(a);

 array<dcomplex,3> r(2,2,2);
 r() = 0.5;

 EXPECT_CLOSE_ARRAY(r,a);
}

MAKE_MAIN;

