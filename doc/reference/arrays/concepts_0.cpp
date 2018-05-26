#include <triqs/arrays.hpp>
#include <iostream>
namespace triqs {
  namespace arrays { // better to put it in this namespace for ADL...

    template <typename T> class immutable_diagonal_matrix_view {

      array_view<T, 1> data; // the diagonal stored as a 1d array

      public:
      immutable_diagonal_matrix_view(array_view<T, 1> v) : data(v) {} // constructor

      // the ImmutableMatrix concept
      typedef indexmaps::cuboid::domain_t<2> domain_type;
      domain_type domain() const {
        auto s = data.shape()[0];
        return domain_type{s, s};
      }
      typedef T value_type;
      T operator()(size_t i, size_t j) const { return (i == j ? data(i) : 0); } // just kronecker...

      friend std::ostream &operator<<(std::ostream &out, immutable_diagonal_matrix_view const &d) { return out << "diagonal_matrix " << d.data; }
    };

    // Marking this class as belonging to the Matrix & Vector algebra.
    template <typename T> struct ImmutableMatrix<immutable_diagonal_matrix_view<T>> : std::true_type {};
  } // namespace arrays
} // namespace triqs

/// TESTING
using namespace triqs::arrays;
int main(int argc, char **argv) {
  auto a = array<int, 1>{1, 2, 3, 4};
  auto d = immutable_diagonal_matrix_view<int>{a};
  std::cout << "domain = " << d.domain() << std::endl;
  std::cout << "d   = " << d << std::endl;
  std::cout << "2*d = " << make_matrix(2 * d) << std::endl;
  std::cout << "d*d = " << matrix<int>(d * d) << std::endl;
}
