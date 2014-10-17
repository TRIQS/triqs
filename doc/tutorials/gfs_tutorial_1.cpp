#include <triqs/gfs.hpp>
using namespace triqs::gfs;

int main() {
 double tmin = 0, tmax = 10;            // the time interval
 const int n = 2, m = 2, n_times = 100; // we will have 100 points
 auto g = gf<retime, matrix_valued>{{tmin, tmax, n_times}, {n, m}};
}

