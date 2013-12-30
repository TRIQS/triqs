.. highlight:: c

.. _gf_retime: 

Real time
===================================================

This is a specialisation of :ref:`gf_and_view` for imaginary Matsubara frequencies.
 


Synopsis
------------

.. code::

  gf<retime, Target, Opt>

The *Target* template parameter can take the following values : 
 
+-------------------------+-----------------------------------------------------+
| Target                  | Meaning                                             |
+=========================+=====================================================+
| scalar_valued           | The function is scalar valued (double, complex...). |
+-------------------------+-----------------------------------------------------+
| matrix_valued [default] | The function is matrix valued.                      |
+-------------------------+-----------------------------------------------------+

Domain & mesh
----------------

CORRECT THIS !! PUT OUT THE C++ impl for doxy doc

The domain is :doxy:`matsubara_freq_domain<triqs::gfs::matsubara_domain>`.

The mesh is :doxy:`matsubara_freq_mesh<triqs::gfs::matsubara_freq_mesh>`.


Singularity
-------------

The singularity is a high frequency expansion,  :ref:`gf_tail`.


Evaluation method
---------------------

* Linear interpolation on the mesh.

* Return type : 

  * If Target==scalar_valued : a complex 
  * If Target==matrix_valued : an object modeling ImmutableMatrix concept.

* When the point is outside of the mesh, the evaluation of the gf returns : 

  * the evaluation of the high frequency tail if no_tail is not set.
  * 0 otherwise


Data storage
---------------

* If Target==scalar_valued :
  
  * `data_t` : 1d array of complex<double>.

  * g.data()(i) is the value of g for the i-th point of the mesh.

* If Target==matrix_valued :

  * `data_t` : 3d array (C ordered) of complex<double>.

  * g.data()(i, range(), range()) is the value of g for the i-th point of the mesh.

HDF5 storage convention
---------------------------

h5 tag : `ReTime`

Examples
---------

.. compileblock:: 

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs;
    
    int main() {
      double tmin=0, tmax=10;    // the time interval
      auto n_times=100;          // we will have 100 points
      
      //we want a 2x2 matrix-valued Green function
      auto g=gf<retime>{ {tmin, tmax, n_times} , {2,2} };  
    };

