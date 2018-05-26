#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/arrays.hpp>
using triqs::arrays::array;
int main() {
  try {
    array<double, 2> A(2, 2);
    A() = 3;
    std::cout << A(0, 3) << std::endl;
  }
  // catch (triqs::arrays::key_error & e) { std::cout<< e.what()<< std::endl;}
  catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  } // or like this : triqs::arrays::key_error derives from std::exception
}
