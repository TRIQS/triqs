#include "start.hpp"

placeholder<0> i_;
placeholder<1> j_;
placeholder<2> k_;
placeholder<3> l_;

TEST(Array, c_ordered_transposed_view1) {

  array<int, 4> a(1, 2, 3, 4, make_memory_layout(0, 1, 2, 3));
  a(i_, j_, k_, l_) << i_ + 10 * j_ + 100 * k_ + 1000 * l_;

  auto v = c_ordered_transposed_view(a());

  EXPECT_EQ(a.shape(), (mini_vector<size_t, 4>{1, 2, 3, 4}));
  EXPECT_EQ(v.shape(), (mini_vector<size_t, 4>{1, 2, 3, 4}));
  EXPECT_EQ(a.indexmap().memory_layout(), (memory_layout_t<4>{0, 1, 2, 3}));
  EXPECT_EQ(v.indexmap().memory_layout(), (memory_layout_t<4>{0, 1, 2, 3}));
}

TEST(Array, c_ordered_transposed_view2) {

  array<int, 4> a(1, 2, 3, 4, make_memory_layout(1, 0, 2, 3));
  a(i_, j_, k_, l_) << i_ + 10 * j_ + 100 * k_ + 1000 * l_;

  auto v = c_ordered_transposed_view(a());

  EXPECT_EQ(a.shape(), (mini_vector<size_t, 4>{1, 2, 3, 4}));
  EXPECT_EQ(v.shape(), (mini_vector<size_t, 4>{2, 1, 3, 4}));
  EXPECT_EQ(a.indexmap().memory_layout(), (memory_layout_t<4>{1, 0, 2, 3}));
  EXPECT_EQ(v.indexmap().memory_layout(), (memory_layout_t<4>{0, 1, 2, 3}));
}

TEST(Array, c_ordered_transposed_view3) {

  array<int, 4> a(1, 2, 3, 4, make_memory_layout(2, 0, 3, 1));
  a(i_, j_, k_, l_) << i_ + 10 * j_ + 100 * k_ + 1000 * l_;

  auto v = c_ordered_transposed_view(a());

  EXPECT_EQ(a.shape(), (mini_vector<size_t, 4>{1, 2, 3, 4}));
  EXPECT_EQ(v.shape(), (mini_vector<size_t, 4>{3, 1, 4, 2}));
  EXPECT_EQ(a.indexmap().memory_layout(), (memory_layout_t<4>{2, 0, 3, 1}));
  EXPECT_EQ(v.indexmap().memory_layout(), (memory_layout_t<4>{0, 1, 2, 3}));
}

MAKE_MAIN;
