#include <triqs/test_tools/gfs.hpp>
using namespace triqs::clef;
using namespace triqs::lattice;
using namespace triqs::gfs;

gf<cartesian_product<brillouin_zone, imfreq>,matrix_valued> compute_bubble(int nw, int nk){
  placeholder<0> k_;
  placeholder<1> q_;
  placeholder<2> r_;
  placeholder<3> iw_;
  placeholder<4> inu_;

  double beta = 20, mu=0.;
  int ntau = nw*4;

  auto bz = brillouin_zone{bravais_lattice{{ {1., 0.}, {0.5, sqrt(3)/2.} }}};

  auto chi0_q_w = gf<cartesian_product<brillouin_zone, imfreq>, matrix_valued>{{  {bz, nk}, {beta, Boson, nw}}, {1, 1}};
  auto chi0_q_tau = gf<cartesian_product<brillouin_zone, imtime>, matrix_valued>{{  {bz, nk}, {beta, Boson, ntau}}, {1, 1}};
  auto chi0_R_tau = gf<cartesian_product<cyclic_lattice, imtime>, matrix_valued>{{ {nk,  nk}, {beta, Boson, ntau}}, {1, 1}};

  auto G_k_w = gf<cartesian_product<brillouin_zone, imfreq>,matrix_valued>{{ {bz, nk},{beta, Fermion, nw}}, {1, 1}};
  auto G_k_tau = gf<cartesian_product<brillouin_zone, imtime>,matrix_valued>{{ {bz, nk},{beta, Fermion, ntau}}, {1, 1}};
  auto G_R_tau = gf<cartesian_product<cyclic_lattice, imtime>,matrix_valued>{{ {nk,  nk},{beta, Fermion, ntau} }, {1, 1}};

  auto eps_k_ = -2 * (cos(k_(0)) + cos(k_(1)));
  G_k_w(k_, inu_) << 1 / (inu_ + mu - eps_k_);

  auto kmesh = std::get<0>(G_k_w.mesh());
  auto wmesh = std::get<1>(G_k_w.mesh());

  curry<0>(G_k_tau)[k_] << inverse_fourier(curry<0>(G_k_w)[k_]);
  
  placeholder_prime <0> tau_;
  curry<1>(G_R_tau)[tau_] << inverse_fourier(curry<1>(G_k_tau)[tau_]);

  //chi0_q_w( q_, inu_) << sum(G_k_w(k_,inu_ ) * G_k_w( k_ + q_,inu_ + iw_), k_ = kmesh, iw_=wmesh ) / kmesh.size() / beta;
  //chi0_R_w(r_, inu_) << sum(G_w_R( r_, inu_) * G_w_R(-r_, inu_ + iw_), iw_=wmesh)/beta;
  for(auto const & r : std::get<0>(chi0_R_tau.mesh().components()))
  for(auto const & tau : std::get<1>(chi0_R_tau.mesh().components()))
    chi0_R_tau[{r, tau}] = G_R_tau( r, tau) * G_R_tau(-r, -tau);

  chi0_R_tau(r_, tau_) << G_R_tau( r_, tau_) * G_R_tau(-r_, -tau_);

  curry<1>(chi0_q_tau)(tau_) << fourier(on_mesh(curry<1>(chi0_R_tau))(tau_));
  curry<0>(chi0_q_w)(k_) << fourier(on_mesh(curry<0>(chi0_q_tau))(k_));

  return chi0_q_w;
}

TEST(Gf, Bubble) {

auto r = compute_bubble(10,10);
}
MAKE_MAIN;


