..
   Generated automatically by cpp2rst

.. highlight:: c


.. _trace_rho_op:

trace_rho_op
============

**Synopsis**:

.. code-block:: c

    template<bool Complex>
    typename atom_diag<Complex>::scalar_t trace_rho_op (const typename
   atom_diag<Complex>::block_matrix_t & density_matrix, const typename
   atom_diag<Complex>::many_body_op_t & op, const atom_diag<Complex> & atom)

Compute Tr (op * density_matrix)


Parameters
----------

 * **density_matrix**: Density matrix as a list of diagonal blocks for all invariant subspaces in `atom`.

 * **op**: Operator to be averaged.

 * **atom**: Solved diagonalization problem.



Return value
------------

Operator `op` averaged over the density matrix.


Documentation
-------------