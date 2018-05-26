/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#include "start.hpp"

namespace triqs {
  namespace arrays {

    template <typename T> class immutable_diagonal_matrix_view : TRIQS_CONCEPT_TAG_NAME(ImmutableMatrix) {
      array_view<T, 1> data;

      public:
      immutable_diagonal_matrix_view(array_view<T, 1> v) : data(v) {}

      // the ImmutableMatrix concept
      using value_type = T;
      T operator()(size_t i, size_t j) const { return (i == j ? data(i) : 0); }

      using domain_type = indexmaps::cuboid::domain_t<2>;
      domain_type domain() const {
        auto s = data.shape()[0];
        return mini_vector<size_t, 2>(s, s);
      }

      //
      friend std::ostream &operator<<(std::ostream &out, immutable_diagonal_matrix_view const &d) { return out << "diagonal_matrix " << d.data; }
    };
  } // namespace arrays
} // namespace triqs

TEST(Array, DiagonalMatrix) {

  auto a = array<int, 1>{1, 2, 3, 4};
  auto d = immutable_diagonal_matrix_view<int>(a);

  EXPECT_ARRAY_NEAR(matrix<int>(2 * d), matrix<double>{{2, 0, 0, 0}, {0, 4, 0, 0}, {0, 0, 6, 0}, {0, 0, 0, 8}});
  EXPECT_ARRAY_NEAR(matrix<int>(d * d), matrix<double>{{1, 0, 0, 0}, {0, 4, 0, 0}, {0, 0, 9, 0}, {0, 0, 0, 16}});

  int sum = 0;
  foreach (d, [&](int i, int j) { sum += d(i, j); })
    ;
  EXPECT_EQ(sum, 10);
}
MAKE_MAIN
