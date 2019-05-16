..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__atom_diag__partition_function:

triqs::atom_diag::partition_function
====================================

*#include <triqs/atom_diag/functions.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<bool Complex>`
       | double :red:`partition_function` (:ref:`atom_diag\<Complex\> <triqs__atom_diag__atom_diag>` const & :param:`atom`, double :param:`beta`)





The atomic partition function





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`Complex` Do we have a diagonalization problem with a complex-valued Hamiltonian?


Parameters
^^^^^^^^^^

 * :param:`atom` Solved diagonalization problem.

 * :param:`beta` Inverse temperature.


Returns
^^^^^^^

Value of the partition function.