#include <numeric>

#include <triqs/test_tools/arrays.hpp>
#include <triqs/arrays.hpp>
#include <triqs/det_manip/det_manip.hpp>
namespace arrays = triqs::arrays;
using vector = arrays::vector<double>;
using matrix = arrays::matrix<double>;
using triqs::det_manip::det_manip;

struct func {
  double operator()(int x, int y) const {
    if ((x<0) and (y<0)) return 0;
    else return 2/double(x + y);
  }
};

det_manip<func> init_dm(int size){
  func f;
  std::vector<int> initial_x(size), initial_y(size);
  std::iota(initial_x.begin(), initial_x.end(), 1);
  std::iota(initial_y.begin(), initial_y.end(), 1);
  det_manip<func> dm(f, initial_x, initial_y);
  std::cerr << "matrix = " << dm.matrix() << std::endl;
  std::cerr << "det    = " << dm.determinant() << std::endl;
  return dm;
}

// ----- TESTS ------------------

TEST(det_manip, det_manip_refill_0) {
 auto dm = init_dm(3);
 EXPECT_CLOSE(1.0/5400, dm.determinant());

 EXPECT_NEAR(5400, dm.try_refill(vector{}, vector{}), 1e-9);
 dm.complete_operation();

 EXPECT_ARRAY_NEAR(matrix{}, dm.matrix());
 EXPECT_CLOSE(1, dm.determinant());
 EXPECT_ARRAY_NEAR(matrix{}, dm.inverse_matrix());
}

TEST(det_manip, det_manip_refill_2) {
 auto dm = init_dm(3);
 EXPECT_CLOSE(1.0/5400, dm.determinant());

 EXPECT_NEAR(300.0, dm.try_refill(vector{1,2}, vector{1,2}), 1e-10);
 dm.complete_operation();

 EXPECT_ARRAY_NEAR(matrix{{1.0, 2.0/3},{2.0/3, 0.5}}, dm.matrix());
 EXPECT_CLOSE(1.0/18, dm.determinant());
 EXPECT_ARRAY_NEAR(matrix{{9, -12},{-12, 18}}, dm.inverse_matrix());
}

TEST(det_manip, det_manip_refill_4) {
 auto dm = init_dm(3);
 EXPECT_CLOSE(1.0/5400, dm.determinant());

 EXPECT_NEAR(1.0/4900, dm.try_refill(vector{1,2,3,4}, vector{1,2,3,4}), 1e-10);
 dm.complete_operation();

 EXPECT_ARRAY_NEAR(matrix{{1.0,   2.0/3, 1.0/2, 2.0/5},
                          {2.0/3, 1.0/2, 2.0/5, 1.0/3},
                          {1.0/2, 2.0/5, 1.0/3, 2.0/7},
                          {2.0/5, 1.0/3, 2.0/7, 1.0/4}}, dm.matrix());
 EXPECT_CLOSE(1.0/26460000, dm.determinant());
 EXPECT_ARRAY_NEAR(matrix{{100, -600, 1050, -560},
                          {-600, 4050, -7560, 4200},
                          {1050, -7560, 14700, -8400},
                          {-560, 4200, -8400, 4900}}, dm.inverse_matrix(), 1e-8);
}

TEST(det_manip, det_manip_refill_empty) {
 auto dm = init_dm(0);
 EXPECT_CLOSE(1.0, dm.determinant());

 EXPECT_NEAR(1.0/18, dm.try_refill(vector{1,2}, vector{1,2}), 1e-10);
 dm.complete_operation();

 EXPECT_ARRAY_NEAR(matrix{{1.0, 2.0/3},{2.0/3, 0.5}}, dm.matrix());
 EXPECT_CLOSE(1.0/18, dm.determinant());
 EXPECT_ARRAY_NEAR(matrix{{9, -12},{-12, 18}}, dm.inverse_matrix());
}

// ------------------------

MAKE_MAIN;
