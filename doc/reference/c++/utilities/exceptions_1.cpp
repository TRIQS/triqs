// automatically included in e.g. arrays, gfs, any triqs library...
#include <triqs/utility/exceptions.hpp>
#include <iostream>

void f() {
 try {
  if (2 != 3) TRIQS_RUNTIME_ERROR << " The condition is false because " << 2 << "!=" << 3;
 }
 catch (triqs::runtime_error const& e) {
  std::cout << "caught error " << e.what() << std::endl;
 }
}

int main() { f(); }

