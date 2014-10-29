#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp>
#include <triqs/gfs/bz.hpp>
#include <triqs/gfs/m_tail.hpp>

namespace h5 = triqs::h5;
using namespace triqs::gfs;
using namespace triqs::clef;
using namespace triqs::arrays;
using namespace triqs::lattice;

template<typename T1, typename T2>
void assert_equal(T1 const& x, T2 const& y, std::string mess) {
 if (std::abs(x - y) > 1.e-13) TRIQS_RUNTIME_ERROR << mess;
}

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> " << (X) << std::endl << std::endl;

int main() {
 try {
  double beta = 1;
  auto bz = brillouin_zone{bravais_lattice{{{1, 0}, {0, 1}}}};

  auto g_eps = gf<brillouin_zone>{{bz, 100}, {1, 1}};

  // Default, with a tail.
  auto G = gf<cartesian_product<brillouin_zone, imfreq>, matrix_valued, m_tail<brillouin_zone>>{{{bz, 100}, {beta, Fermion, 100}}, {1, 1}};

  // Same : the gf_default_singularity is the same
  static_assert(std::is_same<std14::decay_t<decltype(G.singularity())>, gf<brillouin_zone, tail>>::value, "!!");
  
  // auto G = gf<cartesian_product<brillouin_zone, imfreq>, matrix_valued, gf<brillouin_zone, tail>>{{{bz, 100}, {beta, Fermion, 100}}, {1, 1}};

  // The variant without tail
  auto G_no_tail = gf<cartesian_product<brillouin_zone, imfreq>, matrix_valued, no_tail>{{{bz, 100}, {beta, Fermion, 100}}, {1, 1}};

  // try to assign to expression
  placeholder<0> k_;
  placeholder<1> w_;

  auto eps_k = -2 * (cos(k_(0)) + cos(k_(1)));
  G(k_, w_) << 1 / (w_ - eps_k - 1 / (w_ + 2));

  //G_no_tail(k_, w_) << 1 / (w_ - eps_k - 1 / (w_ + 2));
  //return 0;

  if (0) { // to check speed of computation, which is  long due to tail (inversion ?)
   auto t = tail_omega(1, 1);
   auto t2 = t;
   for (int u = 0; u < 10000; ++u) {
    t2 = (t + u - inverse(t - u));
    //t2 = inverse(t + u - inverse(t - u));
   }
  }

  for (auto k : std::get<0>(G.mesh().components())) {
   assert_equal(G.singularity()[k](1)(0, 0), 1, "should be 1/omega");
   assert_equal(G.singularity()[k](2)(0, 0), eval(eps_k, k_ = k), "should be eps_k/omega^2");
  }

  auto gk = curry<0>(G);

  std::cerr << G.singularity()[{0, 0, 0}] << std::endl;
  std::cerr << gk[{0, 0, 0}].singularity() << std::endl;

  if (1) {

   for (auto k : gk.mesh()) {
    // std::cout  << k << std::endl;
    auto p = gk[k];
    assert_equal(gk[k].singularity()(1)(0, 0), 1, "should be 1/omega");
    assert_equal(gk[k].singularity()(2)(0, 0), eval(eps_k, k_ = k), "should be eps_k/omega^2");
   }

   for (auto k : gk.mesh()) {
    auto p = partial_eval<0>(G, 0);
    assert_equal(gk[k].singularity()(1)(0, 0), 1, "should be 1/omega");
    assert_equal(gk[k].singularity()(2)(0, 0), eval(eps_k, k_ = k), "should be eps_k/omega^2");
   }
  }
  // hdf5
  h5::file file("ess_g_k_om.h5", H5F_ACC_TRUNC);
  h5_write(file, "g", G);
 }
 TRIQS_CATCH_AND_ABORT;
}
