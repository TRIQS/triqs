..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _quantum_number_eigenvalues_checked:

quantum_number_eigenvalues_checked
==================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Compute values of a given quantum number for all eigenstates`
       | :green:`template<bool Complex>`
       | std::vector<std::vector<quantum_number_t> > :red:`quantum_number_eigenvalues_checked` (typename atom_diag<Complex>::many_body_op_t const & :param:`op`,
       |   :ref:`atom_diag\<Complex\> <triqs__atom_diag__atom_diag>` const & :param:`atom`)






 This function is similar to :ref:`quantum_number_eigenvalues()` except it checks that
 the quantum number operator is diagonal in the eigenbasis.





Parameters
^^^^^^^^^^

 * **op**: Observable operator; supposed to be a quantum number (if not -> exception).

 * **atom**: Solved diagonalization problem.


Returns
^^^^^^^

 * The eigenvalues by block