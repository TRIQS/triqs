..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _imperative_operator_operator():

triqs::hilbert_space::imperative_operator::operator()
=====================================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Act on a state and return a new state`
       | :green:`template<typename StateType, typename Args>`
       | StateType :red:`operator()` (StateType const & :param:`st`, Args &&... :param:`args`) const






   The optional extra arguments `args...` are forwarded to the coefficients of the operator.

   `auto psi = op(phi,args...);`

   We apply an operator obtained from `op` by replacing its monomial coefficients with values
   returned by `coeff(args...)`. This feature makes sense only for ScalarType being a callable object.





Template parameters
^^^^^^^^^^^^^^^^^^^

 * **StateType**: Type of the initial state

 * **Args**: Types of the optional arguments


Parameters
^^^^^^^^^^

 * **st**: Initial state

 * **args**: Optional argument pack passed to each coefficient of the operator