
#include <triqs/clef.hpp>
#include <triqs/gfs.hpp>
#include <triqs/utility/timer.hpp>

using namespace triqs;   
using namespace triqs::gfs;

// Index placeholders
clef::placeholder<0> i;
clef::placeholder<1> j;
clef::placeholder<2> k;
clef::placeholder<3> l;

// Frequency placeholders
clef::placeholder<4> w;
clef::placeholder<5> n1;
clef::placeholder<6> n2;
clef::placeholder<7> n3;

typedef gf<cartesian_product<imfreq, imfreq, imfreq>, tensor_valued<4>> g2_iw_t;

double test_clef_clef(int n=20, int m=20, double beta=1.0) {
  utility::timer timer;
  auto G2_iw_ph = g2_iw_t{
    {{beta, Boson, n}, {beta, Fermion, m}, {beta, Fermion, m}}, {1, 1, 1, 1}};
  timer.start();
  
  G2_iw_ph(w,n1,n2)(i,j,k,l) << 0.0;

  timer.stop();  
  return double(timer);
}

double test_loop_clef(int n=20, int m=20, double beta=1.0) {
  utility::timer timer;
  gf_mesh<imfreq> mesh_b{beta, Boson, n};
  gf_mesh<imfreq> mesh_f{beta, Fermion, m};
  auto G2_iw_ph = g2_iw_t{{mesh_b, mesh_f, mesh_f}, {1, 1, 1, 1}};
  timer.start();
  
  for(auto const& w : mesh_b)
  for(auto const& n1 : mesh_f)
  for(auto const& n2 : mesh_f)
    G2_iw_ph[w,n1,n2](i,j,k,l) << 0.0;

  timer.stop();  
  return double(timer);
}

double test_loop_loop(int n=20, int m=20, double beta=1.0) {
  utility::timer timer;
  gf_mesh<imfreq> mesh_b{beta, Boson, n};
  gf_mesh<imfreq> mesh_f{beta, Fermion, m};
  auto G2_iw_ph = g2_iw_t{{mesh_b, mesh_f, mesh_f}, {1, 1, 1, 1}};
  timer.start();
  
  int size_0 = G2_iw_ph.target_shape()[0];
  int size_1 = G2_iw_ph.target_shape()[1];
  int size_2 = G2_iw_ph.target_shape()[2];
  int size_3 = G2_iw_ph.target_shape()[3];

  for(auto const& w : mesh_b)
  for(auto const& n1 : mesh_f)
  for(auto const& n2 : mesh_f)
  for(int i : range(size_0))
  for(int j : range(size_1))
  for(int k : range(size_2))
  for(int l : range(size_3))
    G2_iw_ph[w,n1,n2](i,j,k,l) = 0.0;

  timer.stop();  
  return double(timer);
}

double test_loop_loop_bracket(int n=20, int m=20, double beta=1.0) {
  utility::timer timer;
  gf_mesh<imfreq> mesh_b{beta, Boson, n};
  gf_mesh<imfreq> mesh_f{beta, Fermion, m};
  auto G2_iw_ph = g2_iw_t{{mesh_b, mesh_f, mesh_f}, {1, 1, 1, 1}};
  timer.start();
  
  int size_0 = G2_iw_ph.target_shape()[0];
  int size_1 = G2_iw_ph.target_shape()[1];
  int size_2 = G2_iw_ph.target_shape()[2];
  int size_3 = G2_iw_ph.target_shape()[3];

  for(auto const& w : mesh_b)
  for(auto const& n1 : mesh_f)
  for(auto const& n2 : mesh_f)
  for(int i : range(size_0))
  for(int j : range(size_1))
  for(int k : range(size_2))
  for(int l : range(size_3))
    G2_iw_ph[{w,n1,n2}](i,j,k,l) = 0.0;

  timer.stop();  
  return double(timer);
}

double test_loop_loop_constexpr_target(int n=20, int m=20, double beta=1.0) {
  utility::timer timer;
  gf_mesh<imfreq> mesh_b{beta, Boson, n};
  gf_mesh<imfreq> mesh_f{beta, Fermion, m};
  auto G2_iw_ph = g2_iw_t{{mesh_b, mesh_f, mesh_f}, {1, 1, 1, 1}};
  timer.start();
  
  auto size_0 = 1;
  auto size_1 = 1;
  auto size_2 = 1;  
  auto size_3 = 1;

  for(auto const& w : mesh_b)
  for(auto const& n1 : mesh_f)
  for(auto const& n2 : mesh_f)
  for(int i : range(size_0))
  for(int j : range(size_1))
  for(int k : range(size_2))
  for(int l : range(size_3))
    G2_iw_ph[{w,n1,n2}](i,j,k,l) = 0.0;

  timer.stop();  
  return double(timer);
}

double test_loop_fixed(int n=20, int m=20, double beta=1.0) {
  utility::timer timer;
  gf_mesh<imfreq> mesh_b{beta, Boson, n};
  gf_mesh<imfreq> mesh_f{beta, Fermion, m};
  auto G2_iw_ph = g2_iw_t{{mesh_b, mesh_f, mesh_f}, {1, 1, 1, 1}};
  timer.start();
  
  for(auto const& w : mesh_b)
  for(auto const& n1 : mesh_f)
  for(auto const& n2 : mesh_f)
    G2_iw_ph[w,n1,n2](0,0,0,0) = 0.0;

  timer.stop();  
  return double(timer);
}

double test_array(int n=20, int m=20, double beta=1.0) {
  utility::timer timer;
  gf_mesh<imfreq> mesh_b{beta, Boson, n};
  gf_mesh<imfreq> mesh_f{beta, Fermion, m};
  auto G2_iw_ph = g2_iw_t{{mesh_b, mesh_f, mesh_f}, {1, 1, 1, 1}};
  timer.start();
  
  G2_iw_ph.data() *= 0.0;

  timer.stop();  
  return double(timer);
}

/*

//#include <Eigen/Core>
//#include <Eigen/Dense>
//#include <unsupported/Eigen/CXX11/Tensor>

double test_eigen_tensor(int n=20, int m=20, double beta=1.0) {
  utility::timer timer;
  gf_mesh<imfreq> mesh_b{beta, Boson, n};
  gf_mesh<imfreq> mesh_f{beta, Fermion, m};
  auto G2_iw_ph = g2_iw_t{{mesh_b, mesh_f, mesh_f}, {1, 1, 1, 1}};

  // create eigen tensor view of the data ?!?!
  template<int I> using Tensor = Eigen::Tensor<std::complex<double>, I, Eigen::RowMajor>;
  Eigen::TensorMap<Tensor<7>> data(G2_iw_ph.data(), n, m, m, 1, 1, 1, 1);

  timer.start();

  data = 0.0;

  timer.stop();  
  return double(timer);
}

*/
