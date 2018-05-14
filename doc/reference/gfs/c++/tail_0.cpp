#include <triqs/gfs.hpp>
#include <triqs/gfs/singularity/fit_tail.hpp>
using namespace triqs::gfs;

int main() {
  triqs::clef::placeholder<0> iom_;
  double beta = 10;
  int N       = 100;

  auto gw = gf<imfreq>{{beta, Fermion, N}, {1, 1}};
  gw(iom_) << 1 / (iom_ - 1);

  size_t n_min   = 50; // linear index on mesh to start the fit
  size_t n_max   = 90; // final linear index for fit (included)

  std::cout << get_tail(gw) << std::endl;
}
