#include <triqs/arrays.hpp>
/// ---- test ------------

using triqs::arrays::range;

int main() {
 triqs::clef::placeholder<0> a_;
 triqs::clef::placeholder<1> i_;
 triqs::clef::placeholder<2> j_;

 auto expr = i_ + 2* j_;

 auto xa = sum(a_*i_ , i_=range(0,5)); 
 auto x = eval(xa, a_=2);

 std::cout << x << std::endl;

 auto y = sum(2*i_ + j_ , i_=range(0,3), j_= range (0,2)); 
 std::cout << y << std::endl;
}

