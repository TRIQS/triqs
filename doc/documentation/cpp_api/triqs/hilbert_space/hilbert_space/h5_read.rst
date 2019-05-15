..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _hilbert_space_h5_read:

triqs::hilbert_space::hilbert_space::h5_read
============================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Read a Hilbert space from an HDF5 group`
       | void :red:`h5_read` (h5::group :param:`fg`, std::string const & :param:`name`, :ref:`hilbert_space <triqs__hilbert_space__hilbert_space>` & :param:`hs`)







Parameters
^^^^^^^^^^

 * **fg**: Parent HDF5 group to read the space from

 * **name**: Name of the HDF5 subgroup to be read

 * **hs**: Reference to a target Hilbert space object