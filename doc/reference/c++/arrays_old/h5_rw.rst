.. highlight:: c

Simple read/write operations of an array (or a view)
============================================================================

Given an array (or an array_view), the functions `h5::write` and `h5::read` write and read it to/from the file
or any subgroup thereof. For example : 

.. literalinclude:: examples_code/h5_rw.cpp

Note that : 
    
   * The data in the hdf5 file are stored in C order.

   * The data is reordered in Fortran (or custom) order if necessary when reading/writing.

   * It also works with matrix and vector (but the storage is the same in HDF5).
    
   * It also works with the corresponding views.  TO BE ILLUSTRATED.

