
/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2018 by N. Wentzell
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

#include <triqs/test_tools/arrays.hpp>
#include <triqs/test_tools/gfs.hpp>

#include <triqs/lattice/tight_binding.hpp>

#include <vector>

using namespace triqs::gfs;
using namespace triqs::lattice;
using namespace triqs::arrays;

TEST(tight_binding, h5_read_write) {
  double t             = 1.0;
  auto bl              = bravais_lattice{{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};
  auto displ_vec       = std::vector<std::vector<long>>{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};
  auto overlap_mat_vec = std::vector<matrix<dcomplex>>{{{t}}, {{t}}, {{t}}, {{t}}, {{ t}}, { { t }} };

  auto tb = tight_binding{bl, displ_vec, overlap_mat_vec};

  // write
  {
    auto file = triqs::h5::file{"test_tb.h5", H5F_ACC_TRUNC};
    auto grp  = triqs::h5::group{file};
    h5_write(grp, "cubic_lattice_nn_hop", tb);
  }

  // read
  {
    auto file  = triqs::h5::file{"test_tb.h5", H5F_ACC_RDONLY};
    auto grp   = triqs::h5::group{file};
    auto tb_in = triqs::h5::h5_read<tight_binding>(grp, "cubic_lattice_nn_hop");
    //EXPECT_EQ(tb, tb_in);
  }
}

MAKE_MAIN;
