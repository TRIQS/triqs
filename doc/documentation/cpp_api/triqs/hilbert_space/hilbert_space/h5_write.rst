..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _hilbert_space_h5_write:

triqs::hilbert_space::hilbert_space::h5_write
=============================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Write a Hilbert space to an HDF5 group`
       | void :red:`h5_write` (h5::group :param:`fg`, std::string const & :param:`name`, :ref:`hilbert_space <triqs__hilbert_space__hilbert_space>` const & :param:`hs`)







Parameters
^^^^^^^^^^

 * **fg**: Parent HDF5 group to write the space to

 * **name**: Name of the HDF5 subgroup to be created

 * **hs**: Hilbert space to be written