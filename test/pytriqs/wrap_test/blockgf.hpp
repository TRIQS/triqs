#include <triqs/gfs.hpp>

using namespace triqs::gfs;

template<typename... X, typename... Y>
void fct_2_blocks(block_gf_view<X...> x, block_gf_view<Y...> y, double precision = 1.e-10) {
 if (n_blocks(x) != n_blocks(y)) throw "Block GFs have different number of blocks";
}
