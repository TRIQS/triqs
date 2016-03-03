#include <triqs/test_tools/arrays.hpp>
#include <triqs/statistics/histograms.hpp>
#include <triqs/arrays.hpp>
using namespace triqs::statistics;
namespace arrays = triqs::arrays;

histogram make_hi1() {
 histogram hi1{0, 10};
 std::vector<double> data{-1, 0, 0, 0, 1, 2, 2, 2, 3, 5, 9, 32};
 for (auto i : data) hi1 << i;
 return hi1;
}

histogram make_hd1() {
 histogram hd1{0, 10, 21};
 std::vector<double> data{-10, -0.05, 1.1, 2.0, 2.2, 2.9, 3.4, 5, 9, 10.0, 10.5, 12.1, 32.2};
 for (auto i : data) hd1 << i;
 return hd1;
}

histogram make_hi2() {
 histogram hi2{0, 10};
 std::vector<double> data{1.1, 2.0, 2.2, 2.9, 3.4, 5, 9, 10.0, 10.1, 12.1, 32.2};
 for (auto i : data) hi2 << i;
 return hi2;
}

histogram make_hd2() {
 histogram hd2{0, 10, 11};
 std::vector<double> data{1.1, 2.0, 2.2, 2.9, 3.4, 5, 9, 10.0, 10.1, 12.1, 32.2};
 for (auto i : data) hd2 << i;
 return hd2;
}

// ----- TESTS ------------------

TEST(Statistics, histo) {

 using std::abs;

 auto hi1 = make_hi1();
 auto hd1 = make_hd1();
 auto hi2 = make_hi2();
 auto hd2 = make_hd2();

 arrays::vector<double> true_hi1 = {3, 1, 3, 1, 0, 1, 0, 0, 0, 1, 0};
 arrays::vector<double> true_hd1 = {0, 0, 1, 0, 2, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1};
 arrays::vector<double> true_h2 = {0, 1, 2, 2, 0, 1, 0, 0, 0, 1, 1};

 EXPECT_ARRAY_NEAR(hi1.data(), true_hi1);
 EXPECT_EQ(hi1.n_lost_pts(), 2);
 EXPECT_EQ(hi1.n_data_pts(), 10);

 auto cdf_hi1 = cdf(hi1);
 EXPECT_EQ(cdf_hi1.data()[cdf_hi1.size() - 1], 1.0);

 EXPECT_ARRAY_NEAR(hd1.data(), true_hd1);
 EXPECT_EQ(hd1.n_lost_pts(), 5);

 EXPECT_ARRAY_NEAR(hi2.data(), true_h2);
 EXPECT_EQ(hi2.n_lost_pts(), 3);
 EXPECT_ARRAY_NEAR(hi2.data(), hd2.data());
}

// ------------------------

MAKE_MAIN;

