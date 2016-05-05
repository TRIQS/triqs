#include "./fourier_two_variables.hpp"

namespace triqs { namespace gfs {


 gf<cartesian_product<imfreq, imfreq>, tensor_valued<3>> fourier(gf_const_view<cartesian_product<imtime, imtime>, tensor_valued<3>> g2t,  int n_w_1,  int n_w_2, bool positive_matsub_only_1, bool positive_matsub_only_2){

  double beta =  std::get<0>(g2t.mesh().components()).domain().beta;
  auto imfreq_mesh_1 = gf_mesh<imfreq>{beta, std::get<0>(g2t.mesh().components()).domain().statistic, n_w_1, positive_matsub_only_1? matsubara_mesh_opt::positive_frequencies_only : matsubara_mesh_opt::all_frequencies};
  auto imfreq_mesh_2 = gf_mesh<imfreq>{beta, std::get<1>(g2t.mesh().components()).domain().statistic, n_w_2, positive_matsub_only_2? matsubara_mesh_opt::positive_frequencies_only : matsubara_mesh_opt::all_frequencies};

  gf<cartesian_product<imfreq, imfreq>, tensor_valued<3>> g2w({imfreq_mesh_1, imfreq_mesh_2}, g2t.target_shape());
  gf<cartesian_product<imfreq, imtime>, tensor_valued<3>> gwt({imfreq_mesh_1, std::get<1>(g2t.mesh().components())}, g2t.target_shape());

  // long (explicit) version 
  auto g_t_tp = triqs::make_view(curry<1>(g2t));
  array<__tail<scalar_valued>,3> tail_3(g2t.target_shape());
  __tail<scalar_valued> t(1,1);
  t.data()()=0;
  tail_3()=t;
  for(auto const & tau : g_t_tp.mesh()){
   auto g_w_tp_tau = fourier(g_t_tp[tau], tail_3, n_w_1, positive_matsub_only_1);
   for(auto const & om: std::get<0>(gwt.mesh().components()))
    gwt[{om, tau}] = g_w_tp_tau[om];
  }

  auto g_w_tp = triqs::make_view(curry<0>(gwt));
  for(auto const & om : g_w_tp.mesh()){
   auto g_w_wp_nu = fourier(g_w_tp[om], tail_3, n_w_2, positive_matsub_only_2);
   for(auto const & nu: std::get<1>(g2w.mesh().components()))
    g2w[{om, nu}] = g_w_wp_nu[nu];
  }
  /* short version : (does not compile)
  clef::placeholder<0> tau_;
  curry<1>(gwt)[tau_] << fourier(curry<1>(g2t)[tau_]);
  clef::placeholder<1> om_;
  curry<0>(g2w)[om_] << fourier(curry<0>(gwt)[om_]);
  */
  

  return g2w;
 }

 gf<cartesian_product<imtime, imtime>, tensor_valued<3>> inverse_fourier(gf_const_view<cartesian_product<imfreq, imfreq>, tensor_valued<3>> g2w,  int n_t_1,  int n_t_2, bool fit_tails){

  double beta =  std::get<0>(g2w.mesh().components()).domain().beta;
  auto imtime_mesh_1 = gf_mesh<imtime>{beta, std::get<0>(g2w.mesh().components()).domain().statistic, n_t_1};
  auto imtime_mesh_2 = gf_mesh<imtime>{beta, std::get<1>(g2w.mesh().components()).domain().statistic, n_t_2};

  gf<cartesian_product<imtime, imtime>, tensor_valued<3>> g2t({imtime_mesh_1, imtime_mesh_2}, g2w.target_shape());
  gf<cartesian_product<imtime, imfreq>, tensor_valued<3>> gtw({imtime_mesh_1, std::get<1>(g2w.mesh().components())}, g2w.target_shape());

  array<__tail<scalar_valued>,3> known_moments(g2w.target_shape());
  __tail<scalar_valued> t(2, -1); 
  t(-1)=0.; 
  t(0)=0.; 
  known_moments()=t;

  array<__tail<scalar_valued>,3> tail_zero(g2w.target_shape());
  __tail<scalar_valued> t0; 
  t0.data()() = 0;
  tail_zero()=t0;

  auto g_w_wp = triqs::make_view(curry<1>(g2w));

  for(auto const & nu : g_w_wp.mesh()){
   int n_max = g_w_wp[nu].mesh().size();
   auto tail_3 = fit_tails ? fit_tail(g_w_wp[nu], known_moments, 3, int(0.75*n_max), n_max) : tail_zero;
   //std::cout << tail_3(0,0,0) << endl;
   auto g_t_wp_nu = inverse_fourier(g_w_wp[nu], tail_3, imtime_mesh_1.size());
   for(auto const & tau: std::get<0>(gtw.mesh().components()))
    gtw[{tau, nu}] = g_t_wp_nu[tau];
  }

  auto g_t_wp = triqs::make_view(curry<0>(gtw));

  for(auto const & tau : g_t_wp.mesh()){
   int n_max = g_t_wp[tau].mesh().size();
   auto tail_3 = fit_tails ? fit_tail(make_const_view(g_t_wp[tau]), known_moments, 3, int(0.75*n_max), n_max) : tail_zero;
   auto g_t_tp_tau = inverse_fourier(g_t_wp[tau], tail_3, imtime_mesh_2.size());
   for(auto const & tp: std::get<1>(g2t.mesh().components()))
    g2t[{tau, tp}] = g_t_tp_tau[tp];
  }

  return g2t;

 }

}}
