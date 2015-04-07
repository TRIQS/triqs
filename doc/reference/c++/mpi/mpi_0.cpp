#include <triqs/arrays.hpp>
#include <iostream>

using namespace triqs;
using namespace triqs::arrays;
using namespace triqs::mpi;

int main(int argc, char *argv[]) {

 mpi::environment env(argc, argv);
 mpi::communicator world;

 int a = 5;
 broadcast(a);
 reduce_in_place(a);

 array<int, 2> A(2,10); A()=1;

 std::cout<<A<<std::endl;

 scatter(A);
 A += world.rank();
 gather(A);

 std::cout<<A<<std::endl;

 return 0;
}

