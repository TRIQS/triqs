.. highlight:: c

.. _gf_refreq: 

Real frequencies
==========================================================

This is a specialisation of :ref:`gf<gf_and_view>` for real frequencies.


Synopsis
------------

.. code::

  gf<refreq, Target, Opt>

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

h5 tag : `ReFreq`

Examples
---------

.. compileblock:: 

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs;

    int main() {
      double wmin=0, wmax=10;  // Frequencies interval
      auto n_freq=100;         // Number of points

      // We want a 2x2 matrix valued function on this mesh...
      auto g = gf<refreq> { {wmin, wmax, n_freq}, {2,2} };
    };


