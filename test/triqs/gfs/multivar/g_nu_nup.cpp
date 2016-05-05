#include <triqs/test_tools/gfs.hpp>
using namespace triqs::clef;

TEST(Gf, G_nu_nup) {

 double beta = 1;

 auto G = gf<cartesian_product<imfreq, imfreq>, tensor_valued<4>>{{{beta, Fermion, 100}, {beta, Fermion, 100}}, {2, 2, 2, 2},
                                                                  make_memory_layout(0, 2, 3, 1, 4, 5)};

 auto sh = triqs::arrays::memory_layout<6>{triqs::utility::mini_vector<int, 6>{0, 2, 3, 1, 4, 5}};
 EXPECT_EQ(G.data().indexmap().get_memory_layout(), sh);
 
 placeholder<0> nu_;
 placeholder<1> nup_;

 G(nu_, nup_) << 1 / (nu_ + nup_ + 1);

 dcomplex res1 = G[{2, 3}](0,0,0,0);
 dcomplex res = 1 / (matsubara_freq { 2, beta, Fermion } + matsubara_freq { 3, beta, Fermion } + 1);
 EXPECT_CLOSE(res1, res);
 rw_h5(G, "ess_g_nu_nup.h5", "g");
}

TEST(GfCartesian, H5_RW_Evaluator){
 double beta=1;
 auto g = gf<cartesian_product<imfreq, imfreq>, matrix_valued>{{{beta, Fermion, 5}, {beta, Boson, 5}}, {1,1}};
 g()=2;
 
 h5::file file("g_nu_nup.h5", H5F_ACC_TRUNC );
 h5_write(file, "g", g);
 gf<cartesian_product<imfreq, imfreq>, matrix_valued> g2{};
 h5_read(file, "g", g2);

 EXPECT_GF_NEAR(g,g2);

 auto w0 = matsubara_freq(0, beta, Fermion);
 auto W10 = matsubara_freq(10, beta, Boson);
 auto W0 = matsubara_freq(0, beta, Boson);
 EXPECT_ARRAY_NEAR(g(w0, W10),g2(w0, W10));
 EXPECT_ARRAY_NEAR(g(w0, W0),g2(w0, W0));
}

TEST(BlockGfCartesian, H5_RW_Evaluator){
 double beta=1;
 auto g = gf<cartesian_product<imfreq, imfreq>, matrix_valued>{{{beta, Fermion, 5}, {beta, Boson, 5}}, {1,1}};
 g()=2;
 auto G = make_block_gf({"up"}, {g});
 //EXPECT_ARRAY_NEAR(get_target_shape(G[0]), mini_vector<size_t,2>{1,1});
 h5::file file("g_nu_nup.h5", H5F_ACC_TRUNC );
 h5_write(file, "G", G);

 block_gf<cartesian_product<imfreq, imfreq>, matrix_valued> G2{};
 gf<cartesian_product<imfreq, imfreq>, matrix_valued> g2{};
 h5_read(file, "G", G2);
 EXPECT_GF_NEAR(G[0],G2[0]);

 auto w0 = matsubara_freq(0, beta, Fermion);
 auto W10 = matsubara_freq(10, beta, Boson);
 EXPECT_ARRAY_NEAR(G[0](w0, W10),G2[0](w0, W10));

}

TEST(BlockGfCartesian, OutOfBounds){
 double beta=1;
 triqs::clef::placeholder<0> om_;
 triqs::clef::placeholder<1> nu_;
 auto g_2w = gf<cartesian_product<imfreq, imfreq>, tensor_valued<3>>{{{beta, Fermion, 5}, {beta, Boson, 5, matsubara_mesh_opt::positive_frequencies_only}}, {2,2,2}};
 g_2w(om_,nu_) << 1/(om_+nu_)*1/om_;
 auto g_w = gf<imfreq, matrix_valued>{{beta, Fermion, 10}, {1,1}}; //longer than g
 auto W0 = matsubara_freq(0, beta, Boson);
 for(auto const & om : g_w.mesh()){
  auto x = g_2w(om, W0)(0,0,0) ;
 }

}
TEST(BlockGfCartesian, VectorConstruction){
 double beta=1;
 auto g_2w = gf<cartesian_product<imfreq, imfreq>, tensor_valued<3>>{{{beta, Fermion, 5}, {beta, Boson, 5, matsubara_mesh_opt::positive_frequencies_only}}, {2,2,2}};
 block_gf<cartesian_product<imfreq,imfreq>, tensor_valued<3>> L(2);
 for(int i=0;i<2;i++) L[i] = g_2w;

 auto w0 = matsubara_freq(1000, beta, Fermion);
 auto W0 = matsubara_freq(1000, beta, Boson);
 auto x = L[0](w0, W0);
 x(0,0,0); 
}

TEST(BlockGf, H5_RW_Evaluator){
 double beta=1;

 auto g = gf<imfreq, matrix_valued>{{beta, Boson, 5}, {1,1}};
 g()=2;
 auto G = make_block_gf({"up"}, {g});


 h5::file file("g_nu_nup.h5", H5F_ACC_TRUNC );
 h5_write(file, "G", G);
 h5_write(file, "g", g);

 block_gf<imfreq, matrix_valued> G2{};
 gf<imfreq, matrix_valued> g2{};
 h5_read(file, "G", G2);
 h5_read(file, "g", g2);

 EXPECT_GF_NEAR(G[0],G2[0]);
 EXPECT_GF_NEAR(g,g2);

 auto w0 = matsubara_freq(0, beta, Fermion);
 auto W0 = matsubara_freq(0, beta, Boson);
 auto W10 = matsubara_freq(10, beta, Boson);
 EXPECT_ARRAY_NEAR(G[0](W10),G2[0](W10));
 EXPECT_ARRAY_NEAR(g(W10),g2(W10));
}




MAKE_MAIN;
