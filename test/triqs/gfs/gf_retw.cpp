#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp> 
using namespace triqs::gfs;
using namespace triqs::arrays;
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;
double precision=10e-12;

int main() {

 double beta =1;
 double tmax=10;
 double wmax=10;
 double it0=0.3352*beta;
 double t0=0.3352*tmax;
 double w0=0.3352*wmax;
 int N=1000;
 
 auto Gw = gf<refreq> {{-wmax, wmax, N},{2,2}};
 auto Gt = gf<retime> {{0, tmax, N}, {2,2}};
 auto Gw2 = gf<refreq,scalar_valued> {{-wmax, wmax, N}};
 auto Gt2 = gf<retime,scalar_valued> {{0, tmax, N}};
 auto Giw = gf<imfreq> {{beta, Fermion,N}, {2,2}};
 auto Git = gf<imtime> {{beta, Fermion,N},  {2,2}};
 auto Giw2 = gf<imfreq,scalar_valued> {{beta, Fermion, N}};
 auto Git2 = gf<imtime,scalar_valued> {{beta, Fermion, N}};

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
 
 std::cout << "Gt(t0)(0,0)=" << Gt(t0)(0,0) << std::endl;
 if ( std::abs(Gt(t0)(0,0)-t0) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Gt\n";  
 std::cout << "Gt2(t0)=" << Gt2(t0) << std::endl;
 if ( std::abs(Gt2(t0)-t0) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Gt2\n";  
 std::cout << "Gw(w0)(0,0)=" << Gw(w0)(0,0) << std::endl;
 if ( std::abs(Gw(w0)(0,0)-w0) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Gw\n";  
 std::cout << "Gw2(w0)=" << Gw2(w0) << std::endl;
 if ( std::abs(Gw2(w0)-w0) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Gw2\n";  
 
 std::cout << "Git(it0)(0,0)=" << Git(it0)(0,0) << std::endl;
 if ( std::abs(Git(it0)(0,0)-it0) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Git\n"; 
 std::cout << "Git2(it0)=" << Git2(it0) << std::endl; 
 if ( std::abs(Git2(it0)-it0) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Git2\n";   
 
 //verification of windowing for imaginary times outside from [0,beta]
 std::cout << "Git2(5*beta+it0)=" << Git2(it0+5*beta) << std::endl; 
 if ( std::abs(Git2(1.3352*beta)+it0) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Git3\n";   
 std::cout << "Git2(it0-beta)=" << Git2(it0-beta) << std::endl; 
 if ( std::abs(Git2(it0-beta)+it0) > precision) TRIQS_RUNTIME_ERROR<< "evaluator error in Git3\n";  
 
 if ( std::abs(Gw2.on_mesh(N/3) -Gw2[N/3] ) > precision) TRIQS_RUNTIME_ERROR<< "error in on_mesh()\n";  
 if ( std::abs(Gt2.on_mesh(N/3) -Gt2[N/3] ) > precision) TRIQS_RUNTIME_ERROR<< "error in on_mesh()\n";  
 if ( std::abs(Git2.on_mesh(N/3)-Git2[N/3]) > precision) TRIQS_RUNTIME_ERROR<< "error in on_mesh()\n";  
 
 // test hdf5 
 H5::H5File file("ess_gfre.h5", H5F_ACC_TRUNC );
 h5_write(file, "gt",  Gt);
 h5_write(file, "gw",  Gw);
 h5_write(file, "git",  Git);
 h5_write(file, "giw",  Giw);
 
 

}
