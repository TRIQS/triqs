#include <triqs/test_tools/gfs.hpp>
using namespace triqs::clef;
using namespace triqs::lattice;

using triqs::clef::placeholder;
// scalar valued gf_vertex
using gf_vertex_t        = gf<cartesian_product<imfreq, imfreq, imfreq>, scalar_valued>;
using gf_vertex_tensor_t = gf<cartesian_product<imfreq, imfreq, imfreq>, tensor_valued<3>>;

// -----------------------------------------------------

TEST(Gf, VertexScalar) {

 double beta   = 10.0;
 int n_im_freq = 10;

 auto m = gf_mesh<imfreq>{beta, Fermion, n_im_freq};

 auto vertex = gf_vertex_t{{m, m, m}};

 placeholder<0> w0_;
 placeholder<1> w1_;
 placeholder<2> w2_;

 vertex(w0_, w1_, w2_) << w0_ + 2.3 * w1_ + 3.1 * w2_;

 vertex[{0, 0, 0}] = 10;

 EXPECT_CLOSE((vertex[{1, 6, 3}]),
              (M_PI * (2 * 1 + 1) / 10.0 + 2.3 * M_PI * (2 * 6 + 1) / 10.0 + 3.1 * M_PI * (2 * 3 + 1) / 10.0) * 1_j);

 vertex[{0, 0, 0}] *= 2;

 EXPECT_CLOSE(vertex(0, 0, 0), 20);

 rw_h5(vertex, "vertexS", "v");
}

// -----------------------------------------------------

TEST(Gf, VertexTensor) {

 double beta   = 10.0;
 int n_im_freq = 10;

 auto m = gf_mesh<imfreq>{beta, Fermion, n_im_freq};

 // now with indices
 auto vertex = gf_vertex_tensor_t{{m, m, m}, {2, 2, 2}};

 vertex[{0, 0, 0}](0, 0, 0) = 10;
 EXPECT_CLOSE((vertex[{0, 0, 0}](0, 0, 0)), 10);

 rw_h5(vertex, "vertexT", "v");
}
MAKE_MAIN;
