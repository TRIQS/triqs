..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__statistics__histogram__h5_read:

triqs::statistics::histogram::h5_read
=====================================

*#include <triqs/statistics.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | void :red:`h5_read` (h5::group :param:`g`, std::string const & :param:`name`, :ref:`histogram <triqs__statistics__histogram>` & :param:`h`)





Read histogram from HDF5





Parameters
^^^^^^^^^^

 * :param:`g` Enclosing HDF5 group

 * :param:`name` Dataset name with histogram data

 * :param:`h` Histogram to read data into