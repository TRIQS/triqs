#include <triqs/gfs.hpp>
#include <triqs/gfs/singularity/fit_tail.hpp>
using namespace triqs::gfs;
int main() {
 triqs::clef::placeholder<0> iom_;
 double beta = 10;
 int N = 100;

 auto gw = gf<imfreq>{{beta, Fermion, N}, {1, 1}};
 gw(iom_) << 1 / (iom_ - 1);

 size_t n_min = 50, n_max = 90;
 int max_moment = 4;
 int size = 1; // means that we know one moment
 int order_min = 1; // means that the first moment in the final tail will be the first moment
 auto known_moments = tail(make_shape(1, 1), size, order_min); // length is 0, first moment to fit is order_min
 known_moments(1) = 1.; // set the first moment

 fit_tail(gw, known_moments, max_moment, n_min, n_max, true);

 std::cout << gw.singularity() << std::endl;
}

