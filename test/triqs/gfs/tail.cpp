#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::gfs;
using namespace triqs::arrays;
using namespace triqs::utility;
using namespace triqs::clef;

TEST(Gf, Exceptions) {

  double beta = 10;

  auto g = gf<imfreq>{ {beta, Fermion, 1000}, make_shape(1,1)};
 

  placeholder<0> w_;
  g(w_) << 1/(w_ +1);

  std::cout << get_tail(g).first;
}
MAKE_MAIN;



