#pragma once
#include "./arrays.hpp"
#include <triqs/operators/many_body_operator.hpp>

// Test that two operators are term-wise equal up to a precision
template<typename X, typename Y>
::testing::AssertionResult test_operators_are_close(X const &x, Y const &y, double precision = 1e-6) {
 try {
  assert_operators_are_close(x,y, precision);
  return ::testing::AssertionSuccess();
 } catch (triqs::exception const & msg) {
  return ::testing::AssertionFailure() << msg.what();
 }
}

#define EXPECT_OPERATOR_NEAR(X, ...) EXPECT_TRUE(test_operators_are_close(X,__VA_ARGS__))
