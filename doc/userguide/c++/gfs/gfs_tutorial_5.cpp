#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
using namespace triqs::gfs;
using namespace triqs::mesh;

int main() {
  double tmin = 0, tmax = 1.0;
  int nt = 100;
  auto g = gf<prod<retime, retime>, scalar_valued>{{{tmin, tmax, nt}, {tmin, tmax, nt}}};
  nda::clef::placeholder<0> t1_;
  nda::clef::placeholder<1> t2_;
  g(t1_, t2_) << 2 * t1_;
}
