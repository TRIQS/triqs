..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__hilbert_space__stateLTHilbertSpace_ScalarType_trueGT__foreach:

triqs::hilbert_space::state<HilbertSpace,ScalarType,true>::foreach
==================================================================

*#include <triqs/hilbert_space.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename Lambda>`
       | void :red:`foreach` (:ref:`state\<type-parameter-0-0, type-parameter-0-1, true\> <triqs__hilbert_space__stateLTHilbertSpace_ScalarType_trueGT>` const & :param:`st`, Lambda :param:`l`)





Apply a callable object to **non-vanishing** amplitudes of a state

 The callable must take two arguments, 1) index of the basis Fock state in the associated Hilbert space, and 2) the corresponding amplitude.





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`Lambda` Type of the callable object


Parameters
^^^^^^^^^^

 * :param:`st` State object

 * :param:`l` Callable object