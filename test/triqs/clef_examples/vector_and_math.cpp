#include "triqs/clef.hpp"
#include "triqs/clef/adapters/vector.hpp"
#include "triqs/clef/adapters/math.hpp"
#include <iostream> 
namespace tql = triqs::clef;

int main() { 

 int N = 10;
 double pi = std::acos(-1);
 std::vector<double> V(N);

 // automatic assignment of vector and use of lazy math function
 tql::placeholder <0> k_; 
 tql::lazy(V) [k_]  << cos( (2* pi* k_)/ N );

 // check result... 
 for (size_t u=0; u<V.size(); ++u)
   std::cout<< u << " "<<V[u]<< "  "<< cos((2*pi*u)/N)<<std::endl;

}


