..
   Generated automatically by cpp2rst

.. highlight:: c


.. _partition_function:

partition_function
==================

**Synopsis**:

.. code-block:: c

    template<bool Complex>
    double partition_function (const atom_diag<Complex> & atom, double beta)

The atomic partition function


Template parameters
-------------------

 * **Complex**: Do we have a diagonalization problem with a complex-valued Hamiltonian?



Parameters
----------

 * **atom**: Solved diagonalization problem.

 * **beta**: Inverse temperature.



Return value
------------

Value of the partition function.


Documentation
-------------