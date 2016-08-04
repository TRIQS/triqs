#include <triqs/test_tools/gfs.hpp>
using namespace triqs::clef;
using namespace triqs::lattice;
using namespace triqs::gfs;

gf<cartesian_product<brillouin_zone, imfreq>,matrix_valued> compute_gg_fft(gf<cartesian_product<brillouin_zone, imfreq>, matrix_valued> const & G_k_w){
  placeholder<0> r_;
  placeholder_prime <1> tau_;
  placeholder<2> k_;

  auto w_mesh = std::get<1>(G_k_w.mesh().components());
  auto k_mesh = std::get<0>(G_k_w.mesh().components());
  double beta = w_mesh.domain().beta;
  int nw = w_mesh.last_index()+1;
  int ntau = nw*4;
  int nk = k_mesh.get_dimensions()[0];
 
  auto chi0_q_w = gf<cartesian_product<brillouin_zone, imfreq>, matrix_valued>{{  k_mesh, {beta, Boson, nw}}, {1, 1}};

  auto chi0_q_tau = gf<cartesian_product<brillouin_zone, imtime>, matrix_valued>{{  k_mesh, {beta, Boson, ntau}}, {1, 1}};
  auto chi0_R_tau = gf<cartesian_product<cyclic_lattice, imtime>, matrix_valued>{{ {nk,  nk}, {beta, Boson, ntau}}, {1, 1}};
  auto G_k_tau = gf<cartesian_product<brillouin_zone, imtime>,matrix_valued>{{ k_mesh,{beta, Fermion, ntau}}, {1, 1}};
  auto G_R_tau = gf<cartesian_product<cyclic_lattice, imtime>,matrix_valued>{{ {nk,  nk},{beta, Fermion, ntau} }, {1, 1}};

  curry<0>(G_k_tau)[k_] << inverse_fourier(curry<0>(G_k_w)[k_]);
  curry<1>(G_R_tau)[tau_] << inverse_fourier(curry<1>(G_k_tau)[tau_]);
  
  //chi0_R_tau(r_, tau_) << G_R_tau(r_, tau_) * G_R_tau(-r_, -tau_); //-tau does not work: if tau>0, returns 0
  for(auto const & r : std::get<0>(G_R_tau.mesh().components()))
   for(auto const & tau : std::get<1>(G_R_tau.mesh().components()))
    chi0_R_tau[{r,tau}] = G_R_tau(r,tau)*G_R_tau(-r,-tau); //-tau does not work: if tau>0, returns 0
  /*
  h5::file file("bubble.h5", H5F_ACC_TRUNC );
  h5_write(file, "G_R_tau", G_R_tau);
  h5_write(file, "chi0_R_tau", chi0_R_tau);
  */

  curry<1>(chi0_q_tau)(tau_) << fourier(on_mesh(curry<1>(chi0_R_tau))(tau_));
  curry<0>(chi0_q_w)(k_) << fourier(on_mesh(curry<0>(chi0_q_tau))(k_));

  return chi0_q_w;
}

gf<cartesian_product<brillouin_zone, imfreq>,matrix_valued> compute_gg(gf<cartesian_product<brillouin_zone, imfreq>, matrix_valued> const & G_k_w){

  placeholder_prime<0> k_;
  placeholder_prime<1> q_;
  placeholder_prime<3> iw_;
  placeholder_prime<4> inu_;
  auto w_mesh = std::get<1>(G_k_w.mesh().components());
  auto k_mesh = std::get<0>(G_k_w.mesh().components());
  double beta = w_mesh.domain().beta;
  int nw = w_mesh.last_index()+1;
 
  auto chi0_q_w = gf<cartesian_product<brillouin_zone, imfreq>, matrix_valued>{{  k_mesh, {beta, Boson, nw}}, {1, 1}};

  chi0_q_w( q_, inu_) << sum(G_k_w(k_,inu_ ) * G_k_w( k_ + q_,inu_ + iw_), k_ = k_mesh, iw_=w_mesh ) / k_mesh.size() / beta;

  return chi0_q_w;
}

TEST(Gf, Bubble) {
 int nw = 10;
 int nk = 6;
  auto bz = brillouin_zone{bravais_lattice{{ {1., 0.}, {0.5, sqrt(3)/2.} }}};
  double beta = 20, mu=0.;
  placeholder<0> k_;
  placeholder<4> inu_;
  auto eps_k_ = -2 * (cos(k_(0)) + cos(k_(1)));
  auto G_k_w = gf<cartesian_product<brillouin_zone, imfreq>,matrix_valued>{{ {bz, nk},{beta, Fermion, nw}}, {1, 1}};
  G_k_w(k_, inu_) << 1 / (inu_ + mu - eps_k_);

 auto chi_q_w_fft = compute_gg_fft(G_k_w);
 auto chi_q_w = compute_gg(G_k_w);

/* 
 h5::file file("bubble.h5", H5F_ACC_RDWR );
 h5_write(file, "chi_fft", chi_q_w_fft);
 h5_write(file, "chi", chi_q_w);
*/
 EXPECT_ARRAY_NEAR(chi_q_w_fft.data(), chi_q_w.data());
}
MAKE_MAIN;


