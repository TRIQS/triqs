#include <triqs/arrays.hpp>
using triqs::arrays::array;
using triqs::arrays::ellipsis;
int main() {
  array<long, 4> B(2, 3, 4, 5);
  B(0, ellipsis(), 3);    // same as  B(0, range(),range(), 3 )
  B(0, ellipsis(), 2, 3); // same as  B(0, range(), 2, 3 )
  B(ellipsis(), 2, 3);    // same as  B( range(),range(),  2, 3 )
}
