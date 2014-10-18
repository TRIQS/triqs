#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp>
#include <triqs/gfs/bz.hpp>
#include <triqs/gfs/m_tail.hpp>

namespace h5 = triqs::h5;
using namespace triqs::gfs;
using namespace triqs::clef;
using namespace triqs::arrays;
using namespace triqs::lattice;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> " << (X) << std::endl << std::endl;

// THE NAME bz is TOO SHORT

int main() {
 try {
  auto bz_ = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};

  auto t = tail{1,1};
  auto G = gf<bz, tail>{{bz_, 100}, {1,1}};

  placeholder<0> k_;
  G(k_) << -2 * (cos(k_(0)) + cos(k_(1))) * t;

  h5::file file("ess_g_k_tail.h5", H5F_ACC_TRUNC );
  h5_write(file, "g", G);
 
 }
 TRIQS_CATCH_AND_ABORT;
}
