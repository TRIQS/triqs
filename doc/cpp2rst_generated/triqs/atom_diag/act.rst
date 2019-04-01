..
   Generated automatically by cpp2rst

.. highlight:: c


.. _act:

act
===

**Synopsis**:

.. code-block:: c

    template<bool Complex>
    typename atom_diag<Complex>::full_hilbert_space_state_t act (const typename
   atom_diag<Complex>::many_body_op_t & op, const typename
   atom_diag<Complex>::full_hilbert_space_state_t & st, const atom_diag<Complex> & atom)

Act with operator `op` on state `st`


Parameters
----------

 * **op**: Operator to act on the state.

 * **st**: Initial state vector in the full Hilbert space, written in the eigenbasis of the Hamiltonian.

 * **atom**: Solved diagonalization problem.



Return value
------------

Final state vector in the full Hilbert space.


Documentation
-------------