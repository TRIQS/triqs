#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
using namespace triqs::gfs;
using namespace triqs;

int main() {
   double beta = 1.0;
   int n_iw = 100;
   auto g = gf<imfreq>{{beta, Fermion, n_iw}, {1,1}};

   auto empty_tail = make_zero_tail(g, 2);
}
