.. highlight:: c

.. _gf_legendre: 

Legendre representation
==========================================================

This is a specialisation of :ref:`gf<gf_and_view>` for Legendre polynomial expansion.

Synopsis
------------

.. code::

  gf<legendre, Target, Opt>

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

TO BE WRITTEN

Singularity
-------------

None


Evaluation method
---------------------

TO BE WRITTEN

Data storage
---------------

TO BE WRITTEN

HDF5 storage convention
---------------------------

h5 tag : `Legendre`

Examples
---------

.. compileblock:: 

    #include <triqs/gfs.hpp>
    using namespace triqs::gfs;

    int main() {
      // We want a 2x2 matrix valued function on this mesh...
      //auto g = gf<legendre> { {wmin, wmax, n_freq}, {2,2} };
    };


