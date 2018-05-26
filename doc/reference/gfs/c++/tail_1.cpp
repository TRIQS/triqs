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
  int max_moment = 4;  // Highest moment in the final tail

  // Create a tail with moments -2 .. 8 (intialized to 0) and a given shape
  auto known_moments = tail(make_shape(1, 1));
  known_moments.reset(2); // set moments >=2 to NaN (unkonwn)
  known_moments(1) = 1.;  // set the first moment

  fit_tail(gw, known_moments, max_moment, n_min, n_max, true); // true replace the gf data in the fitting range by the tail values
  std::cout << gw.singularity() << std::endl;
}
