..
   Generated automatically by cpp2rst

.. highlight:: c


.. _quantum_number_eigenvalues_checked:

quantum_number_eigenvalues_checked
==================================

**Synopsis**:

.. code-block:: c

    template<bool Complex>
    std::vector<std::vector<quantum_number_t> > quantum_number_eigenvalues_checked
   (const typename atom_diag<Complex>::many_body_op_t & op, const atom_diag<Complex> &
   atom)

Compute values of a given quantum number for all eigenstates


Parameters
----------

 * **op**: Observable operator; supposed to be a quantum number (if not -> exception).

 * **atom**: Solved diagonalization problem.



Return value
------------

The eigenvalues by block


Documentation
-------------


 This function is similar to :ref:`quantum_number_eigenvalues()` except it checks that
 the quantum number operator is diagonal in the eigenbasis.