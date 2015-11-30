#include "../test_tools.hpp"
#include <triqs/gfs.hpp>
#include <triqs/gfs/functions/gf_tests.hpp>
#include <triqs/gfs/functions/functions.hpp> 

using namespace triqs::gfs;
using namespace triqs;
using namespace triqs::arrays;
using namespace triqs::lattice;

// 
template<typename X, typename Y>
::testing::AssertionResult test_gfs_are_close(X const &x, Y const &y) {

 try {
  assert_gfs_are_close(x,y);
  return ::testing::AssertionSuccess();
 } catch (const char* msg) {
  return ::testing::AssertionFailure() << msg;
 }

}

#define EXPECT_GF_NEAR(X, Y) EXPECT_TRUE(test_gfs_are_close(X,Y))

// 
template<typename X, typename Y>
::testing::AssertionResult test_block_gfs_are_close(X const &x, Y const &y) {

 try {
  assert_block_gfs_are_close(x,y);
  return ::testing::AssertionSuccess();
 } catch (const char* msg) {
  return ::testing::AssertionFailure() << msg;
 }

}

#define EXPECT_BLOCK_GF_NEAR(X, Y) EXPECT_TRUE(test_block_gfs_are_close(X,Y))

