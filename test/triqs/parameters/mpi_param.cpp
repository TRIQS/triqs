#include <triqs/parameters.hpp>
#include <iostream>

#include <vector>
#include <complex>
#include <string>

#include <boost/mpi.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/serialization/complex.hpp>

using namespace triqs::utility;

int main(int argc, char* argv[]) {
 boost::mpi::environment env(argc, argv);
 boost::mpi::communicator world;

 parameters P;

 if (world.rank() == 0) {   
  P["a"] = long(1);
  P["d"] = 2.7;
  P["s"] = std::string("-14.3");
 }

 std::cout << "Before bcast rank " << world.rank() << " : "  << P << std::endl ;
 boost::mpi::broadcast(world,P,0);
 std::cout << "After bcast rank " << world.rank() << " : "  << P << std::endl ;

 return 0;
}








