#include <triqs/test_tools/gfs.hpp>

#include <triqs/lattice/cluster_mesh.hpp>


TEST(ClusterMesh, FindCellDims) { 

 EXPECT_THROW( find_cell_dims(matrix<double>{{{1./4.5,0.,0.},{0.,1.,0.},{0.,0.,1.}}}), triqs::runtime_error);

 auto k_s1 = find_cell_dims(matrix<double>{{{1.,0.,0.},{0.,1.,0.},{0.,0.,1.}}});
 auto k_s1_res = utility::mini_vector<int,3>{1,1,1};
 EXPECT_EQ(k_s1, k_s1_res);
 auto k_s2 = find_cell_dims(inverse(matrix<double>{{{1.,2.,0.},{2.,1.,0.},{0.,0.,1.}}}));
 auto k_s2_res = utility::mini_vector<int,3>{3,1,1};
 EXPECT_EQ(k_s2, k_s2_res);
 auto k_s3 = find_cell_dims(inverse(matrix<double>{{{1.,2.,0.},{-2.,2.,0.},{0.,0.,1.}}}));
 auto k_s3_res = utility::mini_vector<int,3>{3,2,1};
 EXPECT_EQ(k_s3, k_s3_res);
 auto k_s4 = find_cell_dims(inverse(matrix<double>{{{6.,6.,0.},{-6.,6.,0.},{0.,0.,1.}}}));
 auto k_s4_res = utility::mini_vector<int,3>{12,6,1};
 EXPECT_EQ(k_s4, k_s4_res);
 }

TEST(ClusterMesh, Basic){

 auto m1 = cluster_mesh(make_unit_matrix<double>(3), 3*make_unit_matrix<int>(3));
 EXPECT_EQ(m1.size(), 3*3*3);
 auto m2 = cluster_mesh(make_unit_matrix<double>(3), matrix<int>({{1,2,0},{2,1,0},{0,0,1}}));
 EXPECT_EQ(m2.size(), 3);
 auto m3 = cluster_mesh(make_unit_matrix<double>(3), matrix<int>({{6,6,0},{-6,6,0},{0,0,1}}));
 EXPECT_EQ(m3.size(), 72);

}
 
MAKE_MAIN;

