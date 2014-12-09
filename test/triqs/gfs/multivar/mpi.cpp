#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp>
#include <fstream>

namespace h5 = triqs::h5;
using namespace triqs;
using namespace triqs::gfs;
using namespace triqs::arrays;
using namespace triqs::lattice;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> " << (X) << std::endl << std::endl;

using gf3_s = gf<cartesian_product<imfreq, imfreq, imfreq>, scalar_valued>;

int main(int argc, char* argv[]) {

 mpi::environment env(argc, argv);
 mpi::communicator world;

 std::ofstream out("node" + std::to_string(world.rank()));

 try {
  int nw = 2, nbw = 10;
  double beta = 10;
  clef::placeholder<0> k_;
  clef::placeholder<1> q_;
  clef::placeholder<2> r_;
  clef::placeholder<3> iw_;
  clef::placeholder<4> inu_;
  clef::placeholder<5> inup_;

  auto g = gf3_s{{{beta, Boson, nbw}, {beta, Fermion, nw}, {beta, Fermion, nw}}};
  auto g2 = g;

  g(iw_, inu_, inup_) << inu_ + 10 * inup_ + 100 * iw_;

  {
   out << "reduction " << std::endl;
   g2 = mpi::reduce(g, world);
   out << g2.data() << std::endl;
  }

  {
   out << "all reduction " << std::endl;
   g2 = mpi::all_reduce(g, world);
   out << g2.data() << std::endl;
  }

 {
   out << "scatter-gather test with =" << std::endl;
   auto g2b = g;

   g2 = mpi::scatter(g);
   g2(iw_, inu_, inup_) << g2(iw_, inu_, inup_) * (1 + world.rank());
   g2b = mpi::gather(g2);

   for (int i = 0; i < nw; ++i)
    for (int j = 0; j < nw; ++j) out << g2b.data()(range(), i, j) << std::endl;
  }
 }
 TRIQS_CATCH_AND_ABORT;
}

