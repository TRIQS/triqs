#include <triqs/arrays.hpp>
#include <boost/mpi.hpp>
using triqs::arrays::array;
using triqs::clef::placeholder;
namespace mpi = triqs::mpi;

int main(int argc, char **argv) {
 mpi::environment env(argc, argv);
 mpi::communicator world;

 array<long, 2> A(2, 2), B(2, 2), C(2, 2);
 placeholder<0> i_;
 placeholder<1> j_;
 A(i_, j_) << (1 + world.rank()) * (10 * i_ + j_);

 if (world.rank() == 0) std::cout << " A = " << A << std::endl;

 C = mpi::reduce(A, world);

 int s = world.size();
 if (world.rank() == 0) std::cout << " C = " << C << "  should be " << array<long, 2>((s * (s + 1) / 2) * A) << std::endl;
}

