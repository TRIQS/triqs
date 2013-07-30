//#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK

#include <triqs/gfs/refreq.hpp> 
#include <triqs/gfs/retime.hpp> 
#include <triqs/gfs/imfreq.hpp> 
#include <triqs/gfs/imtime.hpp> 

using triqs::gfs::refreq;
using triqs::gfs::retime;
using triqs::gfs::imfreq;
using triqs::gfs::imtime;
using triqs::gfs::make_gf;
using triqs::gfs::scalar_valued;
using triqs::gfs::Fermion;
using triqs::arrays::make_shape;
using triqs::arrays::range;
double precision=10e-12;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

int main() {

 double beta =1;
 double tmax=10;
 double wmax=10;
 int N=1000;
 
 auto Gw = make_gf<refreq> (-wmax, wmax, N, make_shape(2,2));
 auto Gt = make_gf<retime> (0, tmax, N, make_shape(2,2));
 auto Gw2 = make_gf<refreq,scalar_valued> (-wmax, wmax, N);
 auto Gt2 = make_gf<retime,scalar_valued> (0, tmax, N);
 auto Giw = make_gf<imfreq> (beta, Fermion, make_shape(2,2), N);
 auto Git = make_gf<imtime> (beta, Fermion, make_shape(2,2), N);
 auto Giw2 = make_gf<imfreq,scalar_valued> (beta, Fermion, N);
 auto Git2 = make_gf<imtime,scalar_valued> (beta, Fermion, N);

 int i =0;
 for (auto & t : Gt.mesh()) Gt[t] = 1.0*t;
 for (auto & w : Gw.mesh()) Gw[w] = 1.0*w;
 for (auto & it : Git.mesh()) Git[it] = 1.0*it;
 for (auto & iw : Giw.mesh()) Giw[iw] = 1.0*iw;
 
 triqs::clef::placeholder<0> t_;
 triqs::clef::placeholder<1> w_;
 Gt2( t_) << 1.0*t_;
 Gw2( w_) << 1.0*w_;
 Git2( t_) << 1.0*t_;
 Giw2( w_) << 1.0*w_;
 
 std::cout << "Gt(0.3352*tmax)(0,0)=" << Gt(0.3352*tmax)(0,0) << std::endl;
 if ( std::abs(Gt(0.3352*tmax)(0,0)-0.3352*tmax) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Gt\n";  
 std::cout << "Gt2(0.3352*tmax)=" << Gt2(0.3352*tmax) << std::endl;
 if ( std::abs(Gt2(0.3352*tmax)-0.3352*tmax) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Gt2\n";  
 std::cout << "Gw(0.3352*wmax)(0,0)=" << Gw(0.3352*wmax)(0,0) << std::endl;
 if ( std::abs(Gw(0.3352*wmax)(0,0)-0.3352*wmax) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Gw\n";  
 std::cout << "Gw2(0.3352*wmax)=" << Gw2(0.3352*wmax) << std::endl;
 if ( std::abs(Gw2(0.3352*wmax)-0.3352*wmax) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Gw2\n";  
 
 std::cout << "Git(0.3352*beta)(0,0)=" << Git(0.3352*beta)(0,0) << std::endl;
 if ( std::abs(Git(0.3352*beta)(0,0)-0.3352*beta) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Git\n";  
// if ( std::abs(Git2(0.3352*beta)-0.3352*beta) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Git2\n";   
 
 if ( std::abs(Gw2.on_mesh(N/3)-Gw2[N/3]) > precision) TRIQS_RUNTIME_ERROR<< "error in on_mesh()\n";  
 if ( std::abs(Gt2.on_mesh(N/3)-Gt2[N/3]) > precision) TRIQS_RUNTIME_ERROR<< "error in on_mesh()\n";  
 if ( std::abs(Git.on_mesh(N/3)(0,0)-Git[N/3](0,0)) > precision) TRIQS_RUNTIME_ERROR<< "error in on_mesh()\n";  
 
 // test hdf5 
 H5::H5File file("ess_gfre.h5", H5F_ACC_TRUNC );
 h5_write(file, "gt",  Gt);
 h5_write(file, "gw",  Gw);
 h5_write(file, "git",  Git);
 h5_write(file, "giw",  Giw);
 
 

}
