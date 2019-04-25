#include <triqs/gfs.hpp>
#include <triqs/clef.hpp>
using namespace triqs::gfs;
using namespace triqs::clef;

int main() {
  double beta = 10;  // the inverse temperature
  int n_freq  = 500; // we will have 500 frequencies
  auto g      = gf<imfreq>{{beta, Fermion, n_freq}, {1, 1}};
  placeholder<0> om_;
  g(om_) << 1 / (om_ - 2);

  std::cout << "g(0) = " << g[0] << std::endl;

  //print to hdf5
  triqs::h5::file hfile("my_gf.h5", 'w');
  h5_write(hfile, "g_w", g);
  hfile.close();

  //read from hdf5
  triqs::h5::file hfile2("my_gf.h5", H5F_ACC_RDWR);
  gf<imfreq> g2;
  h5_read(hfile2, "g_w", g2);
  hfile2.close();

  std::cout << "g2(0) = " << g2[0] << std::endl;
}
