
#include <triqs/arrays.hpp>

using namespace triqs::arrays;

template<typename T> 
void f(array<T,1> const & M) { std::cout  << M << std::endl ;}

void g(array<double,1> const & M) { f(M);}

int main() {

 array<double,1> A = {1,2,3};

 f(A);

 g(2*A);
 
}


