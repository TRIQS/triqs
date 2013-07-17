//#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/gf/two_real_times.hpp> 

//using namespace triqs::gf::local;
using namespace triqs::gf;
namespace tql= triqs::clef;
//namespace tqa= triqs::arrays;
using tqa::range;
using triqs::arrays::make_shape;
using triqs::arrays::array;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

int main() {

 //typedef gf<two_real_times> Gf_type;
 //typedef gf_view<two_real_times> Gf_view_type;

 auto G  = make_gf<two_real_times>( 10,100,make_shape(2,2));
 auto G2 = make_gf<two_real_times>( 10,100,make_shape(2,2));

 //Gf_type G   (two_real_times::mesh_t(10,100),make_shape(2,2));
 //Gf_type G2  (two_real_times::mesh_t(10,100),make_shape(2,2));

 triqs::clef::placeholder<0> t_;
 triqs::clef::placeholder<1> tp_;

 array<double,2> A(2,2);
 A(t_,tp_)  << t_ - 3*tp_;
 std::cout  <<A << std::endl ;

 G(t_,tp_)  << t_ - 3*tp_;
 //G2(t_,tp_) << t_ + 3*tp_;

 TEST( G(1,1) );
 TEST( G(G.mesh()(1,1)) );
 TEST( G.on_mesh(1,1));

 //G2(t_,tp_) << G(tp_,t_);
 TEST( G(2,1) );
 TEST( G2(1,2) );
 TEST( G(1,2) );
 TEST( G2(2,1) );

 //TEST( G2(2,1,3) ); // should not compile
 
}
