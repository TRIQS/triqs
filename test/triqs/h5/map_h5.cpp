/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
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
#include <map>
#include <triqs/h5.hpp>

namespace h5 = triqs::h5;

int main(int argc, char **argv) {

 try { 

  // write
  std::map<std::string, int> m = { {"a",1}, {"b",2} };
  std::map<std::string, std::vector<double>> mv = { {"a",{1.0, 2.0}}, {"b",{2.0, 3.0, 4.0}} };

  H5::H5File file1("test_map.h5", H5F_ACC_TRUNC);
  h5::group top1(file1);
  h5_write(top1, "map_int", m);
  h5_write(top1, "map_vec", mv);

  // read
  std::map<std::string, int> mm = { {"c",1} };
  std::map<std::string, std::vector<double>> mmv = { {"c",{1.0}} };

  H5::H5File file2("test_map.h5", H5F_ACC_RDONLY);
  h5::group top2(file2);
  h5_read(top2, "map_int", mm);
  h5_read(top2, "map_vec", mmv);

  for (auto const & val: mm) std::cout << val.first << " " << val.second << std::endl;
  for (auto const & val: mmv) {
    std::cout << val.first << std::endl;
    for (auto const & x: val.second) std::cout << x << std::endl;
  }

 }
 TRIQS_CATCH_AND_ABORT;
}

