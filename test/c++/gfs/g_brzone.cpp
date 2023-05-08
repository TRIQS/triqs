#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::gfs;
using namespace triqs::clef;
using namespace triqs::arrays;
using namespace triqs::lattice;

double pi = M_PI;

double f(auto & k){ 
  return -2 * (cos(k[0]) + cos(k[1]));
}

TEST(Gfk, base) {

  int N = 10;
  //int S = 2;
  placeholder<0> k_;

  auto bz = brillouin_zone{bravais_lattice{nda::eye<double>(2)}};
  auto gk = gf<brzone, scalar_valued>{{bz, N}};

  gk(k_) << -2 * (cos(k_(0)) + cos(k_(1)));

 auto m = gk.mesh();
 auto m2 = brzone{bz, 3*N};
 
 EXPECT_FALSE(m. mesh_hash()  == m2. mesh_hash() );

 // get 2 mp from m, 2 from m2
  auto p = m.begin();
  auto mp0 = *p;
  ++p;
  auto mp1 = *p;
  
  auto p2 = m2.begin();
  auto m2p0 = *p2;
  ++p2;
  auto m2p1 = *p2;
 
  // 
 ASSERT_DEBUG_DEATH( gk[m2p1], "mesh_hash violated");
 
 EXPECT_EQ(mp0.get_idx(), (std::array<long,3>{0,0,0}));
 EXPECT_EQ(mp1.get_idx(), (std::array<long,3>{0,1,0}));
 EXPECT_EQ(m2p1.get_idx(), (std::array<long,3>{0,1,0}));

  auto mp2 = mp0 + mp1;  
  EXPECT_EQ(mp2.get_idx(), (std::array<long,3>{0,1,0}));

 EXPECT_COMPLEX_NEAR(gk[mp0 + mp1], gk[mp1]);
 EXPECT_COMPLEX_NEAR(gk(mp0), f(mp0.value()));
 EXPECT_COMPLEX_NEAR(gk(mp1), f(mp1.value()));

 PRINT( make_regular( mp2.value()/pi) );

 EXPECT_COMPLEX_NEAR(gk(mp0 + mp1),  (-2* ( cos(0) + cos(0.2*M_PI))), 1.e-10);

 PRINT(gk[closest_mesh_pt(mp1 + m2p1)]);

}

// --------------------------

TEST(Gfk, loopkq) {

  int N = 10;
  //int S = 2;
  placeholder<0> k_;
  double tol = 1.e-13;

  auto bz = brillouin_zone{bravais_lattice{nda::eye<double>(2)}};
  auto g = gf<brzone, scalar_valued>{{bz, N}};

  g(k_) << -2 * (cos(k_(0)) + cos(k_(1)));

 auto m = g.mesh();
 auto m2 = brzone{bz, 3*N};

 for (auto k : m) 
  for (auto q : m) 
  {
    // MP sum
    {   auto kq_idx =  m.idx_modulo(k.idx + q.idx);
    EXPECT_COMPLEX_NEAR(g[kq_idx], g[k+q], tol);
  }
  
  {   auto kq_idx =  m.idx_modulo(k.idx - q.idx);
    EXPECT_COMPLEX_NEAR(g[kq_idx], g[k-q], tol);
  }
 
  // value +
  {   auto kq_idx =  m.idx_modulo(k.idx + q.idx);
    EXPECT_COMPLEX_NEAR(g[kq_idx], g(k+q), tol);
  }
  // value -
  {   auto kq_idx =  m.idx_modulo(k.idx - q.idx);
    EXPECT_COMPLEX_NEAR(g[kq_idx], g(k-q), tol);
  }
 
  }

}


MAKE_MAIN;
