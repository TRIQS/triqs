#include <triqs/clef.hpp>
#include <triqs/gfs.hpp>
#include <triqs/mc_tools/random_generator.hpp>
#include <triqs/statistics.hpp>
using namespace triqs::statistics;
using triqs::clef::placeholder;
using namespace triqs::gfs;
int main() {

  //generate Green's functions with random noise
  triqs::mc_tools::random_generator RND;
  placeholder<0> w_;
  auto gw = gf<imfreq, scalar_valued>{{10, Fermion, 200}};
  gw(w_) << 1 / (w_ - 2.0);
  int n_tau = 41;
  auto gt   = gf<imtime, scalar_valued>{{10, Fermion, n_tau}};
  gt()      = fourier(gw);

  int n_measures = 20;
  std::vector<gf<imtime, scalar_valued>> G_measurements(n_measures, gt);
  for (auto &g : G_measurements)
    for (auto const &t : g.mesh()) g[t] += RND(0.1) - 0.05; //adding uniform noise of 0.05

  //put the generated Green's functions into a vector of observables
  std::vector<observable<dcomplex>> gt_as_observable(n_tau);
  int i_tau = 0;
  for (auto &o : gt_as_observable) {
    for (int i = 0; i < n_measures; i++) o << G_measurements[i][i_tau];
    i_tau++;
  }
  //compute the average and error bar
  i_tau = 0;
  std::cout << "Exact value \t Average +/- Error" << std::endl;
  for (auto const &o : gt_as_observable) {
    std::cout << gt[i_tau] << "\t" << average_and_error(o) << std::endl;
    i_tau++;
  }
  return 0;
}
