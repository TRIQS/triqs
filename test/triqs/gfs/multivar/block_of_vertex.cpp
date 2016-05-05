/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015 by O. Parcollet, I. Krivenko
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <triqs/test_tools/gfs.hpp>

using namespace triqs::clef;
using namespace triqs::lattice;

using triqs::clef::placeholder;
// scalar valued gf_vertex
using gf_vertex_t = gf<cartesian_product<imfreq, imfreq, imfreq>, scalar_valued>;
using gf_vertex_tensor_t = gf<cartesian_product<imfreq, imfreq, imfreq>, tensor_valued<3>>;

#define ITERATE_I1I2I3 \
for(int i1 = 0; i1 < n_im_freq; ++i1) \
for(int i2 = 0; i2 < n_im_freq; ++i2) \
for(int i3 = 0; i3 < n_im_freq; ++i3) 

double beta = 10.0;
auto iw_n = [](int n){ return M_PI*(2*n+1)/beta*1_j; };
int n_im_freq = 10;

std::vector<int> indices1 = {0,1};
std::vector<int> indices2 = {0,1,2};

const auto m = gf_mesh<imfreq>{beta, Fermion, n_im_freq};

// -----------------------------------------------------

TEST(Gf, BlockOfVertexScalar) {

  auto vertex = gf_vertex_t{{m, m, m}};

  placeholder<0> w0_;
  placeholder<1> w1_;
  placeholder<2> w2_;
  placeholder<3> s1_;
  placeholder<4> s2_;

  // Now make the block of vertices:
  auto B = make_block2_gf(indices1.size(), indices2.size(), vertex);

  // The total number of blocks
  // TODO: these lines should compile and run
  EXPECT_EQ(B.size1(), indices1.size());
  EXPECT_EQ(B.size2(), indices2.size());
  //EXPECT_EQ(B.size(), indices1.size() * indices2.size());

  // Assign a function
  for(int u : indices1)
    for(int v : indices2)
      //B[{u,v}] = vertex;
      B(u,v) = vertex;

  // Assign expression
  B(s1_,s2_)(w0_, w1_, w2_) << w0_ +  (s1_-1)*(s2_-1)/(w1_ - 0.5) + (s1_-s2_)/(w2_ + 0.5);

  // Checking the result
  auto check = [](int s1, int s2, int i1, int i2, int i3) {
    return iw_n(i1) + (s1-1)*(s2-1)/(iw_n(i2) - 0.5) + (s1-s2)/(iw_n(i3) + 0.5);
  };

  for(int u : indices1)
    for(int v : indices2)
      ITERATE_I1I2I3 {
        EXPECT_CLOSE((B(u, v)[{i1, i2, i3}]), check(u, v, i1, i2, i3));

        // Testing map in a very simple case
        std::vector<std::vector<dcomplex>> rr = map([i1,i2,i3](auto const& g) { return g(i1, i2, i3); }, B);
        EXPECT_CLOSE(rr[u][v], check(u, v, i1, i2, i3));
      }

  // Should not compile
  // B[{0,0}] (0,0,0)= 89;
  //std::cout  <<  B[{0,0}] (0,0,0) << std::endl;

  // Testing iterator and const_iterator
  ITERATE_I1I2I3 {
    for(auto & x : B)  {
     x [{i1,i2,i3}] = 1.0/((iw_n(i1) - 3.0)*iw_n(i2)*(iw_n(i3) + 3.0));
     //x(i1,i2,i3) = 1.0/((iw_n(i1) - 3.0)*iw_n(i2)*(iw_n(i3) + 3.0));
    }
  }
  ITERATE_I1I2I3 {
    // TODO: this assertion fails ...
    for(auto const& x : B) EXPECT_CLOSE(x(i1,i2,i3),1.0/((iw_n(i1) - 3.0)*iw_n(i2)*(iw_n(i3) + 3.0)));
  }

  // HDF5
  rw_h5(B, "vertexBlockS", "B");
}

// -----------------------------------------------------

TEST(Gf, BlockOfVertexTensor) {

  // now with indices
  auto vertex = gf_vertex_tensor_t{{m, m, m}, {2, 2, 2}};
  vertex[{0, 1, 0}](0, 0, 0) = 10;

  // Now make the block of vertices:
  auto B = make_block2_gf(indices1.size(), indices2.size(), vertex);

  // The total number of blocks
  // TODO: these lines should compile and run
  //EXPECT_EQ(size1(B), indices1.size());
  //EXPECT_EQ(size2(B), indices2.size());
  //EXPECT_EQ(n_blocks(B), indices1.size() * indices2.size());

  for(int u : indices1)
    for(int v : indices2)
      EXPECT_CLOSE((B(u,v)[{0, 1, 0}](0, 0, 0)), 10);

  // HDF5
  rw_h5(B, "vertexBlockT", "B");
}
#undef ITERATE_I1I2I3

MAKE_MAIN;

