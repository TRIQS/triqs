// automatically included in e.g. arrays, gfs, any triqs library...
#include <triqs/utility/exceptions.hpp>

int main() {
  try {
    if (2 != 3) TRIQS_RUNTIME_ERROR << " The condition is false because " << 2 << "!=" << 3;
  } catch (...) {}
}
