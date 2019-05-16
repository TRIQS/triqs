..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__hilbert_space__imperative_operator__operator():

triqs::hilbert_space::imperative_operator::operator()
=====================================================

*#include <triqs/hilbert_space/imperative_operator.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | :green:`template<typename StateType, typename Args>`
       | StateType :red:`operator()` (StateType const & :param:`st`, Args &&... :param:`args`) const





Act on a state and return a new state

  The optional extra arguments `args...` are forwarded to the coefficients of the operator.

  `auto psi = op(phi,args...);`

  We apply an operator obtained from `op` by replacing its monomial coefficients with values
  returned by `coeff(args...)`. This feature makes sense only for ScalarType being a callable object.





Template parameters
^^^^^^^^^^^^^^^^^^^

 * :param:`StateType` Type of the initial state

 * :param:`Args` Types of the optional arguments


Parameters
^^^^^^^^^^

 * :param:`st` Initial state

 * :param:`args` Optional argument pack passed to each coefficient of the operator