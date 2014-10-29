#include <triqs/gfs.hpp>
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> " << (X) << std::endl << std::endl;

template<typename T>
void assert_equal(T const& x, T const& y, std::string mess="") {
 if (std::abs(x - y) > 1.e-13) TRIQS_RUNTIME_ERROR << mess;
}

template<typename T1, typename T2>
void assert_equal_array(T1 const& x, T2 const& y, std::string mess="") {
 if (max_element(abs(x - y)) > 1.e-13) TRIQS_RUNTIME_ERROR << mess << "\n" << x << "\n" << y << "\n" << max_element(abs(x - y));
}

template<typename T>
void assert_zero_array(T const& x, std::string mess="") {
 if (max_element(abs(x)) > 1.e-13) TRIQS_RUNTIME_ERROR << mess << "\n" << x << "\n" << max_element(abs(x));
}


