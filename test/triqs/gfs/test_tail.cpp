#include <triqs/gfs.hpp>

using namespace triqs::gfs;
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

// Test from issue #
int main() {

 triqs::clef::placeholder<0> iom_;
 double beta =10;
 int N=100;

 auto g = gf<imfreq>{{beta, Fermion, N},{1,1}};
 triqs::clef::placeholder<0> w_;
 g(w_) << w_ + 0.2;
 TEST(g.singularity());
 g(w_) << g(w_);
 TEST(g.singularity());

}

