..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__atom_diag__atom_diag__get_quantum_numbers:

triqs::atom_diag::atom_diag::get_quantum_numbers
================================================

*#include <triqs/atom_diag/atom_diag.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | std::vector<std::vector<quantum_number_t> > const & :red:`get_quantum_numbers` () const





A vector of all the quantum numbers, grouped by subspace





Returns
^^^^^^^

result[sp_index][qn_index] is the qunatum number value.