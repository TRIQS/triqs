#include <triqs/h5/variant.hpp>
#include <triqs/test_tools/arrays.hpp>

using namespace triqs;

TEST(H5, Variant) {

  using v_t = std::variant<int, std::string>;
  auto s    = std::string{"Hello"};
  {
    auto v1 = v_t{6};
    auto v2 = v_t{s};

    h5::file file("variant.h5", 'w');
    h5_write(file, "v1", v1);
    h5_write(file, "v2", v2);
  }

  {
    v_t v1, v2{std::string{}};
    h5::file file("variant.h5", 'r');
    h5_read(file, "v1", v1);

    h5_read(file, "v2", v2);

    EXPECT_EQ(std::get<int>(v1), 6);
    //EXPECT_EQ(std::get<std::string>(v2), s);
  }
}
MAKE_MAIN;
