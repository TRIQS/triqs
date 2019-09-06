#include <mpi/mpi.hpp>
#include <gtest/gtest.h>

using namespace mpi;

// a simple structure
struct Complex {
  double real, imag;

  Complex operator+(Complex y) const {
    y.real += real;
    y.imag += imag;
    return y;
  }
};

// tie the data
inline auto tie_data(Complex c) { return std::tie(c.real, c.imag); }

// The mpi custom type is implemented using a tie
namespace mpi {
  template <> struct mpi_type<Complex> : mpi_type_from_tie<Complex> {};
} // namespace mpi

// a user defined function
Complex fff(Complex const &x, Complex const &y) { return x + y; }

//---------------------------------------

TEST(MPI_CUSTOM, custom_type_op) {

  communicator world;
  int rank = world.rank();
  int size = world.size();

  Complex a[10], answer[10];

  for (int u = 0; u < 10; ++u) {
    a[u].real      = rank + 1;
    a[u].imag      = 0;
    answer[u].real = 0;
    answer[u].imag = 0;
  }

  int root = 0;

  MPI_Reduce(a, answer, 10, mpi::mpi_type<Complex>::get(), mpi::map_add<Complex>(), root, MPI_COMM_WORLD);

  if (rank == root)
    for (int u = 0; u < 10; ++u) { ASSERT_NEAR(answer[u].real, size * (size + 1) / 2, 1.e-14); }
}

//---------------------------------------

TEST(MPI_CUSTOM, custom_type_op2) {

  communicator world;
  int rank = world.rank();
  int size = world.size();

  Complex a[10], answer[10];

  for (int u = 0; u < 10; ++u) {
    a[u].real      = rank + 1;
    a[u].imag      = 0;
    answer[u].real = 0;
    answer[u].imag = 0;
  }

  int root = 0;

  MPI_Reduce(a, answer, 10, mpi::mpi_type<Complex>::get(), mpi::map_C_function<Complex, fff>(), root, MPI_COMM_WORLD);

  if (rank == root)
    for (int u = 0; u < 10; ++u) { ASSERT_NEAR(answer[u].real, size * (size + 1) / 2, 1.e-14); }
}

//---------------------------------------

// A struct with elementwise addition on first and last component
struct A {
  int i;
  long long ll;
  double d;
};
A add_As(A const &lhs, A const &rhs) {
  return A{lhs.i + rhs.i, 0ll, lhs.d + rhs.d};
}

// Tie the data to construct the custom MPI type
inline auto tie_data(A a) { return std::tie(a.i, a.ll, a.d); }
namespace mpi {
  template <> struct mpi_type<A> : mpi_type_from_tie<A> {};
} // namespace mpi

TEST(MPI_CUSTOM, struct_custom_add) {

  communicator world;
  int rank = world.rank();
  int size = world.size();

  A a[10], answer[10];

  for (int u = 0; u < 10; ++u) {
    a[u].i = rank + 1;
    a[u].d = 3.0 * (rank + 1);
  }

  int root = 0;

  MPI_Reduce(a, answer, 2, mpi_type<A>::get(), mpi::map_C_function<A, add_As>(), root, MPI_COMM_WORLD);

  if (rank == root)
    for (int u = 0; u < 2; ++u) {
    ASSERT_NEAR(answer[u].i + answer[u].d, 2 * size * (size + 1), 1.e-14);
    }
}

MPI_TEST_MAIN;
