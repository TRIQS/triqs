..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _space_partition_foreach:

triqs::hilbert_space::space_partition::foreach
==============================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Apply a callable object to all basis Fock states in a given space partition`
       | :green:`template<typename Lambda>`
       | void :red:`foreach` (space_partition<StateType, OperatorType> & :param:`SP`, Lambda :param:`L`)






  The callable must take two arguments, 1) index of the basis Fock state in the considered full Hilbert space,
  and 2) index of the subspace this basis state belongs to.





Template parameters
^^^^^^^^^^^^^^^^^^^

 * **Lambda**: Type of the callable object


Parameters
^^^^^^^^^^

 * **SP**: Subject space partition

 * **L**: Callable object