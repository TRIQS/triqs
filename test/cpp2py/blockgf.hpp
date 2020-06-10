#include <triqs/gfs.hpp>

using namespace triqs::gfs;

template <typename M, typename T> void fct_2_blocks(block_gf_view<M,T> x, block_gf_view<M,T> y, double precision = 1.e-10) {
  if (x.size() != y.size()) throw "Block GFs have different number of blocks";
}
