#include <nda/nda.hpp>
#include <nda/mpi.hpp>
#include <iostream>

using namespace nda;

int main(int argc, char *argv[]) {

  mpi::environment env(argc, argv);
  mpi::communicator world;

  int a = 5;
  mpi::broadcast(a);
  a = mpi::reduce(a);

  array<int, 2> A(2, 10);
  A() = 1;

  std::cout << A << std::endl;

  mpi::scatter(A);
  A += world.rank();
  mpi::gather(A);

  std::cout << A << std::endl;
}
