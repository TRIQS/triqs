#pragma once
#include "./arrays.hpp"
#include <triqs/gfs.hpp>
#include <triqs/gfs/gf_tests.hpp>

using namespace triqs::gfs;
using namespace triqs;
using namespace triqs::arrays;
using namespace triqs::lattice;

// 
template<typename X, typename Y>
::testing::AssertionResult test_gfs_are_close(X const &x, Y const &y, double precision = 1.e-6) {

 try {
  assert_gfs_are_close(x,y, precision);
  return ::testing::AssertionSuccess();
 } catch (triqs::exception const & msg) {
  return ::testing::AssertionFailure() << msg.what();
 }

}

#define EXPECT_GF_NEAR(X, ...) EXPECT_TRUE(test_gfs_are_close(X,__VA_ARGS__))

template<typename X, typename Y>
::testing::AssertionResult test_tails_are_close(X const &x, Y const &y, double precision = 1.e-6) {

 try {
  assert_tails_are_close(x,y, precision);
  return ::testing::AssertionSuccess();
 } catch (triqs::exception const & msg) {
  return ::testing::AssertionFailure() << msg.what();
 }

}

#define EXPECT_TAIL_NEAR(X, ...) EXPECT_TRUE(test_tails_are_close(X,__VA_ARGS__))
// 
template<typename X, typename Y>
::testing::AssertionResult test_block_gfs_are_close(X const &x, Y const &y, double precision = 1.e-6) {

 try {
  assert_block_gfs_are_close(x,y, precision);
  return ::testing::AssertionSuccess();
 } catch (triqs::exception const & msg) {
  return ::testing::AssertionFailure() << msg.what();
 }

}

#define EXPECT_BLOCK_GF_NEAR(X, ...) EXPECT_TRUE(test_block_gfs_are_close(X,__VA_ARGS__))

