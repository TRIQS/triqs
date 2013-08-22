.. highlight:: c

.. _gf_retime: 

gf<retime> 
===================================================

This is a specialisation of :ref:`gf_and_view` for imaginary Matsubara frequencies.
 
Domain & mesh
----------------


Singularity
-------------

Factories
-------------

Code ::

  make_gf(double tmin, double tmax, size_t n_points, tqa::mini_vector<size_t,2> shape)

  make_gf(double tmin, double tmax, size_t n_points, tqa::mini_vector<size_t,2> shape, mesh_kind mk)



Interpolation method
---------------------

Data storage
---------------


HDF5 storage convention
---------------------------



Examples
---------

.. compileblock:: 

    #include <triqs/gfs/refreq.hpp>
    using namespace triqs::gfs;

    int main() {
      double wmin=0;
      double wmax=10;
      size_t n_freq=5;
      //we want a Green function whose values are 2*2 matrices of complex numbers
      auto shape = triqs::arrays::make_shape(2,2);
      auto GF=make_gf<refreq>(wmin, wmax, n_freq, shape);
    };


