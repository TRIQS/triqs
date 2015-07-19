#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp> 
using namespace triqs::gfs;
using namespace triqs::arrays;
namespace h5 = triqs::h5;
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;
#include <triqs/gfs/functions/functions.hpp> 

int main() {
 try { 

  double beta =1;

  auto G =  gf<imfreq>{ {beta, Fermion}, {2,2} };
  auto Gc = gf<imfreq>{ {beta, Fermion}, {2,2} };
  auto G3 = gf<imfreq>{ {beta, Fermion}, {2,2} };

  triqs::clef::placeholder<0> om_;
  
  G(om_) << om_ + 2 - G3(om_);
 
 G = -G3;

#ifndef TRIQS_CPP11

  //auto g4 = gf<imfreq, tensor_valued<3>>{};
  //auto g4 = gf<imfreq, tensor_valued<3>>{{beta,Fermion},{3,3,3}};

  auto G3_view = gf_view<imfreq>(G3);
  //TEST(G3_view[0].shape());
  //TEST(G3_view.data().shape());
 
 array<gf<imfreq>,2> A(2,2);
 array<gf<imfreq>,1> B(2);
 triqs::clef::placeholder<1> i_;
 triqs::clef::placeholder<2> j_;
 A(i_,j_) << gf<imfreq>{{1.0,Fermion},{1,1}};
 A(i_,j_)(om_) << 1/(om_-3);

 B(i_) << sum(A(i_,j_), j_=range(0,2));

 block_gf<imfreq> G2(2);
 //B(i_) << G2(i_);
 B(i_) << G2[i_];

  //auto g=gf<imfreq, matrix_valued>{{1.0,Fermion},{1,1}};
  auto g=gf<imfreq, matrix_valued, no_tail>{{1.0,Fermion},{1,1}};
  g(om_)(i_,j_) << 0.0;
  auto g2 = gf<cartesian_product<imfreq,imfreq>, matrix_valued>{{{1.0,Fermion},{1.0,Fermion}},{1,1}};
  triqs::clef::placeholder<3> nu_;
  //g2(om_,nu_)(i_,j_) << 0.0;
  //g2(om_,nu_) << 0.0;

#endif

 }
 TRIQS_CATCH_AND_ABORT;

}
