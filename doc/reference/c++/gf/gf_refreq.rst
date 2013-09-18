.. highlight:: c

.. _gf_refreq: 

gf<refreq> 
===================================================

This is a specialisation of :ref:`gf_and_view` for imaginary Matsubara frequencies.
 
Domain & mesh
----------------


Singularity
-------------

Interpolation method
---------------------

Linear interpolation on the mesh.

Data storage
---------------

* `data_t` : 3d array (C ordered) of complex<double>.

* g.data()(i, range(), range()) is the value of g for the i-th point of the mesh.

HDF5 storage convention
---------------------------

h5 tag : `ReFreq`

Examples
---------

.. compileblock:: 

    #include <triqs/gfs/refreq.hpp>
    using namespace triqs::gfs;

    int main() {
      double wmin=0, wmax=10;  // Frequencies interval
      auto n_freq=100;         // Number of points

      // We want a 2x2 matrix valued function on this mesh...
      auto g = gf<refreq> { {wmin, wmax, n_freq}, {2,2} };
    };


