#include <itertools/itertools.hpp>
#include <vector>
#include <iostream>

int main() { 

  std::vector<int> v1 {10,11,12,13,14};
  std::vector<int> v2 {0,1,2,3,4};
  
  for (auto [x,y] : itertools::zip(v1, v2)) 
    std::cout << x << "  " << y << std::endl;
}
