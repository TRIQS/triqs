#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include "../test_tools.hpp"
#include <triqs/gfs.hpp>
#include <triqs/gfs/functions/functions.hpp> 

using namespace triqs::gfs;
using namespace triqs;
using namespace triqs::arrays;
using namespace triqs::lattice;

// 
::testing::AssertionResult tail_are_close(tail const &x, tail const &y) {
 if (x.order_min() != y.order_min())
  return ::testing::AssertionFailure() << "Tail do not have the same omin"
                                       << "\n X = " << x << "\n Y = " << y;

 EXPECT_ARRAY_NEAR(x.data(), y.data()) << "Gf are not equal : data differ";

 return ::testing::AssertionSuccess();
}

::testing::AssertionResult tail_are_close(nothing const &x, nothing const &y) {
 return ::testing::AssertionSuccess();
}

template<typename X, typename Y>
::testing::AssertionResult gf_are_close(X const &x, Y const &y) {
 double precision = 1.e-10;
 if (x.mesh() != y.mesh()) 
  return ::testing::AssertionFailure() << "Comparing two arrays of different size "
          << "\n X = "<<  x << "\n Y = "<< y;

 EXPECT_ARRAY_NEAR(x.data(), y.data()) << "Gf are not equal : data differ";

 return tail_are_close(x.singularity(), y.singularity());

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


