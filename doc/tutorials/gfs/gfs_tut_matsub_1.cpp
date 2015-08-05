#include <triqs/gfs.hpp>
using namespace triqs::gfs;
using triqs::clef::placeholder;

int main(){

 double beta=1;//inverse temperature
 int nw=100;//number of Matsubara frequencies
 auto g = gf<imfreq>{{beta, Fermion, nw},{1,1}};//{1,1} : 1x1 Green's function

 //the shortest way to fill a gf
 placeholder<0> w_;
 g(w_) << 1/(w_-3);
 std::cout <<g(0) << std::endl;

 g()=0.0;
 //an equivalent way
 for(auto const & w : g.mesh())  g[w] = 1/(w-3);
 std::cout <<g(0) << std::endl;

 //an incorrect way : throws exception as expected
 //g(w) returns a const_view: () are to be used for interpolation, see bottom of the page
 //for(auto const & w : g.mesh())  g(w) = 1/(w-3);
}
