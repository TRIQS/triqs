..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__real:

triqs::gfs::real
================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename G>`
       | std::enable_if_t<is_gf<G>::value, typename G::regular_type::real_t> :red:`real` (G const & :param:`g`)





Takes the real part of g without check, and returns a new gf with a real target
real(g).





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`G` any Gf, BlockGf or Block2Gf type


Parameters
^^^^^^^^^^

 * :param:`g` a gf