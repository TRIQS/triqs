#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp>
using namespace triqs::gfs;
using namespace triqs;
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> " << (X) << std::endl << std::endl;

int main() {
 try {
  double beta = 1;
  auto G1 = gf<imfreq>({beta, Fermion}, {2, 2});
  auto G2 = G1;
  auto G3 = G2;

  // construct some block functions
  auto B0 = block_gf<imfreq>(3);

  auto B1 = make_block_gf<imfreq>(3, G1);
  auto B2 = make_block_gf<imfreq>({G1, G1, G1});
  auto B3 = make_block_gf<imfreq>({"a", "b", "c"}, {G1, G1, G1});
  auto B4 = block_gf<imfreq>(1);

  // test hdf5
  {
   H5::H5File file("ess_gf.h5", H5F_ACC_TRUNC);
   h5_write(file, "B3", B3);
  }

  {
   H5::H5File file("ess_gf.h5", H5F_ACC_RDONLY);
   std::cout << "B4 mesh" << B4.mesh().size() << std::endl;
   h5_read(file, "B3", B4);
   std::cout << "B4 mesh" << B4.mesh().size() << std::endl;
  }

  B1[0][0] = 98;
  // not implemented yet
  // B3["a"][0] = 98;

  auto View = make_block_gf_view(G1, G2, G3);

  std::cout << "Number of blocks " << View.mesh().size() << std::endl;
  auto g0 = View[0];
  auto g0v = View[0]();

  auto Gv = g0();

  Gv[0] = 20;
  TEST(G1(0));
  Gv[0] = 0;

  g0v[0] = 3.2;
  TEST(G1(0));

  // Operation
  g0[0] = 3.2;
  TEST(View[0](0));
  View = View / 2;
  TEST(View[0](0));

  // try the loop over the block.
  for (auto& g : View) {
   g[0] = 20;
  }
  for (auto& g : B1) {
   g[0] = 20;
  }

  // check chaining of clef
  clef::placeholder<0> b_;
  clef::placeholder<1> om_;
  B1[b_][om_] << b_ / (om_ + 2);

  auto B11 = B1;

  B1[b_](om_) << B11[b_](om_) * B1[b_](om_) * B11[b_](om_);

  // does not work
  // std::vector<gf<imfreq>> green_v;
  // green_v.emplace_back({beta, Fermion}, {2,2} );

  // test reinterpretation
  // compile only, add more test here
  auto gs1 = gf<imfreq, scalar_valued>({beta, Fermion});
  auto bgs = make_block_gf<imfreq>(3, gs1);
  auto bg = reinterpret_scalar_valued_gf_as_matrix_valued(bgs);
 }
 TRIQS_CATCH_AND_ABORT;
}
