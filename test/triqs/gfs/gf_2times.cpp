#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp> 
using namespace triqs::gfs;
using namespace triqs::arrays;
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

int main() {

 try {

 auto m = gf_mesh<retime>{0,10,99,full_bins};
 auto G  = gf<cartesian_product<retime,retime>> { {m,m}, {2,2}};
 auto G2 = G;

 auto gg  = gf<retime> { {m}, {2,2}};
 
 triqs::clef::placeholder<0> t_;
 triqs::clef::placeholder<1> tp_;

 array<double,2> A(2,2);
 A(t_,tp_)  << t_ - 3*tp_;
 std::cout  <<A << std::endl ;

 std::cout  << G.data().shape() << 2*G(0,0)<<std::endl;

 auto xx =eval ( G(t_,tp_), t_=2, tp_=1.2);
//xx =0;
 std::cout  << eval ( gg(t_), t_=2)<< std::endl ;

 double beta = 1;
 double wmin=0.;
 double wmax=1.0;
 int n_im_freq=100;
 
 auto G_w_wn2 = gf<cartesian_product<imfreq,imfreq>>( {gf_mesh<imfreq>(beta, Fermion, n_im_freq), gf_mesh<imfreq>(beta, Fermion, n_im_freq)}, {2,2});

 auto zz = G_w_wn2(t_,tp_);
 //std::cout  << std::get<0>(zz.childs).data() << std::endl ;
 auto yy = eval ( zz, t_=2, tp_=3);
 std::cout  << yy.indexmap()<< std::endl ;
 std::cout  << yy << std::endl ;
 //std::cout  << eval ( zz, t_=2, tp_=3)<< std::endl ;
 //std::cout  << eval ( G_w_wn2(t_,tp_), t_=2, tp_=3)<< std::endl ;

 //std::cout  << eval ( G(t_,tp_), t_=2, tp_=1.2)<< std::endl ;
 //G(t_,tp_)  << t_ - 3*tp_;
 //G2(t_,tp_) << t_ + 3*tp_;
  
 //G2(t_,tp_) << 2* G(t_,tp_);

 TEST( G(1,1) );
 TEST( G[G.mesh()(1,1) ]);
 TEST( G.on_mesh(1,1));

 //G2(t_,tp_) << G(tp_,t_);
 TEST( G(2,1) );
 TEST( G2(1,2) );
 TEST( G(1,2) );
 TEST( G2(2,1) );

 //TEST( G2(2,1,3) ); // should not compile
 
 }
 TRIQS_CATCH_AND_ABORT;
}
