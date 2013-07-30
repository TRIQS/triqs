//#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/gfs/imfreq.hpp> 
#include <triqs/gfs/imtime.hpp> 
#include <triqs/gfs/local/functions.hpp> 

namespace tql= triqs::clef;
namespace tqa= triqs::arrays;
using tqa::range;
using triqs::arrays::make_shape;
using triqs::gfs::Fermion;
using triqs::gfs::imfreq;
using triqs::gfs::imtime;
using triqs::gfs::make_gf;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

 // example 
 //template<typename T> using block_gf = gf<block_index, gf<T>>;
 // block_gf<imtime> ... 
 // but not on gcc 4.6 !
 //

int main() {

 triqs::gfs::freq_infty inf;

 double beta =1;
 auto G =  make_gf<imfreq> (beta, Fermion, make_shape(2,2));
 auto Gc = make_gf<imfreq> (beta, Fermion, make_shape(2,2));
 auto G3 = make_gf<imfreq> (beta, Fermion, make_shape(2,2));
 auto Gt = make_gf<imtime> (beta, Fermion, make_shape(2,2));

 auto Gv = G();
 TEST( G( 0) ) ;
 Gv.on_mesh(0) = 20;
 TEST( Gv( 0) ) ;
 TEST( G( 0) ) ;
 Gv.on_mesh(0) = 0;

 auto Gv2 = slice_target(G,range(0,1),range(0,1));
 TEST( Gv2( 0) ) ;
 Gv2.on_mesh(0) = 10;
 TEST( Gv2( 0) ) ;
 TEST( G( 0) ) ;

 triqs::clef::placeholder<0> om_;

 TEST( G(om_) ) ;
 TEST( tql::eval(G(om_), om_=0) ) ;

 TEST( Gv(om_) ) ;
 TEST( tql::eval(Gv(om_), om_=0) ) ;

 std::cout  <<"-------------lazy assign 1 ------------------"<<std::endl;

 Gv(om_) << (0.2 + om_ + 2.1);
 TEST(G(0));
 TEST(G(inf));

 std::cout  <<"-------------lazy assign 2 ------------------"<<std::endl;

 G(om_) << 1/(om_ + 2.3);

 TEST(G(0));
 TEST(G(inf));
 TEST(inverse(G(inf)));

 std::cout  <<"-----------------   3 --------------------"<<std::endl;

 TEST( Gv(om_) ) ;
 TEST( tql::eval(Gv(om_), om_=0) ) ;

 // tail 
 BOOST_AUTO( t, G(inf));
 //local::gf<meshes::tail> t2 = t + 2.4;

 TEST(t.order_min()); 
 TEST( t( 2) ) ;

 TEST( Gv2(inf)( 2) ) ;

 // copy 
 Gc = G;
 TEST( G( 0) ) ;
 TEST( Gc( 0) ) ;


 // operations on gf
 G3 = G +2* Gc;
 G3 = G + Gc;
 
// TEST( G3( 0) ) ;
//  G3 = G3 /2.0;
// TEST( G3( 0) ) ;

 // does not compile : ok 
 // G3 = G + Gt;
 //
 std::cout  <<"-----------------   4 --------------------"<<std::endl;

 // test for density
 TEST( density(G3) );

 // should not compile
 //G3 = G + Gt;

 //#define ALL_TEST
#ifdef ALL_TEST
 for (int u=0; u<10; ++u) { 
  TEST( (G + 2.0* Gc)( u) ) ;
  TEST( (8.0*G + 2.0* Gc)( u) ) ;
  TEST( (8.0*G  - 2.0* Gc)( u) ) ;
  TEST( (G - Gc)( u) ) ;
  TEST( (G - 2.0* Gc)( u) ) ;
  TEST( (G * Gc)( u) ) ;
 }
#endif
 TEST( G( 0) ) ;
 TEST(G(inf)(2));

 TEST( ( G(inf) + G(inf) )  (2));
 TEST( ( G(inf) * G(inf) )  (4));

 TEST( t(1));

 //tqa::array<double,9> A(1,2,3,4,5,6,7,8,9); A()=0;
 //auto x = local::impl::gf_impl<triqs::gfs::meshes::imfreq, true>::wrap_infty (G.tail_view()) + 2.0;

 // test hdf5 
 H5::H5File file("ess_gf.h5", H5F_ACC_TRUNC );
 h5_write(file, "g", G);


}
