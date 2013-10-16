#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp> 
using namespace triqs::gfs;
using namespace triqs::arrays;
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;
#include <triqs/gfs/local/functions.hpp> 
#include <triqs/utility/serialization.hpp>

int main() {

 double beta =1;
 auto G =  gf<imfreq>{ {beta, Fermion}, {2,2} };
 double x = 127;
 std::string s = triqs::serialize(x);
 
 std::cout  << " s = "<< s<< std::endl;
 std::cout  << triqs::deserialize<double>(s) << std::endl;
 std::cout  << triqs::deserialize<int>(s) << std::endl;

 std::vector<std::string> v; v.push_back("abc"); v.push_back("3");
 std::cout << triqs::serialize(v)<< std::endl;

}
