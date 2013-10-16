#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp> 
using namespace triqs::gfs;
using namespace triqs::arrays;
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;
#include <triqs/gfs/local/fourier_matsubara.hpp> 
#include<fstream>
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

void print_to_file(std::string const s, gf<imtime> const & gt){
 std::ofstream mfile(s);
 if(mfile.is_open()){
  for(int i=0;i<gt.mesh().size();i++){
   mfile << gt.mesh().index_to_point(i) << "\t" << gt.data()(i,0,0) << std::endl;
  }
  mfile.close();
 }
 else{
  throw std::runtime_error("Could not print to file");
 }
}

void test_0(){
 
 int Ntau = 10001;
 double beta =1;

 /* ---------- construct a Green's function ---------*/

 auto G =  gf<imfreq> {{beta, Fermion, 100}, {1,1}};
 triqs::clef::placeholder<0> om_;
 G(om_) << 1./(om_ - 2.1);

 /* ---------- Fourier transform ---------------------*/
 auto Gt = gf<imtime> {{beta, Fermion, Ntau, full_bins}, {1,1}};
 Gt() = lazy_inverse_fourier(G);

 TEST(Gt(0.0));

 TEST(Gt.data());
 TEST(Gt.mesh().index_to_point(0));
 TEST(Gt.mesh().index_to_point(1));

}

void test_1(){


 double beta=10;
 /* ----- Fourier ----- */
 auto Gt =  gf<imtime> {{beta, Fermion, 100,full_bins}, {1,1}};
 auto Gw =  gf<imfreq> {{beta, Fermion, 100}, {1,1}};
 Gw.singularity()(1) = 1;

 Gt() = lazy_inverse_fourier(Gw);
}

int main() {

 test_0();
 test_1();

}




