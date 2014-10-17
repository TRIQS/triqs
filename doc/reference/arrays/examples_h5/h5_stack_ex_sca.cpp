#include <triqs/arrays/array.hpp>
#include <triqs/arrays/h5/array_stack.hpp>
namespace tqa=triqs::arrays; namespace h5=tqa::h5;

int main() { 
  const size_t N = 100, bufsize = 5;
  h5::H5File file("ess.h5", H5F_ACC_TRUNC); // open a file

  h5::array_stack< double > S (file, "my_stack", bufsize); 

  for (int u = 0; u< N ; ++u)  { // fill the stack for a while...
   S << (u+1)* 20; 
  }
  S.flush(); // useless at the end...

}


