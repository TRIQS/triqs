#include <mpi/mpi.hpp>
#include <gtest.h>

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

TEST(MPI_CUSTOM, tuple_op) {

  communicator world;
  int rank = world.rank();
  int size = world.size();

  // Tuple with elementwise addition on first and last component
  using tpl_type = std::tuple<int, long long, double>;
  auto add_tpls = [](tpl_type const & lhs, tpl_type const & rhs){ return std::make_tuple(std::get<0>(lhs) + std::get<0>(rhs), 0ll, std::get<2>(lhs) + std::get<2>(rhs)); };

  tpl_type a[10], answer[10];

  for (int u = 0; u < 10; ++u) {
    std::get<0>(a[u])= rank + 1;
    std::get<2>(a[u])= 3.0 * (rank + 1);
  }

  int root = 0;

  MPI_Reduce(a, answer, 10, mpi_type<tpl_type>::get(), mpi::map_C_function<tpl_type, add_tpls>(), root, MPI_COMM_WORLD);

  if (rank == root)
    for (int u = 0; u < 10; ++u) { ASSERT_NEAR(std::get<0>(answer[u]) + std::get<2>(answer[u]), 2 * size * (size + 1), 1.e-14); }
}

MPI_TEST_MAIN;
