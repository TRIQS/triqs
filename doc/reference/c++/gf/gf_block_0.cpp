#include <triqs/gfs.hpp>
using namespace triqs::gfs;
using triqs::clef::placeholder;
int main() {
 double beta = 1;

 // Make a block gf of 3 gf, which are empty (default constructed).
 auto Bg0 = block_gf<imfreq>(3);

 // make a few gf<imfreq> ...
 auto g1 = gf<imfreq>({beta, Fermion}, {2, 2});

 // ... and construct some block function out of them.
 auto Bg1 = make_block_gf<imfreq>(3, g1);
 auto Bg2 = make_block_gf<imfreq>({g1, g1, g1});
 auto Bg3 = make_block_gf<imfreq>({"a", "b", "c"}, {g1, g1, g1});

 // do something on all blocks
 for (auto const &g : Bg1) {
  std::cout << g.mesh() << std::endl;
 }

 // multiply them by 1,2,3  ...
 auto i = 1;
 for (auto &g : Bg1) g = g * i++;

 // a little save in an hdf5 file ?
 H5::H5File file("test_block_gf.h5", H5F_ACC_TRUNC);
 h5_write(file, "B3", Bg3);
}

