..
   Generated automatically by cpp2rst

.. highlight:: c


.. _quantum_number_eigenvalues:

quantum_number_eigenvalues
==========================

**Synopsis**:

.. code-block:: c

    template<bool Complex>
    std::vector<std::vector<quantum_number_t> > quantum_number_eigenvalues (const
   typename atom_diag<Complex>::many_body_op_t & op, const atom_diag<Complex> & atom)

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