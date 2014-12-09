#include <triqs/arrays/array.hpp>
#include <triqs/arrays/h5/simple_read_write.hpp>
namespace tqa=triqs::arrays; namespace h5=tqa::h5;

int main() { 

 tqa::array<long,2> A(2,2),B; A() = 3;    // declare some array and init it

 h5::H5File file("ess.h5",H5F_ACC_TRUNC); // open the file ess.h5
 h5::write(file,"A",A);                   // write A in the file as 'A' 

 file.createGroup("G");                   // create a subgroup G (Cf hdf5 doc, C++ API)
 h5::write(file,"G/A",A);                 // write A as G/A

 h5::Group G = file.openGroup("G");       // another way to do it : open the group and write G/A2
 h5::write(G, "A2",A);

 h5::read (file, "A",B);                  // Read from the file. NB : B is automatically resized
}

