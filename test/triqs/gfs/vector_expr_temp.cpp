#include <triqs/utility/std_vector_expr_template.hpp>
#include <iostream>

int main() {

 std::vector<int> V1, V2; 

 for(size_t i = 0; i<5; ++i) V1.push_back(i);
 for(size_t i = 0; i<5; ++i) V2.push_back(2-i);

 for (size_t i =0; i<5; ++i) std::cout  << (V1+ 2*V2)[i] << std::endl ;

 //auto V3 = make_vector(V1/2);
 std::vector<double> V3;
V3 = make_vector(V1 - V2/2.5);

 for (auto &x: V3) std::cout  << x << std::endl ;

}

