#include <Python.h>
#include <iostream>
#include <triqs/gfs/local/tail.hpp>
int main() {
 int N1 = 1, N2 = 1;
 triqs::gfs::local::tail t(N1, N2);
 t.mask_view() = 5; // only coeffs from -1 to 5 are meaningful
 std::cout << t(0) << std::endl;
 t(2) = .5;
 std::cout << t << std::endl;
}

