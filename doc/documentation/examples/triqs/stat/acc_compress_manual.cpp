#include <triqs/stat/accumulator.hpp>
#include <iostream>

using namespace triqs::stat;

int main() {
    // Accumulating a simple scalar
    // Defines linear bins of capacity 1, up to a maximum of 100 bins
    accumulator<double> my_acc(0.0, 0, 100, 1);

    // Add data
    my_acc << 0.0;
    my_acc << 1.0;
    my_acc << 2.0;
    my_acc << 3.0;
    my_acc << 4.0;
    
    std::cout << "Before compression: " << std::endl;
    for(auto &x : my_acc.linear_bins()){
      std::cout << x << std::endl;  // 0.0, 1.0, 2.0, 3.0, 4.0
    }
    // Compress by a factor two by hand
    my_acc.compress_linear_bins(2);

    std::cout << "After compression: " << std::endl;
    for(auto &x : my_acc.linear_bins()){
      std::cout << x << std::endl; // 0.5, 2.5, 4.0
    }

    std::cout << "New Bin Capacity: " << my_acc.lin_bin_capacity() << std::endl; // = 2
}