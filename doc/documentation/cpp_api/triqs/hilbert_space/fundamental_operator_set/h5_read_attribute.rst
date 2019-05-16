..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__hilbert_space__fundamental_operator_set__h5_read_attribute:

triqs::hilbert_space::fundamental_operator_set::h5_read_attribute
=================================================================

*#include <triqs/hilbert_space.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | void :red:`h5_read_attribute` (hid_t :param:`id`,
       |                    std::string const & :param:`name`,
       |                    :ref:`fundamental_operator_set <triqs__hilbert_space__fundamental_operator_set>` & :param:`f`)





Read a set from an HDF5 attribute





Parameters
^^^^^^^^^^

 * :param:`id` ID of an HDF5 object the attribute is attached to

 * :param:`name` Name of the attribute

 * :param:`f` Reference to a fundamental set to be read