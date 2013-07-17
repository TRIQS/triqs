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

.. literalinclude:: examples_code/h5_rw.cpp

Note that : 
    
   * The data in the hdf5 file are stored in C order.

   * The data is reordered in Fortran (or custom) order if necessary when reading/writing.

   * It also works with matrix and vector (but the storage is the same in HDF5).
    
   * It also works with the corresponding views.  TO BE ILLUSTRATED.

