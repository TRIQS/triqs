#include <triqs/gfs.hpp>
#include <triqs/gfs/local/functions.hpp>
using namespace triqs::gfs;
using namespace triqs::arrays;

struct wgf {
 void operator()() {
  double beta = 10;
  int Nfreq = 10000000;
  auto g = gf<imfreq, scalar_valued>{{beta, Fermion, Nfreq}};
  auto a = array<std::complex<double>,1>(Nfreq);
  for (int u = 0; u < 50; ++u) {
   for (int n = 0; n < Nfreq; ++n) {
    g[n] +=1;
    //g.data()(n) += 1;
    //a(n) += 1;
   }
  }
 }
};

struct vec {
 void operator()() {
  double beta = 10;
  int Nfreq = 10000000;
 //std::vector<float> V(Nfreq);
 std::vector<std::complex<double>> V(Nfreq);
  for (int u = 0; u < 50; ++u) {
   for (int n = 0; n < Nfreq; ++n) V[n] += 1;
  }
 }
};


#include "./speed_tester.hpp"
int main() {
 const int l = 10;

 std::cout  << "----------- -------------------"<< std::endl ;

 speed_tester<vec> (l);
 speed_tester<wgf> (l);
}

 



