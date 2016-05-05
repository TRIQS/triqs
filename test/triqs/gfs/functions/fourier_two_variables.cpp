#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs/transform/fourier_two_variables.hpp>
using namespace triqs::gfs;

TEST(Fourier, TwoVariables){
 //auto mesh_opt = matsubara_mesh_opt::positive_frequencies_only ;
 auto mesh_opt = matsubara_mesh_opt::all_frequencies ;
 auto pos_only = (mesh_opt == matsubara_mesh_opt::positive_frequencies_only);


 double beta = 1;
 int n_iw=100;
 int n_tau=2*n_iw+1;
 auto f_imtime_mesh = gf_mesh<imtime>{beta, Fermion, n_tau};
 auto b_imtime_mesh = gf_mesh<imtime>{beta, Boson, n_tau};
 auto f_imfreq_mesh = gf_mesh<imfreq>{beta, Fermion, n_iw, mesh_opt};
 auto b_imfreq_mesh = gf_mesh<imfreq>{beta, Boson, n_iw, mesh_opt};

 gf<cartesian_product<imfreq, imfreq>, tensor_valued<3>> g2w({f_imfreq_mesh, b_imfreq_mesh}, {1,1,1});
 placeholder<0> om_;
 placeholder<1> nu_;
 g2w(om_,nu_) << 1/om_*1/(om_+nu_);


 auto g2t = inverse_fourier(g2w, n_tau, n_tau, false);
 auto g2w_2 = fourier(g2t, n_iw, n_iw, pos_only,pos_only);

 EXPECT_CLOSE_ARRAY(g2w.data(), g2w_2.data());
}

MAKE_MAIN;
