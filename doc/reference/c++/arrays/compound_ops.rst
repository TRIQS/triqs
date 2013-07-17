.. highlight:: c

Compound operators (+=, -=, *=, /=)
======================================

The `value classes` and the `view classes` behaves similarly.
We will illustrate it on the `array` class, it is the same for `matrix` and `vector`.

 * **Syntax** 

   The syntax is natural ::

    template<typename RHS> array & operator += (const RHS & X);
    template<typename RHS> array & operator -= (const RHS & X);
    template<typename RHS> array & operator *= (const Scalar & S);
    template<typename RHS> array & operator /= (const Scalar & S);

 * **Behaviour**

   - Domain must match.
   - X is evaluated and added term by term.
