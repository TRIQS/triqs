..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__mc_tools__move__h5_write:

triqs::mc_tools::move::h5_write
===============================

*#include <triqs/mc_tools.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | void :red:`h5_write` (h5::group :param:`g`, std::string const & :param:`name`, move<MCSignType> const & :param:`m`)





redirect the h5 call to the object lambda, if it not empty (i.e. if the underlying object can be called with h5_read/write