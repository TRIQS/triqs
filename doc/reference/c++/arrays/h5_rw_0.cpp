#include <triqs/arrays.hpp>
using triqs::arrays::array;
using triqs::arrays::matrix;
int main() {

 array<double, 2> A(2, 2);
 A() = 3; // declare and init

 H5::H5File file("store_A.h5", H5F_ACC_TRUNC); // open the file
 h5_write(file, "A", A);                       // write the array as 'A' into the file

 // array<double,2> B;                        // read the file into B
 matrix<double> B; // read the file into B
 h5_read(file, "A", B);
 std::cout << "B = " << B << std::endl;
 h5_write(file, "B", B);
}

