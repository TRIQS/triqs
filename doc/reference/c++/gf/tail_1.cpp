#include <triqs/gfs.hpp>
#include <triqs/gfs/local/fit_tail.hpp>
using namespace triqs::gfs;
int main() {
 triqs::clef::placeholder<0> iom_;
 double beta = 10;
 int N = 100;

 auto gw = gf<imfreq>{{beta, Fermion, N}, {1, 1}};
 gw(iom_) << 1 / (iom_ - 1);

 size_t n_min = 50; // linear index on mesh to start the fit
 size_t n_max = 90; // final linear index for fit (included)
 int n_moments = 4; // number of moments in the final tail (including known ones)
 int size = 1; // means that we know one moment
 int order_min = 1; // means that the first moment in the final tail will be the first moment
 auto known_moments = local::tail(make_shape(1, 1), size, order_min); // length is 0, first moment to fit is order_min
 known_moments(1) = 1.; // set the first moment
 set_tail_from_fit(gw, known_moments, n_moments, n_min, n_max,
                   true); // true replace the gf data in the fitting range by the tail values
 std::cout << gw.singularity() << std::endl;
}

