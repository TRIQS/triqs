#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include "../test_tools.hpp"
#include <triqs/gfs.hpp>

// 
template<typename X, typename Y>
::testing::AssertionResult gf_are_close(X const &x, Y const &y) {
 double precision = 1.e-10;
 if (x.mesh() != y.mesh()) 
  return ::testing::AssertionFailure() << "Comparing two arrays of different size "
          << "\n X = "<<  x << "\n Y = "<< y;

 EXPECT_ARRAY_NEAR(x.data(), y.data()) << "Gf are not equal : data differ";
 return ::testing::AssertionSuccess();
}

#define EXPECT_GF_NEAR(X, Y) EXPECT_TRUE(gf_are_close(X,Y))

// 
template<typename X, typename Y>
::testing::AssertionResult block_gf_are_close(X const &x, Y const &y) {
 double precision = 1.e-10;

 EXPECT_EQ( n_blocks(x), n_blocks(y)) << "Number of block differ between two block_gf";

 for (int u = 0; u < n_blocks(x); ++u) EXPECT_GF_NEAR(x[u], y[u]);
 return ::testing::AssertionSuccess();
}

#define EXPECT_BLOCK_GF_NEAR(X, Y) EXPECT_TRUE(block_gf_are_close(X,Y))


