..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _atom_diag_c_matrix:

triqs::atom_diag::atom_diag::c_matrix
=====================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Matrix block for fundamental operator :math:`C``
       | atom_diag::matrix_t const & :red:`c_matrix` (int :param:`op_linear_index`, int :param:`sp_index`) const







Parameters
^^^^^^^^^^

 * **op_linear_index**: The linear index (i.e. number) of the annihilation operator, as defined by the fundamental operator set.

 * **sp_index**: The index of the initial subspace.


Returns
^^^^^^^

 * The index of the final subspace.