..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _move_h5_write:

triqs::mc_tools::move::h5_write
===============================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`redirect the h5 call to the object lambda, if it not empty (i.e. if the underlying object can be called with h5_read/write`
       | void :red:`h5_write` (h5::group :param:`g`, std::string const & :param:`name`, move<MCSignType> const & :param:`m`)