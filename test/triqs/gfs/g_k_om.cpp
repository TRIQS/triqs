#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp>
#include <triqs/gfs/bz.hpp>

namespace h5 = triqs::h5;
using namespace triqs::gfs;
using namespace triqs::clef;
using namespace triqs::arrays;
using namespace triqs::lattice;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> " << (X) << std::endl << std::endl;

int main() {
 try {
  double beta = 1;

  auto bz_ = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};

  auto g_eps = gf<bz>{{bz_, 1000}, {1, 1}};

  auto G = gf<cartesian_product<bz, imfreq>>{{{bz_, 100}, {beta, Fermion, 100}}, {1, 1}};

  // try to assign to expression 
  placeholder<0> k_;
  placeholder<1> w_;
  auto eps = make_expr( [](k_t const& k) { return -2 * (cos(k(0)) + cos(k(1))); }) ;

  G(k_, w_) << 1 / (w_ - eps(k_) - 1 / (w_ + 2));

  // hdf5 
  h5::file file("ess_g_k_om.h5", H5F_ACC_TRUNC );
  h5_write(file, "g", G);

 
 }
 TRIQS_CATCH_AND_ABORT;
}
