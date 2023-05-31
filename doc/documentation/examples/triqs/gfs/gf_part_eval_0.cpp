#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
using namespace triqs::gfs;
using namespace triqs;
using nda::clef::placeholder;
int main() {
  double beta = 1, tmin = 0, tmax = 1.0;
  int n_re_time = 5, n_im_time = 10;

  using g_t_tau_s = gf<prod<retime, imtime>, scalar_valued>;

  // a scalar valued function
  auto g = g_t_tau_s{{{tmin, tmax, n_re_time}, {beta, Fermion, n_im_time}}};

  auto _ = all_t{}; // a dedicated placeholder for partial evaluation

  auto g_sliced = g[3, _]; // 3 is the index of the point

  // or if we loop over the first component of the mesh of g
  for (auto w : std::get<0>(g.mesh())) {
    std::cout << g[w, _] << std::endl; // or do anything with it ...
  }
}
