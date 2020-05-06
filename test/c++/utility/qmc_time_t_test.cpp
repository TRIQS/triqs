#include <iostream>

#include <triqs/utility/time_pt.hpp>

using triqs::utility::time_pt;
using triqs::utility::time_segment;

void print_me(double x) { std::cout << " print " << x << std::endl; }

int main(int argc, char **argv) {
  time_segment ts(40);
  time_pt zero = ts.get_lower_pt();
  time_pt beta = ts.get_upper_pt();

  auto t1 = div_by_int(beta, 5); // 8
  auto t2 = mult_by_int(t1, 4);  // 32

  std::cout << "zero: " << zero << std::endl;
  std::cout << "beta: " << beta << std::endl;
  std::cout << "t1: " << t1 << std::endl;
  std::cout << "t2: " << t2 << std::endl;

  std::cout << "t1 + t2: " << t1 + t2 << std::endl;
  std::cout << "t1 + t2 - (as double) : " << t1 + t2 - (double(t1) + double(t2)) << "" << std::endl;

  std::cout << "t1 - t2: " << t1 - t2 << std::endl;
  std::cout << "t1 - t2 - (as double) : " << t1 - t2 - (double(t1) - double(t2)) << "" << std::endl;

  std::cout << "t1 * t2: " << t1 * t2 << std::endl;
  std::cout << "t1 * t2 - (as double) : " << t1 * t2 - (double(t1) * double(t2)) << "" << std::endl;

  std::cout << "t1 / t2: " << t1 / t2 << std::endl;
  std::cout << "t1 / t2 - (as double) : " << t1 / t2 - (double(t1) / double(t2)) << "" << std::endl;

  std::cout << "t1 + beta: " << t1 + beta << std::endl;
  std::cout << "t1 - beta: " << t1 - beta << std::endl;

  print_me(double(t1)); // to check explicit cast

  std::cout << " -t1: " << -t1 << std::endl;

  std::cout << "epsilon: " << ts.get_epsilon() << std::endl;
  std::cout << "- epsilon: " << -ts.get_epsilon() << std::endl;
  std::cout << "t1 - epsilon :" << t1 - ts.get_epsilon() << std::endl;
  std::cout << "beta - epsilon :" << beta - ts.get_epsilon() << std::endl;
  std::cout << "beta - epsilon + epsilon:" << (beta - ts.get_epsilon()) + ts.get_epsilon() << std::endl;
  std::cout << "beta + 0:" << beta + zero << std::endl;

  std::cout << " num limit max " << std::numeric_limits<uint64_t>::max() << std::endl;
  uint64_t n = std::numeric_limits<uint64_t>::max() - 2;
  n++;
  std::cout << " num limit-1  " << n << std::endl;
  n++;
  std::cout << " num limit+0  " << n << std::endl;
  n++;
  std::cout << " num limit+1  " << n << std::endl;
  n++;
  std::cout << " num limit+2  " << n << std::endl;

  return 0;
}
