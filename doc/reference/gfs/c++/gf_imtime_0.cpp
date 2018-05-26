#include <triqs/gfs.hpp>
using namespace triqs::gfs;
int main() {
 double beta = 10, a = 1;
 int n_times = 1000;

 // --- first a matrix_valued function ------------

 // First give information to build the mesh, second to build the target
 auto g1 = gf<imtime, matrix_valued>{{beta, Fermion, n_times}, {1, 1}};

 // or a more verbose/explicit form ...
 auto g2 = gf<imtime>{gf_mesh<imtime>{beta, Fermion, n_times}, make_shape(1, 1)};

 triqs::clef::placeholder_prime<0> tau_;
 g1(tau_) << exp(-a * tau_) / (1 + exp(-beta * a));

 // evaluation at tau=3.2
 std::cout << triqs::arrays::make_matrix(g1(3.2)) << " == " << exp(-a * 3.2) / (1 + exp(-beta * a)) << std::endl;

 // --- a scalar_valued function ------------

 // same a before, but without the same of the target space ...
 auto g3 = gf<imtime, scalar_valued>{{beta, Fermion, n_times}};

 g3(tau_) << exp(-a * tau_) / (1 + exp(-beta * a));

 // evaluation at tau=3.2
 std::cout << g3(3.2) << " == " << exp(-a * 3.2) / (1 + exp(-beta * a)) << std::endl;
}

