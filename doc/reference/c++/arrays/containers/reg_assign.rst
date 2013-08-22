.. highlight:: c

.. _arr_reg_assign:

Assignment 
=========================

The `value classes` and the `view classes` have a quite general assignment operator.
We will illustrate it on the `array` class, it is the same for `matrix` and `vector`.

* **Syntax** : the syntax is the same in both cases::

    template<typename RHS> array      & operator=(const RHS & X);
    template<typename RHS> array_view & operator=(const RHS & X);

* **What can be RHS ?**

  RHS can be  ... anything that models the :ref:`ImmutableCuboidArray` concept !
  
  e.g. : array, array_view, matrix, matrix_view, 
  but also formal expression (See , e.g. A+B), or any custom object of your choice.

* **Behaviour**

   ================= ======================================================================= ======================================================================================
   Topic             value class : array, matrix, vector                                     view: array_view, matrix_view, vector_view   
   ================= ======================================================================= ======================================================================================
   RHS ...           - RHS models the concept :ref:`ImmutableCuboidArray`.                  - RHS models :ref:`ImmutableCuboidArray` 
                                                                                               [or less ? : RHS can be evaluated in the domain_type::value_type, no domain needed.].
   Effect            - array is resized to have a domain X.domain()                          - View's domain must match X's domain or behaviour is undefined.
                     - array is filled with the evaluation of X.                             - array is filled with the evaluation of X.  
   ================= ======================================================================= ======================================================================================

   * By evaluation of X, we mean : 

      - a copy if X is an array.
      - computing the value if X is an expression.               

Compound operators (+=, -=, * =, /=)
-------------------------------------------------

* **Syntax** 

   The syntax is natural ::

    template<typename RHS> array & operator += (const RHS & X);
    template<typename RHS> array & operator -= (const RHS & X);
    template<typename RHS> array & operator *= (const Scalar & S);
    template<typename RHS> array & operator /= (const Scalar & S);

* **What can be RHS ?**

   Same as for = operator.

* **Behaviour**

   Similar to  for = operator, with obvious changes.

 
