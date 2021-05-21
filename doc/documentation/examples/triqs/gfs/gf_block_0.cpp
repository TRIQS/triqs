#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
using namespace triqs::gfs;
using namespace triqs::mesh;
using nda::clef::placeholder;
int main() {
  double beta = 1;

  // Make a block gf of 3 gf, which are empty (default constructed).
  auto Bg0                             = block_gf<imfreq>(3);
  auto Bg1                             = block_gf<imfreq>({"a", "b", "c"});
  std::vector<std::string> block_names = {"a", "b", "c"};
  auto Bg11                            = block_gf<imfreq>(block_names);

  // make a few gf<imfreq> ...
  auto g1 = gf<imfreq>({beta, Fermion}, {2, 2});

  // ... and construct some block function out of them.
  auto Bg2 = make_block_gf<imfreq>(3, g1);
  auto Bg3 = make_block_gf<imfreq>({g1, g1, g1});
  auto Bg4 = make_block_gf<imfreq>({"a", "b", "c"}, {g1, g1, g1});

  // do something on all blocks
  for (auto const &g : Bg2) { std::cout << g.mesh() << std::endl; }

  // multiply them by 1,2,3  ...
  auto i = 1;
  for (auto &g : Bg2) g = g * i++;

  // a little save in an hdf5 file ?
  h5::file file("test_block_gf.h5", 'w');
  h5_write(file, "B4", Bg4);
}
