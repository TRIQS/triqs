#include <triqs/stat/mean_error.hpp>
#include <mpi/mpi.hpp>

using namespace triqs::stat;

int main(int argc, char *argv[]) {
  mpi::environment env(argc, argv);
  mpi::communicator world;
  int rank = world.rank();
  // Define linear data spread over the different mpi threads:
  // thread 0: {1,2,3,4}; thread 1 {5,6,7,8}, etc.
  std::vector<double> data{4. * rank + 1, 4. * rank + 2, 4. * rank + 3, 4. * rank + 4};

  auto ave = mean_mpi(world, data);

  std::cout << "Average: " << ave << std::endl; 
  // =  (1.0 + 4.0 * world.size()) / 2.0
}
