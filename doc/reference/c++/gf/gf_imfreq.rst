.. highlight:: c

.. _gf_imfreq: 

gf<imfreq, matrix_valued> & gf<imfreq, scalar_valued>
==========================================================

This is a specialisation of :ref:`gf_and_view` for imaginary Matsubara frequencies.
 
Domain & mesh
----------------


Singularity
-------------

:ref:`gf_tail`.

Factories
-------------


The factories are  ::

  make_gf(mesh<imfreq,Opt> m,            matrix_shape_t shape,                     local::tail_view t = local::tail(shape) )
  make_gf(double beta, statistic_enum S, matrix_shape_t shape, size_t Nmax = 1025, local::tail_view t = local::tail(shape) )


Interpolation method
---------------------

None

Data storage
---------------

* `data_t` : 3d array (C ordered) of complex<double>.

* g.data()(i, range(), range()) is the value of g for the i-th point of the mesh.

HDF5 storage convention
---------------------------

h5 tag : `ImFreq`


Examples
---------


.. compileblock:: 

    #include <triqs/gfs/imfreq.hpp>
    using namespace triqs::gfs;
    int main() {
      double beta=1;   // inverse temperature
      size_t n_freq=5; // we will have 5 points including iw=0 and iw=beta
      
      auto GF = make_gf<imfreq>(beta, Fermion, make_shape(1,1), n_freq);  
    };


An alternative declaration with an explicit construction of the underlying mesh:

.. compileblock:: 

 
    #include <triqs/gfs/imfreq.hpp>
    using namespace triqs::gfs;
    int main(){
     double beta=10;
     int Nfreq =100;
     
     auto GF  = make_gf<imfreq>(gf_mesh<imfreq>{beta,Fermion,Nfreq}, make_shape(1,1), local::tail(1,1));
     // or even simpler 
     auto GF2 = make_gf<imfreq>({beta,Fermion,Nfreq}, make_shape(1,1), local::tail(1,1));
    }



