..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__tail_fitter__setup_lss:

triqs::gfs::tail_fitter::setup_lss
==================================

*#include <triqs/gfs/tail_fitter.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<bool enforce_hermiticity, typename M>`
       | void :red:`setup_lss` (M const & :param:`m`, int :param:`n_fixed_moments`)





Set up the least-squares solver for a given number of known moments.