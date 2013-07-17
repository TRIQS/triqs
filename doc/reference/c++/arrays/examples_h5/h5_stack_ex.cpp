
#include <triqs/arrays/array.hpp>
#include <triqs/arrays/h5/array_stack.hpp>
namespace tqa=triqs::arrays; namespace h5=tqa::h5;

int main() { 
  const size_t N = 100, bufsize = 5;
  h5::H5File file("ess.h5", H5F_ACC_TRUNC); // open a file
  
  tqa::array<double,2> A(3,2);  // an array of the shape of the base of the stack                         

  h5::array_stack< tqa::array<double,2> > S(file, "my_stack", A.shape(), bufsize); 

  for (int u = 0; u< N ; ++u)  { // fill the stack for a while...
   A() = (u+1)* 20; 
   S << A; 
   // S() =  A; ++S; // equivalent
  }
  S.flush(); // useless at the end...

}


