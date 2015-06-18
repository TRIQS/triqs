#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp>
#include <triqs/gfs/bz.hpp>
#include <triqs/gfs/m_tail.hpp>
#include "../common.hpp"

namespace h5 = triqs::h5;
using namespace triqs::gfs;
using namespace triqs::clef;
using namespace triqs::arrays;
using namespace triqs::lattice;
using triqs::utility::mindex;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> " << (X) << std::endl << std::endl;

int main() {
 try {
  double beta = 1;
  auto bz = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};

  int n_freq = 100;
  double t_min=-10, t_max = 10;

  int n_times = n_freq * 2 + 1;
  int L = 50;
  int n_bz = L;

  auto gkt = gf<cartesian_product<brillouin_zone, retime>, matrix_valued, no_tail>{
      {{bz, n_bz}, {t_min, t_max, n_times}}, {1, 1}};
  
  auto gxt = gf<cartesian_product<cyclic_lattice, retime>, matrix_valued, no_tail>{
   {{L,L}, {t_min, t_max, n_times}}, {1, 1}};

  placeholder<0> k_;
  placeholder<1> t_;

  auto eps_k = -2 * (cos(k_(0)) + cos(k_(1)));
  gkt(k_, t_) << exp(- 1_j * eps_k * t_);

  auto gx_t = curry<1>(gxt);
  auto gk_t = curry<1>(gkt);

  gx_t[t_] << inverse_fourier(gk_t[t_]);
 
  // hdf5
  h5::file file("ess_g_x_t.h5", H5F_ACC_TRUNC);
  h5_write(file, "g", gxt);

  std::cout << gxt(mindex(0,0,0),0.0)<<std::endl;
  std::cout << gxt(mindex(0,0,0),0.0)(0,0)<<std::endl;
 }
 TRIQS_CATCH_AND_ABORT;
}
