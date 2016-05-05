#include <triqs/gfs.hpp>

using namespace triqs::gfs;

template<typename... X, typename... Y>
void fct_2_blocks(block_gf_view<X...> x, block_gf_view<Y...> y, double precision = 1.e-10) {
 if (x.size() != y.size()) TRIQS_RUNTIME_ERROR "Block GFs have different number of blocks";
}
