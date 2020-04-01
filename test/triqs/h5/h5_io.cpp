/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet, N. Wentzell
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
#include <iostream>
#include <tuple>
#include <h5/h5.hpp>
#include <triqs/test_tools/arrays.hpp>

TEST(H5Io, Pair) {

  // write
  std::pair<std::string, int> m                  = {"a", 1};
  std::pair<std::string, std::vector<double>> mv = {"a", {1.0, 2.0}};

  {
    h5::file file{"test_pair.h5", 'w'};
    h5::group grp{file};
    h5_write(grp, "pair_int_str", m);
    h5_write(grp, "pair_str_vec", mv);
  }

  // read
  std::pair<std::string, int> mm;
  std::pair<std::string, std::vector<double>> mmv;

  {
    h5::file file{"test_pair.h5", 'r'};
    h5::group grp{file};
    h5_read(grp, "pair_int_str", mm);
    h5_read(grp, "pair_str_vec", mmv);
  }

  // compare
  EXPECT_EQ(m, mm);
  EXPECT_EQ(mv, mmv);
}

TEST(H5Io, Tuple) {

  // write
  std::tuple<std::string, int> m                  = {"a", 1};
  std::tuple<std::string, std::vector<double>> mv = {"a", {1.0, 2.0}};

  {
    h5::file file{"test_tuple.h5", 'w'};
    h5::group grp{file};
    h5_write(grp, "tpl_int_str", m);
    h5_write(grp, "tpl_str_vec", mv);
  }

  // read
  std::tuple<std::string, int> mm;
  std::tuple<std::string, std::vector<double>> mmv;

  {
    h5::file file{"test_tuple.h5", 'r'};
    h5::group grp{file};
    h5_read(grp, "tpl_int_str", mm);
    h5_read(grp, "tpl_str_vec", mmv);
  }

  // compare
  EXPECT_EQ(m, mm);
  EXPECT_EQ(mv, mmv);
}

TEST(H5Io, Map) {

  // write
  std::map<std::string, int> m                  = {{"a", 1}, {"b", 2}};
  std::map<std::string, std::vector<double>> mv = {{"a", {1.0, 2.0}}, {"b", {2.0, 3.0, 4.0}}};

  {
    h5::file file{"test_map.h5", 'w'};
    h5::group grp{file};
    h5_write(grp, "map_int", m);
    h5_write(grp, "map_vec", mv);
  }

  // read
  std::map<std::string, int> mm;
  std::map<std::string, std::vector<double>> mmv;

  {
    h5::file file{"test_map.h5", 'r'};
    h5::group grp{file};
    h5_read(grp, "map_int", mm);
    h5_read(grp, "map_vec", mmv);
  }

  // compare
  EXPECT_EQ(m, mm);
  EXPECT_EQ(mv, mmv);
}

TEST(H5Io, Vector) {

  // write
  std::vector<std::string> m = {"a", "b"};
  std::vector<double> mv     = {1.0, 2.0};

  {
    h5::file file{"test_vec.h5", 'w'};
    h5::group grp{file};
    h5_write(grp, "vec_str", m);
    h5_write(grp, "vec_dbl", mv);
  }

  // read
  std::vector<std::string> mm;
  std::vector<double> mmv;

  {
    h5::file file{"test_vec.h5", 'r'};
    h5::group grp{file};
    h5_read(grp, "vec_str", mm);
    h5_read(grp, "vec_dbl", mmv);
  }

  // compare
  EXPECT_EQ(m, mm);
  EXPECT_EQ(mv, mmv);
}

TEST(H5Io, Attribute) {
  // write
  std::vector<std::vector<std::string>> vvs  = {{"a", "b"}, {"c", "d"}, {"e", "f"}};
  std::vector<std::vector<std::string>> evvs = {};
  std::vector<std::vector<std::string>> vevs = {{}, {}, {}};
  std::vector<std::vector<std::string>> vves = {{"", ""}, {"", ""}, {"", ""}};

  {
    h5::file file{"test_attribute.h5", 'w'};
    h5::group grp{file};
    h5_write(grp, "vec", 0);

    auto id = grp.open_dataset("vec");
    h5_write_attribute(id, "attr_vvs", vvs);
    h5_write_attribute(id, "attr_evvs", evvs);
    h5_write_attribute(id, "attr_vevs", vevs);
    h5_write_attribute(id, "attr_vves", vves);
  }

  // read
  std::vector<std::vector<std::string>> rvvs, revvs, rvevs, rvves;

  {
    h5::file file{"test_attribute.h5", 'r'};
    h5::group grp{file};

    auto id = grp.open_dataset("vec");
    h5_read_attribute(id, "attr_vvs", rvvs);
    h5_read_attribute(id, "attr_evvs", revvs);
    h5_read_attribute(id, "attr_vevs", rvevs);
    h5_read_attribute(id, "attr_vves", rvves);
  }

  // compare
  EXPECT_EQ(vvs, rvvs);
  EXPECT_EQ(evvs, revvs);
  EXPECT_EQ(vevs, rvevs);
  EXPECT_EQ(vves, rvves);
}

MAKE_MAIN;
