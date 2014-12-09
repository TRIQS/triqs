#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp> 
using namespace triqs::gfs;
using namespace triqs::arrays;
namespace h5 = triqs::h5;
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;
#include <triqs/gfs/local/functions.hpp> 

int main() {
 try { 

  double beta =1;

  auto G =  gf<imfreq>{ {beta, Fermion}, {2,2} };
  auto Gc = gf<imfreq>{ {beta, Fermion}, {2,2} };
  auto G3 = gf<imfreq>{ {beta, Fermion}, {2,2} };
  auto Gt = gf<imtime>{ {beta, Fermion,100}, {2,2} };

  auto Gv = G();
  TEST( G( 0) ) ;
  Gv.on_mesh(0) = 20;
  TEST( Gv( 0) ) ;
  TEST( G( 0) ) ;
  Gv.on_mesh(0) = 0;

  auto Gv2 = slice_target(G(),range(0,1),range(0,1));
  TEST( Gv2( 0) ) ;
  Gv2.on_mesh(0) = 10;
  TEST( Gv2( 0) ) ;
  TEST( G( 0) ) ;

  triqs::clef::placeholder<0> om_;

  TEST( G(om_) ) ;
  TEST( eval(G(om_), om_=0) ) ;

  TEST( Gv(om_) ) ;
  TEST( eval(Gv(om_), om_=0) ) ;

  std::cout  <<"-------------lazy assign 1 ------------------"<<std::endl;

  Gv(om_) << (0.2 + om_ + 2.1);
  TEST(G(0));
  TEST(G.singularity());

  std::cout  <<"-------------lazy assign 2 ------------------"<<std::endl;

  G(om_) << 1/(om_ + 2.3);

  TEST(G(0));
  TEST(G.singularity());
  TEST(inverse(G.singularity()));

  std::cout  <<"-----------------   3 --------------------"<<std::endl;

  TEST( Gv(om_) ) ;
  TEST( eval(Gv(om_), om_=0) ) ;

  // tail 
  auto  t = G.singularity();

  TEST(t.order_min()); 
  TEST( t( 2) ) ;

  TEST( Gv2.singularity()( 2) ) ;

  // copy 
  Gc = G;
  TEST( G( 0) ) ;
  TEST( Gc( 0) ) ;

  // error
  //TEST (G(1.0_j));

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
  TEST(G.singularity()(2));

  TEST( ( G.singularity() + G.singularity() )  (2));
  TEST( ( G.singularity() * G.singularity() )  (4));

  TEST( t(1));

  //tqa::array<double,9> A(1,2,3,4,5,6,7,8,9); A()=0;
  //auto x = local::impl::gf_impl<triqs::gfs::meshes::imfreq, true>::wrap_infty (G.tail_view()) + 2.0;

  // test hdf5 
  h5::file file("ess_gf.h5", H5F_ACC_TRUNC );
  h5_write(file, "g", G);

  //
  {
   auto G0w = gf<imfreq, scalar_valued>{{beta, Fermion, 100}};
   auto D0w = gf<imfreq, scalar_valued>{{beta, Boson, 100}};
   auto Sigma_w = gf<imfreq, scalar_valued>{{beta, Fermion, 100}};
   G0w(om_) << 1 / (om_ - 3);

   for (auto const& nu : D0w.mesh()) Sigma_w(om_) << 2 * G0w(om_ - nu);
  }

  //
  {
   auto G0w = gf<imfreq, matrix_valued>{{beta, Fermion, 100}, {1,1}};
   auto D0w = gf<imfreq, matrix_valued>{{beta, Boson, 100}, {1,1}};
   auto Sigma_w = gf<imfreq, matrix_valued>{{beta, Fermion, 100}, {1,1}} ;
   G0w(om_) << 1 / (om_ - 3);

   for (auto const& nu : D0w.mesh()) Sigma_w(om_) << 2 * G0w(om_ - nu);
  }
 }
 TRIQS_CATCH_AND_ABORT;

}
