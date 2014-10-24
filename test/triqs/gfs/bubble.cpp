#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp>
#include <triqs/gfs/bz.hpp>
#include <triqs/gfs/cyclic_lattice.hpp>
#include <triqs/gfs/local/fourier_lattice.hpp>

namespace h5 = triqs::h5;
using namespace triqs::gfs;
using namespace triqs::clef;
using namespace triqs::arrays;
using namespace triqs::lattice;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> " << (X) << std::endl << std::endl;

int main() {
 try {
  int N = 16;
  int S = 1;
  placeholder<0> k_;
  placeholder<1> q_;
  placeholder<2> r_;
  placeholder<3> iw_;
  placeholder<4> inu_;
  placeholder<5> X_;

  double beta = 20, mu=0.;

  auto bz = brillouin_zone{bravais_lattice{{ {1, 0}, {0, 1} }}};

  /*
  auto gk = gf<brillouin_zone>{{bz, N}, {S, S}};
  auto sum_ = sum(gk(k_)(0,0), k_=gk.mesh()); 
  gk(k_) << -2 * (cos(k_(0)) + cos(k_(1)));
  auto gr = gf<cyclic_lattice>{{N, N}, {S, S}};
  gr() = inverse_fourier(gk); // ADD Security if gf =
  for (auto r : gr.mesh())
   if (max_element(abs(gr[r])) > 1.e-10) std::cout << r << gr[r] << std::endl;
  */
  // Default, with a tail.



  auto chi0q = gf<cartesian_product<imfreq, imfreq, brillouin_zone>>{{{beta, Fermion, 100}, {beta, Boson, 100}, {bz, N}}, {1, 1}};
  auto chi0r = gf<cartesian_product<imfreq, imfreq, cyclic_lattice>>{{{beta, Fermion, 100}, {beta, Boson, 100}, {N,  N}}, {1, 1}};

  auto Gk = gf<cartesian_product<imfreq, brillouin_zone>,matrix_valued,no_tail>{{{beta, Fermion, 100}, {bz, N}}, {1, 1}};
  auto Gr = gf<cartesian_product<imfreq, cyclic_lattice>,matrix_valued,no_tail>{{{beta, Fermion, 100}, {N,  N}}, {1, 1}};
//  auto G = gf<cartesian_product<brillouin_zone, imfreq>,no_tail>{{ {bz, N},{beta, Fermion, 100}}, {1, 1}};
  auto eps_k_ = -2 * (cos(k_(0)) + cos(k_(1)));
  Gk(inu_, k_) << 1 / (inu_ + mu - eps_k_);

  auto Gmesh = std::get<1>(Gk.mesh().components());
  chi0q(inu_, iw_, q_) << sum( Gk(inu_,k_)*Gk(inu_+iw_,k_+q_), k_=Gmesh )/Gmesh.size(); 

  curry<0>(Gr)[inu_] << inverse_fourier(curry<0>(Gk)[inu_]);

  // some mesh problem here?
  //chi0r(inu_, iw_, r_) << Gr(inu_, r_) * Gr(inu_+iw_, -r_);

  // Currying w.r.t. two variables works? How to specify the placeholder? X_?
  // curry<0,1>(chi0r)(inu_, iw_) instead?
  //curry<0,1>(chi0r)(X_) << inverse_fourier(curry<0,1>(chi0k)(X_));


  

  // hdf5
  h5::file file("ess_g_r_k.h5", H5F_ACC_TRUNC);
  //h5_write(file, "gr", gr);
  //h5_write(file, "gk", gk);
 }
 TRIQS_CATCH_AND_ABORT;
}
