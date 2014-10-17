.. highlight:: c

Simple read/write operations of an array (or a view)
============================================================================

Given an array (or an array_view), the functions `h5::write` and `h5::read` write and read it to/from the file
or any subgroup thereof. For example : 

.. compileblock::

  #include <triqs/arrays.hpp>
  using triqs::arrays::array; using triqs::arrays::matrix; 
  int main(){

   array<double,2> A(2,2); A() = 3;          // declare and init

   H5::H5File file("store_A.h5",H5F_ACC_TRUNC);   // open the file
   h5_write(file,"A",A);                         // write the array as 'A' into the file

   //array<double,2> B;                        // read the file into B
   matrix<double> B;                        // read the file into B
   h5_read (file, "A",B);
   std::cout << "B = "<<B<<std::endl;
   h5_write(file,"B",B); 
  }

.. compileblock:: 

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



examples_code/h5_rw.cpp

Note that : 
    
   * The data in the hdf5 file are stored in C order.

   * The data is reordered in Fortran (or custom) order if necessary when reading/writing.

   * It also works with matrix and vector (but the storage is the same in HDF5).
    
   * It also works with the corresponding views.  TO BE ILLUSTRATED.

