#include <triqs/gfs.hpp>
#include "./common.hpp"

using namespace triqs::gfs;

int main() {

 using A = triqs::arrays::matrix_tensor_proxy<triqs::arrays::array<std::complex<double>, 3, void> &, true>;
 
 static_assert(std::is_constructible<std::complex<double>, matsubara_freq>::value, "oops");
 static_assert(triqs::arrays::is_scalar_or_convertible<matsubara_freq>::value, "oops2");
 static_assert(triqs::arrays::is_scalar_for<std::complex<double>, A>::value, "oops2");
 static_assert(triqs::arrays::is_scalar_for<matsubara_freq, A>::value, "oops2");

 triqs::clef::placeholder<0> om_;
 auto g = gf<imfreq>{{10, Fermion, 10}, {2, 2}};
 auto g2 = g;
 auto g3 = g;

 g(om_) << om_ + 0.0; // Works
 
 g2(om_) << om_;       // Did not compile : rhs = mesh_point
 g3(om_) << om_ + om_; // Did not compile : rhs = matsubara_freq

 assert_equal_array(g.data(), g2.data(), "bug !");
 assert_equal_array(g.data(), g3.data()/2, "bug !");

 std::cerr  << g.data()(triqs::arrays::ellipsis(), 0,0) << std::endl;
 std::cerr  << g.data()(triqs::arrays::ellipsis(), 1,0) << std::endl;
 std::cerr  << g.data()(triqs::arrays::ellipsis(), 0,1) << std::endl;
 std::cerr  << g.data()(triqs::arrays::ellipsis(), 1,1) << std::endl;
 
 std::cerr  << g2.data()(triqs::arrays::ellipsis(), 0,0) << std::endl;
 std::cerr  << g2.data()(triqs::arrays::ellipsis(), 1,0) << std::endl;
 std::cerr  << g2.data()(triqs::arrays::ellipsis(), 0,1) << std::endl;
 std::cerr  << g2.data()(triqs::arrays::ellipsis(), 1,1) << std::endl;
 
 std::cerr  << g3.data()(triqs::arrays::ellipsis(), 0,0) << std::endl;
 std::cerr  << g3.data()(triqs::arrays::ellipsis(), 1,0) << std::endl;
 std::cerr  << g3.data()(triqs::arrays::ellipsis(), 0,1) << std::endl;
 std::cerr  << g3.data()(triqs::arrays::ellipsis(), 1,1) << std::endl;
 
 return 0;
}
