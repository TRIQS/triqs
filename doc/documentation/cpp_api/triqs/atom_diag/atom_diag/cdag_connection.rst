..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _atom_diag_cdag_connection:

triqs::atom_diag::atom_diag::cdag_connection
============================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Subspace-to-subspace connections for fundamental operator :math:`C^\dagger``
       | long :red:`cdag_connection` (int :param:`op_linear_index`, int :param:`sp_index`) const







Parameters
^^^^^^^^^^

 * **op_linear_index**: The linear index (i.e. number) of the creation operator, as defined by the fundamental operator set.

 * **sp_index**: The index of the initial subspace.


Returns
^^^^^^^

 * The index of the final subspace.