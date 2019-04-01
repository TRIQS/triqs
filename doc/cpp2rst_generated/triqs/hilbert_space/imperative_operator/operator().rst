..
   Generated automatically by cpp2rst

.. highlight:: c


.. _imperative_operator_operator():

operator()
==========

**Synopsis**:

.. code-block:: c

    template<typename StateType, typename Args>
    StateType operator() (const StateType & st, Args &&... args)

Act on a state and return a new state


Template parameters
-------------------

 * **StateType**: Type of the initial state

 * **Args**: Types of the optional arguments



Parameters
----------

 * **st**: Initial state

 * **args**: Optional argument pack passed to each coefficient of the operator


Documentation
-------------


   The optional extra arguments `args...` are forwarded to the coefficients of the operator.

   `auto psi = op(phi,args...);`

   We apply an operator obtained from `op` by replacing its monomial coefficients with values
   returned by `coeff(args...)`. This feature makes sense only for ScalarType being a callable object.