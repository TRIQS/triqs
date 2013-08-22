.. highlight:: c

.. _gf_refreq: 

gf<refreq> 
===================================================

This is a specialisation of :ref:`gf_and_view` for imaginary Matsubara frequencies.
 
Domain & mesh
----------------


Singularity
-------------

Factories
-------------

code ::

  make_gf(MeshType && m, tqa::mini_vector<size_t,2> shape, local::tail_view const & t)
  make_gf(double wmin, double wmax, size_t n_freq, tqa::mini_vector<size_t,2> shape)
  make_gf(double wmin, double wmax, size_t n_freq, tqa::mini_vector<size_t,2> shape, mesh_kind mk)




Interpolation method
---------------------

Data storage
---------------


HDF5 storage convention
---------------------------



Examples
---------


.. compileblock:: 

    #include <triqs/gfs/retime.hpp>
    using namespace triqs::gfs;
    
    int main() {
      double tmin=0;
      double tmax=10;
      //we will have 5 points
      size_t n_times=5;
      //we want a Green function whose values are complex numbers
      auto shape = triqs::arrays::make_shape(1,1);
      // the type of GF is triqs::gfs::gf<triqs::gfs::retime>
      auto GF=make_gf<retime>(tmin, tmax, n_times, shape);  
    };

