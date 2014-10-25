#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp>


// FAIRE make_value !!
//
using namespace triqs::gfs;
using namespace triqs::clef;
using namespace triqs::arrays;
using namespace triqs::lattice;
template <typename Function, typename Mesh>
// requires ( is_function_on_mesh<Function,Mesh>())
auto  sum_gf(Function const &f, Mesh const &m) ->decltype(make_matrix(0*f(*(m.begin())))) {
 //auto sum_gf(Function const &f, Mesh const &m) {
 //auto res = typename triqs::regular_type_if_exists_else_type<decltype(f(typename Mesh::mesh_point_t{}))>::type (f(m.begin()));
 auto res = make_matrix(0*f(*(m.begin())));
 for (auto const &x : m) res = res + f(x);
 return res;
}
namespace triqs {
 namespace clef {
  TRIQS_CLEF_MAKE_FNT_LAZY(sum_gf);
  TRIQS_CLEF_MAKE_FNT_LAZY(conj);
 }
}

#define TEST(...) std::cout << BOOST_PP_STRINGIZE((__VA_ARGS__)) << " ---> " << (__VA_ARGS__) << std::endl << std::endl;

int main() {
 try {
  double beta = 1;

  auto bz = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};

  auto g_eps = gf<brillouin_zone>{{bz, 20}, {1, 1}};

  auto G_k_iom = gf<cartesian_product<brillouin_zone, imfreq>>{{{bz, 20}, {beta, Fermion, 100}}, {1, 1}};

  // try to assign to expression 
  placeholder<0> k_;
  placeholder<1> w_;

  auto eps_k = -2 * (cos(k_(0)) + cos(k_(1)));
  G_k_iom(k_, w_) << 1 / (w_ - eps_k);

  auto G_loc = gf<imfreq, matrix_valued, no_tail>{{beta, Fermion, 100}, {1, 1}};

  auto r = G_k_iom(k_t{0, 0, 0}, matsubara_freq{0, beta, Fermion});

  auto r5 = sum_gf(k_ >> G_k_iom(k_,0), g_eps.mesh());
  G_loc(w_) << sum_gf(k_ >> G_k_iom(k_,w_), g_eps.mesh());

  TEST(G_loc(0));

  auto G_k_tau = gf<cartesian_product<brillouin_zone, imtime>>{{{bz, 20}, {beta, Fermion, 100}}, {1, 1}};
  
  //auto r3 = partial_eval<0>(G_k_iom,0);
  //auto r4 = partial_eval<0>(G_k_tau,0);
  //auto gt = curry<0>(G_k_tau) [0];
  //auto  gw = curry<0>(G_k_iom)[0];
 
  //curry<0>(G_k_tau) [k_] << inverse_fourier(curry<0>(G_k_iom)[k_]); 
  
  //TEST(G_k_tau[{0,0}]);

  // hdf5 
  //h5::file file("ess_g_k_om.h5", H5F_ACC_TRUNC );
  //h5_write(file, "g", G);

 
 }
 TRIQS_CATCH_AND_ABORT;
}
