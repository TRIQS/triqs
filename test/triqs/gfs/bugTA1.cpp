#include <triqs/gfs.hpp>
using namespace triqs::gfs;
int main(){
 double beta = 1;
 auto g = gf<imfreq, matrix_valued, no_tail>{{beta, Fermion, 100},{1,1}};
 tail t(1,1);
 auto gwt = make_gf_view_from_g_and_tail(g,t);
}
