..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _stateLTHilbertSpace_ScalarType_trueGT_foreach:

triqs::hilbert_space::state::foreach
====================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Apply a callable object to **non-vanishing** amplitudes of a state`
       | :green:`template<typename Lambda>`
       | void :red:`foreach` (:ref:`state\<type-parameter-0-0, type-parameter-0-1, true\> <triqs__hilbert_space__stateLTHilbertSpace_ScalarType_trueGT>` const & :param:`st`, Lambda :param:`l`)






  The callable must take two arguments, 1) index of the basis Fock state in the associated Hilbert space, and 2) the corresponding amplitude.





Template parameters
^^^^^^^^^^^^^^^^^^^

 * **Lambda**: Type of the callable object


Parameters
^^^^^^^^^^

 * **st**: State object

 * **l**: Callable object