#//define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/gfs.hpp>
#include <triqs/gfs/local/fit_tail.hpp>

using triqs::arrays::make_shape;
using namespace triqs::gfs;
using triqs::gfs::local::tail;
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;
void test_0(){

 triqs::clef::placeholder<0> iom_;
 double beta =10;
 int N=100;

 auto gw = gf<imfreq>{{beta, Fermion, N},{1,1}};
 auto gw_s = gf<imfreq, scalar_valued>{{beta, Fermion, N}};

 triqs::arrays::array<double,1> c(3);
 triqs::clef::placeholder<1> i_;

 c(i_) << (2*i_+1);

 int size=0; //means we don't know any moments
 int order_min=1; //means that the first moment in the final tail will be the first moment
 auto known_moments = tail(make_shape(1,1), size, order_min); //length is 0, first moment to fit is order_min

 gw(iom_) << c(0)/iom_ + c(1)/iom_/iom_ + c(2)/iom_/iom_/iom_; 
 gw_s(iom_) << c(0)/iom_ + c(1)/iom_/iom_ + c(2)/iom_/iom_/iom_; 

 TEST(gw.singularity());

 //erase tail
 for(auto &i : gw.singularity().data()) i = 0.0;
 for(auto &i : gw_s.singularity().data()) i = 0.0;

 size_t wn_min=50; //frequency to start the fit
 size_t wn_max=90; //final fitting frequency (included)
 int n_moments=3;  //number of moments in the final tail (including known ones)

 //restore tail
 fit_tail(gw, known_moments, n_moments, wn_min, wn_max);
 fit_tail(gw_s, known_moments, n_moments, wn_min, wn_max);

 TEST(gw.singularity());
 TEST(gw_s.singularity());
/*
 for(size_t i=0; i<first_dim(c); i++){
  double diff = std::abs( c(i) - gw.singularity().data()(i,0,0) );
  //std::cout<< "diff: " << diff <<std::endl;
  if (diff > precision) TRIQS_RUNTIME_ERROR<<" fit_tail error : diff="<<diff<<"\n";
 }
*/

 //erase tail
 for(auto &i : gw.singularity().data()) i = 0.0;

 //now with a known moment
 size=1; //means that we know one moment
 order_min=1; //means that the first moment in the final tail will be the first moment
 known_moments = tail(make_shape(1,1), size, order_min); //length is 0, first moment to fit is order_min
 known_moments(1)=1.;//set the first moment
 fit_tail(gw, known_moments, n_moments, wn_min, wn_max, true);//true replace the gf data in the fitting range by the tail values

 TEST(gw.singularity());
/*
 for(size_t i=0; i<first_dim(c); i++){
  double diff = std::abs( c(i) - gw.singularity().data()(i,0,0) );
  //std::cout<< "diff: " << diff <<std::endl;
  if (diff > precision) TRIQS_RUNTIME_ERROR<<" fit_tail error : diff="<<diff<<"\n";
 }
 */
}

void test_1(){
 //real life test: find tails of 1/(iom -1)
 triqs::clef::placeholder<0> iom_;
 double beta =10;
 int N=100;

 auto gw = gf<imfreq>{{beta, Fermion, N}, {1, 1}};
 auto gw_b = gf<imfreq>{{beta, Boson, N}, {1, 1}};
 gw(iom_) << 1/(iom_-1);
 gw_b(iom_) << 1/(iom_-1);

 size_t wn_min=50; //frequency to start the fit
 size_t wn_max=90; //final fitting frequency (included)
 int n_moments=4;  //number of moments in the final tail (including known ones)
 int  size=1; //means that we know one moment
 int order_min=1; //means that the first moment in the final tail will be the first moment
 auto known_moments = tail(make_shape(1,1), size, order_min); //length is 0, first moment to fit is order_min
 known_moments(1)=1.;//set the first moment
 fit_tail(gw, known_moments, n_moments, wn_min, wn_max, true);//true replace the gf data in the fitting range by the tail values
 fit_tail(gw_b, known_moments, n_moments, wn_min, wn_max, true);//true replace the gf data in the fitting range by the tail values
 TEST(gw.singularity());
 TEST(gw_b.singularity());
}

void test_2(){
 //real life test: find tails of 1/(iom -1) -- with positive and negative matsubara
 triqs::clef::placeholder<0> iom_;
 double beta =10;
 int N=200;

 auto gw = gf<imfreq>{{beta, Fermion, N, false}, {1, 1}};
 gw(iom_) << 1/(iom_-1);

 size_t wn_min=50; //frequency to start the fit
 size_t wn_max=90; //final fitting frequency (included)
 int n_moments=4;  //number of moments in the final tail (including known ones)
 int  size=1; //means that we know one moment
 int order_min=1; //means that the first moment in the final tail will be the first moment
 auto known_moments = tail(make_shape(1,1), size, order_min); //length is 0, first moment to fit is order_min
 known_moments(1)=1.;//set the first moment
 fit_tail(gw, known_moments, n_moments, wn_min, wn_max, true);//true replace the gf data in the fitting range by the tail values
 TEST(gw.singularity());
}


int main() {
 test_0();
 test_1();
 test_2();
}


