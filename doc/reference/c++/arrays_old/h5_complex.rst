.. highlight:: c

Issue with the complex numbers
============================================================================

There is no standard representation of the complex numbers `std::complex<T>` in hdf5.

The classes therefore represent them as arrays of `T` with one more dimension.

* This is compatible with the ALPS 2.0 storage.
* This is **not** compatible with the regular h5py storage for complex. 
  The conversion is made by HDFArchive class automatically.


