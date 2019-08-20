#define TRIQS_DEBUG
#include <triqs/test_tools/gfs.hpp>

using namespace triqs::gfs;
using namespace triqs::clef;

// Index placeholders
// FIXME:  Make a proper test

placeholder<0> a;
placeholder<1> b;
placeholder<2> c;
placeholder<3> d;

void disconnected_PP_G2(gf_view<imfreq, matrix_valued> g, gf_view<prod<imfreq, imfreq, imfreq>, tensor_valued<4>> g2,
                        bool diag_block = false) {

  g2() = 0.0;
  for (auto const &Omega : std::get<0>(g2.mesh())) {
    for (auto const &n : std::get<1>(g2.mesh())) {
      auto mesh = std::get<2>(g2.mesh());
      // using the .index() method is not correct, Omega is a bosonic Matsubara!
      typename decltype(mesh)::mesh_point_t np{mesh, Omega.index() - n.index()};
      g2[{Omega, n, np}](a, b, c, d) << g(n)(b, a) * g(Omega - n)(d, c);

      // also compiles
      g2[{Omega, n, Omega - n}](a, b, c, d) << g(n)(b, a) * g(Omega - n)(d, c);
    }
  }
  g2 *= g.mesh().domain().beta;
}

MAKE_MAIN
