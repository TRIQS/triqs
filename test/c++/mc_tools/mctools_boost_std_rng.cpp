// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#include <triqs/test_tools/arrays.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

#include <random>
#include <vector>

// Test that std::mt19937 and boost::random::mt19937 produce the same numbers.
// Conclusion: The std and boost generator give the same numbers.
TEST(TRIQSMCTools, BoostVsStdMersenneTwister) {
  int const seed = 1352;

  // expected random numbers
  std::vector<long> result = {869874994,  3205046262, 2119020267, 2669095628, 275633191,  335224535,  1399249061, 4059871834, 1506360241, 2646120513,
                              932791869,  1038906848, 391582322,  2962159196, 1246635690, 589901067,  1065559423, 91395844,   4212208358, 3609786960,
                              2429350836, 614673344,  566349528,  3453660734, 1803951428, 2872537556, 2290263138, 303337700,  1592633174, 918874404,
                              1142687065, 222876524,  2795340428, 2430863508, 634253287,  2430641383, 4085971400, 1898206763, 4124349013, 1443009189,
                              3764761512, 186462643,  3075725912, 331081090,  3404927489, 1600419147, 3150852496, 2733533252, 2808125817, 1223983741,
                              3283812587, 3661621427, 2081695262, 3535349663, 2829108992, 1063226049, 1109558379, 3780004780, 4023168629, 2812834780,
                              1801392963, 3151601571, 1439050999, 2986946488, 1629341800, 3471040045, 135632356,  2852529214, 2486605945, 1895271125,
                              763145322,  132788317,  1803634386, 4063571019, 478127399,  22977226,   3759004003, 1935982873, 2024725320, 3279517291,
                              2588561942, 3364937223, 3236324186, 2508746520, 1305720758, 3713056371, 4291396099, 4172364413, 1255934297, 2575364421,
                              157096516,  567018849,  1964661445, 3336536577, 162680414,  997947546,  2422402185, 234663097,  1843361535, 1752705663};

  // std
  std::mt19937 gen_std(seed);
  for (int i = 0; i < 100; ++i) EXPECT_EQ(result[i], gen_std());

  // boost
  boost::mt19937 gen_boost(seed);
  for (int i = 0; i < 100; ++i) EXPECT_EQ(result[i], gen_boost());
}

#ifdef RANDOM_TEST_UNIFORM
// Test that std::uniform_real_distribution and boost::random::uniform_real_distribution produce the same numbers.
// Conclusion: This is not the case. However, this is not guaranteed to be implementation independent by the std.
TEST(TRIQSMCTools, BoostVsStdUniformDistribution) {
  int const seed = 1352;

  // expected numbers
  std::vector<double> result = {
     0.746232984169,   0.6214472531558, 0.07805054426756, 0.9452625723383,  0.6160979423092,  0.2418893501668,  0.6896814322311, 0.1373470451893,
     0.02127975324357, 0.8404690215785, 0.1431147904521,  0.80411805169,    0.6688147681812,  0.07062631205964, 0.2139421190067, 0.0518924846002,
     0.5659795153539,  0.56592779773,   0.4419607026389,  0.3359767584969,  0.04341421739118, 0.07708582345306, 0.3726266202966, 0.6364503066832,
     0.2849809223911,  0.8525376738432, 0.8231377376906,  0.2475516054916,  0.8801009460023,  0.6549141325375,  0.7337894224141, 0.6954526734387,
     0.808164487914,   0.6641562129444, 0.4412771960671,  0.03091718935819, 0.9461238559848,  0.00534980234488, 0.4507561386272, 0.7635721218473,
     0.7834604996263,  0.5841130671914, 0.8645133048538,  0.9714542920699,  0.5996237558528,  0.1320193635851,  0.7768479589041, 0.2323527694768,
     0.05463676004764, 0.408083587752,  0.6369825399466,  0.5685970584032,  0.1235360900372,  0.9202423981236,  0.7115838618262, 0.02983485111151,
     0.76981373137,    0.0867099417063, 0.07104946619888, 0.6242631153853,  0.9333818935535,  0.2555663074641,  0.5992051313843, 0.5401568983755,
     0.7769657207188,  0.1981726659902, 0.1661893182446,  0.834562142064,   0.9424356488108,  0.04133228686251, 0.4650720410434, 0.07820202766968,
     0.9884859219395,  0.1590063209914, 0.6488666015855,  0.7387352915971,  0.7585917578883,  0.5503377983743,  0.9244397353327, 0.6447530066677,
     0.6129745757522,  0.2542404394466, 0.2026888834774,  0.2527748172769,  0.6740506770768,  0.9107106919416,  0.6213113793034, 0.1554119577284,
     0.7805296694581,  0.660779124941,  0.7750959913801,  0.0938933767948,  0.6609784932051,  0.6148894861399,  0.4792942971708, 0.6545062197143,
     0.6110897641314,  0.3611626664627, 0.2771070578171,  0.5601750233273};

  // std
  std::mt19937 gen_std(seed);
  std::uniform_real_distribution<double> uni_std;
  for (int i = 0; i < 100; ++i) EXPECT_NEAR(result[i], uni_std(gen_std), 1.e-12);

  // boost
  boost::uniform_real<> uni_boost;
  boost::variate_generator<boost::mt19937, boost::uniform_real<>> vg_boost(boost::mt19937(seed), uni_boost);
  for (int i = 0; i < 100; ++i) EXPECT_NEAR(result[i], vg_boost(), 1.e-12);
}
#endif

MAKE_MAIN;
