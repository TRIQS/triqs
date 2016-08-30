#include <triqs/test_tools/arrays.hpp>
#include <triqs/arrays.hpp>
#include <triqs/det_manip/det_manip.hpp>
namespace arrays = triqs::arrays;
using triqs::det_manip::det_manip;

struct func {
  // gives the coefficients of the matrix (function F of the documentation)
  double operator()(int x, int y) const {
    if ((x<0) and (y<0)) return 0;
    else return x + y;
  }
};

det_manip<func> init_dm(){
  func f;
  std::vector<int> initial_x{-2,2}, initial_y{-5,3};
  det_manip<func> dm(f, initial_x, initial_y);
  std::cerr << "matrix = " << dm.matrix() << std::endl;
  std::cerr << "det    = " << dm.determinant() << std::endl;
  return dm;
}

det_manip<func> make_zero(det_manip<func> dm){
  dm.remove(1,1);
  std::cerr << "matrix = " << dm.matrix() << std::endl;
  std::cerr << "det    = " << dm.determinant() << std::endl;
  return dm;
}

det_manip<func> insert_nonzero(det_manip<func> dm){
  dm.insert(1,1,6,4);
  std::cerr << "matrix = " << dm.matrix() << std::endl;
  std::cerr << "det    = " << dm.determinant() << std::endl;
  return dm;
}

det_manip<func> make_empty_from_zero(det_manip<func> dm){
  dm.remove(0,0);
  std::cerr << "matrix = " << dm.matrix() << std::endl;
  std::cerr << "det    = " << dm.determinant() << std::endl;
  return dm;
}

// ----- TESTS ------------------

TEST(det_manip, det_manip_zero_mat) {

// using std::abs;

 auto dm1 = init_dm();
 auto dm2 = make_zero(dm1);
 auto dm3 = insert_nonzero(dm2);
 auto dm4 = make_empty_from_zero(dm2);
 arrays::matrix<double> true_dm1 = {{0,1},{-3,5}};
 arrays::matrix<double> true_dm2 = {{0}};
 arrays::matrix<double> true_dm3 = {{0,2},{1,10}};
 arrays::matrix<double> true_dm4 = {};
 
 EXPECT_ARRAY_NEAR(dm1.matrix(), true_dm1);
 EXPECT_EQ(dm1.determinant(), 3);

 EXPECT_ARRAY_NEAR(dm2.matrix(), true_dm2);
 EXPECT_EQ(dm2.determinant(), 0);
 
 EXPECT_ARRAY_NEAR(dm3.matrix(), true_dm3);
 EXPECT_EQ(dm3.determinant(), -2);

 EXPECT_ARRAY_NEAR(dm4.matrix(), true_dm4);
 EXPECT_EQ(dm4.determinant(), 1);
}

// ------------------------

MAKE_MAIN;
