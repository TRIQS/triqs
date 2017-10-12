#include "./fourier_two_variables.hpp"

namespace triqs {
 namespace gfs {

  gf<cartesian_product<imfreq, imfreq>, tensor_valued<3>>
  fourier(gf_const_view<cartesian_product<imtime, imtime>, tensor_valued<3>> g2t, int n_w_1, int n_w_2,
          bool positive_matsub_only_1, bool positive_matsub_only_2) {

   double beta        = std::get<0>(g2t.mesh()).domain().beta;
   auto imfreq_mesh_1 = gf_mesh<imfreq>{beta, std::get<0>(g2t.mesh()).domain().statistic, n_w_1,
                                        positive_matsub_only_1 ? matsubara_mesh_opt::positive_frequencies_only :
                                                                 matsubara_mesh_opt::all_frequencies};
   auto imfreq_mesh_2 = gf_mesh<imfreq>{beta, std::get<1>(g2t.mesh()).domain().statistic, n_w_2,
                                        positive_matsub_only_2 ? matsubara_mesh_opt::positive_frequencies_only :
                                                                 matsubara_mesh_opt::all_frequencies};

   gf<cartesian_product<imfreq, imfreq>, tensor_valued<3>> g2w({imfreq_mesh_1, imfreq_mesh_2}, g2t.target_shape());
   gf<cartesian_product<imfreq, imtime>, tensor_valued<3>> gwt({imfreq_mesh_1, std::get<1>(g2t.mesh())}, g2t.target_shape());

   array<__tail<scalar_valued>, 3> tail_3(g2t.target_shape());
   __tail<scalar_valued> t(1, 1);
   t.data()() = 0;
   tail_3()   = t;

   auto _ = var_t{};

   for (auto const& tau : second_mesh(g2t)) {
   //for (auto const& tau : std::get<1>(g2t.mesh())) {
    gwt[_, tau] = fourier(g2t[_, tau], tail_3, n_w_1, positive_matsub_only_1);
    // auto g_w_tau = fourier(g2t[_, tau], tail_3, n_w_1, positive_matsub_only_1);
    // for (auto const& om : std::get<0>(gwt.mesh()))
    // gwt[om, tau] = g_w_tau[om];
   }

   for (auto const& om : first_mesh(gwt)) { 
   //for (auto const& om : std::get<0>(gwt.mesh())) {
    g2w[om, _] = fourier(gwt[om, _], tail_3, n_w_2, positive_matsub_only_2);
    // auto g_w_wp = fourier(gwt[om, _], tail_3, n_w_2, positive_matsub_only_2);
    // for (auto const& nu : std::get<1>(g2w.mesh()))
    // g2w[om, nu] = g_w_wp[nu];
   }

   return g2w;
  }

  // ---------------------------------------------------------------------------------------------------------

  gf<cartesian_product<imtime, imtime>, tensor_valued<3>>
  inverse_fourier(gf_const_view<cartesian_product<imfreq, imfreq>, tensor_valued<3>> g2w, int n_t_1, int n_t_2, bool fit_tails) {

   double beta        = std::get<0>(g2w.mesh()).domain().beta;
   auto imtime_mesh_1 = gf_mesh<imtime>{beta, std::get<0>(g2w.mesh()).domain().statistic, n_t_1};
   auto imtime_mesh_2 = gf_mesh<imtime>{beta, std::get<1>(g2w.mesh()).domain().statistic, n_t_2};

   gf<cartesian_product<imtime, imtime>, tensor_valued<3>> g2t({imtime_mesh_1, imtime_mesh_2}, g2w.target_shape());
   gf<cartesian_product<imtime, imfreq>, tensor_valued<3>> gtw({imtime_mesh_1, std::get<1>(g2w.mesh())}, g2w.target_shape());

   array<__tail<scalar_valued>, 3> known_moments(g2w.target_shape());
   __tail<scalar_valued> t(2, -1);
   t(-1)           = 0.;
   t(0)            = 0.;
   known_moments() = t;

   array<__tail<scalar_valued>, 3> tail_zero(g2w.target_shape());
   __tail<scalar_valued> t0;
   t0.data()() = 0;
   tail_zero() = t0;

   auto _ = var_t{};

   for (auto const& wp : std::get<1>(g2w.mesh())) {
    auto g      = g2w[_, wp];
    int n_max   = g.mesh().size();
    auto tail_3 = fit_tails ? fit_tail(g, known_moments, 3, int(0.75 * n_max), n_max) : tail_zero;
    gtw[_, wp]  = inverse_fourier(g, tail_3, imtime_mesh_1.size());
    // auto g_t_wp_nu = inverse_fourier(g, tail_3, imtime_mesh_1.size());
    // for (auto const& tau : std::get<0>(gtw.mesh()))
    // gtw[tau, wp] = g_t_wp_nu[tau];
   }

   for (auto const& tau : std::get<0>(gtw.mesh())) {
    auto g      = gtw[tau, _];
    int n_max   = g.mesh().size();
    auto tail_3 = fit_tails ? fit_tail(make_const_view(g), known_moments, 3, int(0.75 * n_max), n_max) : tail_zero;
    g2t[tau, _] = inverse_fourier(g, tail_3, imtime_mesh_2.size());
    // auto g_t_tp_tau = inverse_fourier(g, tail_3, imtime_mesh_2.size());
    // for (auto const& tp : std::get<1>(g2t.mesh()))
    // g2t[tau, tp] = g_t_tp_tau[tp];
   }

   return g2t;
  }
 }
}
