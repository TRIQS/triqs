..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf_meshLTimfreqGT__get_tail_fitter:

triqs::gfs::gf_mesh<imfreq>::get_tail_fitter
============================================

*#include <triqs/gfs/tail_fitter_handle.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :ref:`tail_fitter <triqs__gfs__tail_fitter>` & :red:`get_tail_fitter` () const

    2. | :ref:`tail_fitter <triqs__gfs__tail_fitter>` & :red:`get_tail_fitter` (double :param:`tail_fraction`,
       |                  int :param:`n_tail_max` = tail_fitter::default_n_tail_max,
       |                  std::optional<int> :param:`expansion_order` = {}) const

Documentation



 **1)**   The tail fitter is mutable, even if the mesh is immutable to cache some data



 **2)**   Adjust the parameters for the tail-fitting and return the fitter