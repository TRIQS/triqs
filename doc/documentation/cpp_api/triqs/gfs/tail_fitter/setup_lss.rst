..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _tail_fitter_setup_lss:

triqs::gfs::tail_fitter::setup_lss
==================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Set up the least-squares solver for a given number of known moments.`
       | :green:`template<bool enforce_hermiticity, typename M>`
       | void :red:`setup_lss` (M const & :param:`m`, int :param:`n_fixed_moments`)