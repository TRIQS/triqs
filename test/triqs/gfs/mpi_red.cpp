#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp> 
#include <boost/serialization/complex.hpp>
using namespace triqs::gfs;
using namespace triqs::arrays;
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;
#include <triqs/gfs/local/functions.hpp> 
#include <boost/mpi.hpp>

int main(int argc, char* argv[]) {
 boost::mpi::environment env(argc,argv);
 boost::mpi::communicator c;

 double beta =1;
 auto G = gf<imfreq> {{beta, Fermion}, {2,2}};
 triqs::clef::placeholder<0> om_;
 //G(om_) << (om_ - 2.1);
 auto G2 = G;
 std::cout << c.rank() << "\t"<<  G.singularity()<< std::endl;
 boost::mpi::reduce(c, G, G2, std::plus<gf<imfreq>>(), 0);
 std::cout << c.rank() << "\t" << G2.singularity()<< std::endl;

 G(om_) << (om_ - 2.1);

 //std::cout << c.rank() << "\t"<<  G.singularity()<< std::endl;
 //boost::mpi::reduce(c, G, G2, std::plus<gf<imfreq>>(), 0);
// std::cout << c.rank() << "\t" << G2.singularity()<< std::endl;

 
 auto g3 = G2 + G;

 std::cout << c.rank() << "\t" << g3.singularity()<< std::endl;


 //G(om_) << (om_ - 2.1);
 auto g4 = g3 + G;
 std::cout << c.rank() << "\t" << g4.singularity()<< std::endl;
 std::cout << c.rank() << "\t" << g3.singularity() + g4.singularity()<< std::endl;
 
}
