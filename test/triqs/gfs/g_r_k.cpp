#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp>
#include <triqs/gfs/bz.hpp>
#include <triqs/gfs/cyclic_lattice.hpp>
#include <triqs/gfs/local/fourier_lattice.hpp>

namespace h5 = triqs::h5;
using namespace triqs::gfs;
using namespace triqs::clef;
using namespace triqs::arrays;
using namespace triqs::lattice;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> " << (X) << std::endl << std::endl;

int main() {
 try {
  int N = 10;
  int S = 2;
  placeholder<0> k_;
  //placeholder<1> r_;

  auto bz_ = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};
  auto gk = gf<bz>{{bz_, N}, {S, S}};

  gk(k_) << -2 * (cos(k_(0)) + cos(k_(1)));

  auto gr = gf<cyclic_lattice>{{N, N}, {S, S}};

  gr() = inverse_fourier(gk); // ADD Security if gf =

  // for (auto k : gk.mesh()) if (max_element(abs(gk[k])) > 1.e-10) std::cout << k << gk[k] << std::endl;

  for (auto r : gr.mesh())
   if (max_element(abs(gr[r])) > 1.e-10) std::cout << r << gr[r] << std::endl;

  // reverse transfo
  auto gk2 = gk;
  gk2() = fourier(gr);
  if (max_element(abs(gk.data() - gk2.data())) > 1.e-13) TRIQS_RUNTIME_ERROR << "fourier pb";

  // check gr
  auto gr_test = gf<cyclic_lattice>{{N, N}, {S, S}};
  gr_test[{1,0,0}] = -1;
  gr_test[{-1,0,0}] = -1;
  gr_test[{0,1,0}] = -1;
  gr_test[{0,-1,0}] = -1;
  
  if (max_element(abs(gr.data() - gr_test.data())) > 1.e-13) TRIQS_RUNTIME_ERROR << "fourier inverse pb"; // << gr.data() << gr_test.data();
  
  // hdf5
  h5::file file("ess_g_r_k.h5", H5F_ACC_TRUNC);
  h5_write(file, "gr", gr);
  h5_write(file, "gk", gk);
 }
 TRIQS_CATCH_AND_ABORT;
}
