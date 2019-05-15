..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _gf_meshLTimfreqGT_get_tail_fitter:

triqs::gfs::gf_mesh::get_tail_fitter
====================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`The tail fitter is mutable, even if the mesh is immutable to cache some data`
       | :ref:`tail_fitter <triqs__gfs__tail_fitter>` & :red:`get_tail_fitter` () const

    2. | :cppbrief:`Adjust the parameters for the tail-fitting and return the fitter`
       | :ref:`tail_fitter <triqs__gfs__tail_fitter>` & :red:`get_tail_fitter` (double :param:`tail_fraction`,
       |   int :param:`n_tail_max` = tail_fitter::default_n_tail_max,
       |   std::optional<int> :param:`expansion_order` = {}) const

Documentation