..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__hilbert_space__space_partition__foreach:

triqs::hilbert_space::space_partition::foreach
==============================================

*#include <triqs/hilbert_space.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename Lambda>`
       | void :red:`foreach` (space_partition<StateType, OperatorType> & :param:`SP`, Lambda :param:`L`)





Apply a callable object to all basis Fock states in a given space partition

 The callable must take two arguments, 1) index of the basis Fock state in the considered full Hilbert space,
 and 2) index of the subspace this basis state belongs to.





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`Lambda` Type of the callable object


Parameters
^^^^^^^^^^

 * :param:`SP` Subject space partition

 * :param:`L` Callable object