#include <triqs/gfs.hpp>
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> " << (X) << std::endl << std::endl;

namespace h5 = triqs::h5;
using namespace triqs::gfs;
using namespace triqs::arrays;

int main(int argc, char* argv[]) {

 try {

  triqs::clef::placeholder<0> w_;
  auto agf = array<gf<imfreq>, 2>{2, 3};
  auto bgf = array<gf<imfreq>, 2>{2, 3};
  agf() = gf<imfreq>{{10.0, Fermion}, {1, 1}};
  agf(0, 0)(w_) << 1 / (w_ + 2);

  array<double,2> A(2,2); A()=0;A(0,0) = 1.3; A(1,1) = -8.2;
  array<array<double,2>, 1> aa(2), bb;
  aa(0) = A;
  aa(1) = 2 * A;
  bb = aa;
  
  { 
   h5::file file("ess_array_gf.h5", H5F_ACC_TRUNC);
   h5_write(file, "Agf", agf);
   h5_write(file, "aa", aa);
  }
  {
   h5::file file("ess_array_gf.h5", H5F_ACC_RDONLY);
   h5_read(file, "Agf", bgf);
   h5_read(file, "aa", bb);
  }
  {
   h5::file file("ess_array_gf2.h5", H5F_ACC_TRUNC);
   h5_write(file, "Agf", bgf);
   h5_write(file, "aa", bb);
  }
 }
 TRIQS_CATCH_AND_ABORT;
}
