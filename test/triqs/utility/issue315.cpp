#include <iostream>

#include <triqs/utility/time_pt.hpp>

using triqs::utility::time_pt;
using triqs::utility::time_segment;

int main(int argc, char **argv) {

  time_segment seg(10);

  time_pt zero_pt = seg.get_lower_pt();
  time_pt beta_pt = seg.get_upper_pt();
  time_pt pt      = seg.make_time_pt(3);

  std::cout << beta_pt << std::endl;

  std::cout << pt + zero_pt << std::endl;
  std::cout << pt + beta_pt << std::endl;
  std::cout << pt - zero_pt << std::endl;
  std::cout << pt - beta_pt << std::endl;

  std::cout << zero_pt + zero_pt << std::endl;
  std::cout << zero_pt + beta_pt << std::endl;
  std::cout << beta_pt + zero_pt << std::endl;
  std::cout << beta_pt + beta_pt << std::endl;

  std::cout << zero_pt - zero_pt << std::endl;
  std::cout << zero_pt - beta_pt << std::endl;
  std::cout << beta_pt - zero_pt << std::endl;
  std::cout << beta_pt - beta_pt << std::endl;

  return 0;
}
