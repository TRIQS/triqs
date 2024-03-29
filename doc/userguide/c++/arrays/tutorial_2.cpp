#include <nda/nda.hpp>
#include <nda/h5.hpp>
#include <h5/h5.hpp>

using nda::array;
int main() {

  array<double, 2> A(2, 2);
  A() = 3; // declare and init

  h5::file file("store_A.h5", 'w'); // open the file
  h5::write(file, "A", A);          // write the array as 'A' into the file

  array<double, 2> B; // read the file into B
  h5::read(file, "A", B);
  std::cout << "B = " << B << std::endl;
}
