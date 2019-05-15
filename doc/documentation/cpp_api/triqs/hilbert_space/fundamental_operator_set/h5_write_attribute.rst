..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _fundamental_operator_set_h5_write_attribute:

triqs::hilbert_space::fundamental_operator_set::h5_write_attribute
==================================================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Write this set as an HDF5 attribute`
       | void :red:`h5_write_attribute` (hid_t :param:`id`,
       |   std::string const & :param:`name`,
       |   :ref:`fundamental_operator_set <triqs__hilbert_space__fundamental_operator_set>` const & :param:`f`)







Parameters
^^^^^^^^^^

 * **id**: ID of an HDF5 object to attach the attribute to

 * **name**: Name of the attribute

 * **f**: Fundamental set to write