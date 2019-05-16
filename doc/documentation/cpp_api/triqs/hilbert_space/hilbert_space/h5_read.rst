..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__hilbert_space__hilbert_space__h5_read:

triqs::hilbert_space::hilbert_space::h5_read
============================================

*#include <triqs/hilbert_space.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | void :red:`h5_read` (h5::group :param:`fg`, std::string const & :param:`name`, :ref:`hilbert_space <triqs__hilbert_space__hilbert_space>` & :param:`hs`)





Read a Hilbert space from an HDF5 group





Parameters
^^^^^^^^^^

 * :param:`fg` Parent HDF5 group to read the space from

 * :param:`name` Name of the HDF5 subgroup to be read

 * :param:`hs` Reference to a target Hilbert space object