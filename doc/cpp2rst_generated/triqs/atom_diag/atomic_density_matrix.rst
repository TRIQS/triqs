..
   Generated automatically by cpp2rst

.. highlight:: c


.. _atomic_density_matrix:

atomic_density_matrix
=====================

**Synopsis**:

.. code-block:: c

    template<bool Complex>
    typename atom_diag<Complex>::block_matrix_t atomic_density_matrix (const
   atom_diag<Complex> & atom, double beta)

The atomic density matrix


Template parameters
-------------------

 * **Complex**: Do we have a diagonalization problem with a complex-valued Hamiltonian?



Parameters
----------

 * **atom**: Solved diagonalization problem.

 * **beta**: Inverse temperature.



Return value
------------

Gibbs' density matrix of the system.


Documentation
-------------