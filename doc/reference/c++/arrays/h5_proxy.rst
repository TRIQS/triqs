.. highlight:: c

h5::array_proxy : a simple proxy to the array in file
===========================================================

.. warning::

   NOT operational any more ...  : development needed

The principle is that `array_proxy` is a proxy to an array, possibly big, in the h5 file.
It has a domain, and can be assigned from/to an array and sliced.
This allows to read/write only parts of a large array using the same slicing syntax 
as for the array class itself (which is then translated into the hyperslab technique of the HDF5 library).


Tutorial
-----------

* Write a "big" array in the h5 file and write it slice by slice::

   #include <triqs/arrays/h5/array_proxy.hpp>
   using namespace triqs::arrays;

   h5::H5File file( "ess.h5",  H5F_ACC_TRUNC );

   // first create a 'big' array in the file (no temporary is made in memory, it uses directly the HDF5 API).
   const size_t N = 100;
   h5::array_proxy<T,3,3> Pn( file, "Z", make_shape(N,2,2) );

   // a slice...
   array<double,2> the_slice (2,2); the_slice() = 1;

   // write the large array slice by slice 
   for (int u = 0; u<N; ++u)  P (u,range(), range())  = u* the_slice; // or any array expression 

* Read a "big" array slice by slice::

   #include <triqs/arrays/h5/array_proxy.hpp>
   using namespace triqs::arrays;

   h5::H5File file ("ess.h5", H5F_ACC_RDONLY );
   h5::array_proxy<T,3,3> P(file, "A");

   array<T,2> a_slice ;
   for (int u = 0; u<N; ++u)  a_slice =  P (u,range(), range()) ;

Reference 
------------

You can also get the :doxy:`full C++ documentation<triqs::arrays::h5::array_proxy>` for this class.

