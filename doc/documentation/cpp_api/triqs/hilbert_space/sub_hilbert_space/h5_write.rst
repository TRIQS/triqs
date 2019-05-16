..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__hilbert_space__sub_hilbert_space__h5_write:

triqs::hilbert_space::sub_hilbert_space::h5_write
=================================================

*#include <triqs/hilbert_space.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | void :red:`h5_write` (h5::group :param:`fg`,
       |           std::string const & :param:`name`,
       |           :ref:`sub_hilbert_space <triqs__hilbert_space__sub_hilbert_space>` const & :param:`hs`)





Write a Hilbert subspace to an HDF5 group





Parameters
^^^^^^^^^^

 * :param:`fg` Parent HDF5 group to write the space to

 * :param:`name` Name of the HDF5 subgroup to be created

 * :param:`hs` Hilbert subspace to be written