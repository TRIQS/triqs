#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp>
#include <triqs/gfs/bz.hpp>

using namespace triqs::gfs;
using namespace triqs::clef;
using namespace triqs::arrays;
using namespace triqs::lattice;
template <typename Function, typename Mesh>
// requires ( is_function_on_mesh<Function,Mesh>())
auto  sum(Function const &f, Mesh const &m)->decltype(f(typename Mesh::mesh_point_t{})) {
 //auto sum(Function const &f, Mesh const &m) {
 auto res = decltype(f(typename Mesh::mesh_point_t{})) {};
 for (auto const &x : m)  res = res + f(x);
 return res;
}
namespace triqs {
 namespace clef {
  TRIQS_CLEF_MAKE_FNT_LAZY(sum);
  TRIQS_CLEF_MAKE_FNT_LAZY(conj);
 }
}

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> " << (X) << std::endl << std::endl;

int main() {
 try {
  double beta = 1;

  auto bz_ = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};

  auto g_eps = gf<bz>{{bz_, 20}, {1, 1}};

  auto G_k_iom = gf<cartesian_product<bz, imfreq>>{{{bz_, 20}, {beta, Fermion, 100}}, {1, 1}};

  // try to assign to expression 
  placeholder<0> k_;
  placeholder<1> w_;
  auto eps = make_expr( [](k_t const& k) { return -2 * (cos(k(0)) + cos(k(1))); }) ;

  G_k_iom(k_, w_) << 1 / (w_ - eps(k_));


  auto G_loc = gf<imfreq>{{beta,Fermion, 100}};
  //G_loc(w_) << sum(k_ >> G_k_iom(k_,w_), g_eps.mesh());//does not compile

  TEST(G_loc(0));

  auto G_k_tau = gf<cartesian_product<bz, imtime>>{{{bz_, 20}, {beta, Fermion, 100}}, {1, 1}};
  
 //curry<1>(G_k_tau) [k_] << inverse_fourier(curry<1>(G_k_iom)[k_]); //does not compile
  
  //TEST(G_k_tau(0,0));

  // hdf5 
  //H5::H5File file("ess_g_k_om.h5", H5F_ACC_TRUNC );
  //h5_write(file, "g", G);

 
 }
 TRIQS_CATCH_AND_ABORT;
}
