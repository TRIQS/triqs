.. highlight:: c

.. _arr_comp_ops:

Compound assignment operators (+=, -=, * =, /=)
-------------------------------------------------

**Synopsis** ::

    template<typename RHS> THIS_TYPE & operator += (const RHS & X);
    template<typename RHS> THIS_TYPE & operator -= (const RHS & X);
    template<typename RHS> THIS_TYPE & operator *= (const Scalar & S);
    template<typename RHS> THIS_TYPE & operator /= (const Scalar & S);

The containers and the view have compound operators.

