#include <triqs/gfs.hpp>
#include <iomanip>

using namespace triqs::gfs;
triqs::clef::placeholder<0> iw_;

int main() {
  double beta = 1.0;
  int n_iw    = 100;
  auto g      = gf<imfreq>{{beta, Fermion, n_iw}, {1, 1}};
  g[iw_] << 1.0 / iw_;
  auto [tail, err] = fit_tail(g);
  std::cout << std::setprecision(2) << "Error: " << err << "\nTail:  " << tail;
}
