//#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/gf/imfreq.hpp> 
#include <triqs/gf/imtime.hpp> 
#include <triqs/gf/local/fourier_matsubara.hpp> 
#include<fstream>
namespace tql= triqs::clef;
namespace tqa= triqs::arrays;
using tqa::range;
using triqs::arrays::make_shape;
using triqs::gfs::Fermion;
using triqs::gfs::gf;
using triqs::gfs::imfreq;
using triqs::gfs::imtime;
using triqs::gfs::make_gf;
using triqs::gfs::full_bins;
using triqs::gfs::half_bins;

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

 auto G =  make_gf<imfreq> (beta, Fermion, make_shape(1,1), 100);
 triqs::clef::placeholder<0> om_;
 G(om_) << 1./(om_ - 2.1);


 //auto omega_mesh = make_mesh();
 //auto tail = ;
 //auto G2 =  make_gf(omega_mesh, make_shape(1,1), tail) ;


 /* ---------- Fourier transform ---------------------*/
 auto Gt = make_gf<imtime> (beta, Fermion, make_shape(1,1), Ntau, full_bins);
 Gt() = lazy_inverse_fourier(G);

 TEST(Gt(0.0));

 TEST(Gt.data());
 //TEST(Gt.mesh());//does not work: mesh has no <<
 TEST(Gt.mesh().index_to_point(0));
 TEST(Gt.mesh().index_to_point(1));


 /* -------- Read/write -----------------------------*/
 /* print_to_file("Gtau.dat", Gt);
 H5::H5File hfile("store_G.h5",H5F_ACC_TRUNC);
 h5_write(hfile, "G_tau", Gt); 
 auto Gt2 = make_gf<imtime> (beta, Fermion, make_shape(1,1));
 h5_read(hfile, "G_tau", Gt2);

 H5::H5File file2("store_G2.h5",H5F_ACC_TRUNC);
 h5_write(file2, "G_tau", Gt2);

 //TEST(Gt==Gt2);//does not work: no == operator
*/

 /* ------------ other example ----------*/

 //auto G =  make_gf<imfreq> (beta, Fermion, make_shape(1,1));
 //triqs::clef::placeholder<0> om_;
 //triqs::vec
 //G(om_) << 1./(om_ - 2.1);


 //auto Gt = make_gf<imtime> (beta, Fermion, make_shape(2,2), n_tau, full_bins);
 //auto Gt = make_gf<imfreq> (beta, Fermion, make_shape(2,2), n_max);
}

void test_1(){


 double beta=10;
 /* ----- Fourier ----- */
 size_t N1=1;
 size_t N2=1; 
 size_t size_ = 5;
 long order_min=-1;
 triqs::gfs::local::tail t(N1,N2, size_, order_min);
 t(1)=1;

 auto Gt =  make_gf<imtime> (beta, Fermion, make_shape(1,1),100,full_bins, t);
 auto Gw =  make_gf<imfreq> (beta, Fermion, make_shape(1,1),100, t);

 Gt() = lazy_inverse_fourier(Gw);
}

int main() {

 test_0();
 test_1();

}




