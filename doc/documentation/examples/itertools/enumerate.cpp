#include <itertools/itertools.hpp>
#include <vector>
#include <iostream>

int main() { 

  std::vector<int> v {10,11,12,13,14};
  
  for (auto [n,x] : itertools::enumerate(v)) 
    std::cout << n << "  " << x << std::endl;
}
