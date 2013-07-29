//#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/utility/serialization.hpp>
#include <triqs/gfs/imfreq.hpp> 
#include <triqs/gfs/imtime.hpp> 

namespace tql= triqs::clef;
namespace tqa= triqs::arrays;
using tqa::range;
using triqs::arrays::make_shape;
using triqs::gfs::Fermion;
using triqs::gfs::imfreq;
using triqs::gfs::imtime;
using triqs::gfs::make_gf;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

int main() {

 double beta =1;
 auto G =  make_gf<imfreq> (beta, Fermion, make_shape(2,2));

 double x = 127;
 std::string s = triqs::serialize(x);
 
 std::cout  << " s = "<< s<< std::endl;

 std::cout  << triqs::deserialize<double>(s) << std::endl;
 std::cout  << triqs::deserialize<int>(s) << std::endl;

std::vector<std::string> v; v.push_back("abc"); v.push_back("3");
std::cout << triqs::serialize(v)<< std::endl;

}
