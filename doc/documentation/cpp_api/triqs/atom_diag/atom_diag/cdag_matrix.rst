..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__atom_diag__atom_diag__cdag_matrix:

triqs::atom_diag::atom_diag::cdag_matrix
========================================

*#include <triqs/atom_diag/atom_diag.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | atom_diag::matrix_t const & :red:`cdag_matrix` (int :param:`op_linear_index`, int :param:`sp_index`) const





Matrix block for fundamental operator :math:`C^\dagger`





Parameters
^^^^^^^^^^

 * :param:`op_linear_index` The linear index (i.e. number) of the creation operator, as defined by the fundamental operator set.

 * :param:`sp_index` The index of the initial subspace.


Returns
^^^^^^^

The index of the final subspace.