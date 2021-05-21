#include <nda/nda.hpp>
#include <iostream>

using namespace triqs;
using namespace nda;
using namespace mpi;

int main(int argc, char *argv[]) {

  mpi::environment env(argc, argv);
  mpi::communicator world;

  int a = 5;
  mpi_broadcast(a);
  a = mpi_reduce(a);

  array<int, 2> A(2, 10);
  A() = 1;

  std::cout << A << std::endl;

  mpi_scatter(A);
  A += world.rank();
  mpi_gather(A);

  std::cout << A << std::endl;
}
