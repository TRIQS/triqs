#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp>
#include <triqs/gfs/bz.hpp>
#include <triqs/gfs/m_tail.hpp>

namespace h5 = triqs::h5;
using namespace triqs::gfs;
using namespace triqs::clef;
using namespace triqs::arrays;
using namespace triqs::lattice;
using local::tail;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> " << (X) << std::endl << std::endl;

// THE NAME bz is TOO SHORT

int main() {
 try {
  double beta = 1;

  auto bz_ = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};

  auto t = tail{1,1};
  auto G = gf<bz, tail>{{bz_, 100}};

  // try to assign to expression 
  placeholder<0> k_;
  placeholder<1> w_;
  auto eps = make_expr( [](k_t const& k) { return -2 * (cos(k(0)) + cos(k(1))); }) ;

  G(k_) << eps(k_) * t;

  // hdf5 
  h5::file file("ess_g_k_tail.h5", H5F_ACC_TRUNC );
  h5_write(file, "g", G);

 
 }
 TRIQS_CATCH_AND_ABORT;
}
