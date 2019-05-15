..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _partition_function:

partition_function
==================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`The atomic partition function`
       | :green:`template<bool Complex>`
       | double :red:`partition_function` (:ref:`atom_diag\<Complex\> <triqs__atom_diag__atom_diag>` const & :param:`atom`, double :param:`beta`)







Template parameters
^^^^^^^^^^^^^^^^^^^

 * **Complex**: Do we have a diagonalization problem with a complex-valued Hamiltonian?


Parameters
^^^^^^^^^^

 * **atom**: Solved diagonalization problem.

 * **beta**: Inverse temperature.


Returns
^^^^^^^

 * Value of the partition function.