.. highlight:: c

Compound assignment operators : +=, -=, *=, /=
=============================================================

The `value classes` and the `view classes` have a quite general assignment operator (=), 
and corresponding compound operators (+=, -=).
We will illustrate it on the `array` class, it is the same for `matrix` and `vector`.

Assignment operator (=)
-------------------------------------------------

The `value classes` and the `view classes` have a quite general assignment operator.

* **Syntax** : the syntax is the same in both cases::

    template<typename RHS> array      & operator=(const RHS & X);
    template<typename RHS> array_view & operator=(const RHS & X);

* **What can be RHS ?**

  RHS can be  ... anything that models the HasImmutableArrayInterface concept !
  
  e.g. : array, array_view, matrix, matrix_view, 
  but also formal expression (See , e.g. A+B), or any custom object of your choice.

* **Behaviour**

   ================= ======================================================================= ======================================================================================
   Topic             value class : array, matrix, vector                                     view: array_view, matrix_view, vector_view   
   ================= ======================================================================= ======================================================================================
   RHS ...           - RHS models the concept `HasImmutableArrayInterface`.                  - RHS models HasImmutableArrayInterface 
                                                                                               [or less ? : RHS can be evaluated in the domain_type::value_type, no domain needed.].
   Effect            - array is resized to have a domain X.domain()                          - View's domain must match X's domain or behaviour is undefined.
                     - array is filled with the evaluation of X.                             - array is filled with the evaluation of X.  
   ================= ======================================================================= ======================================================================================

   * By evaluation of X, we mean : 

      - a copy if X is an array.
      - computing the value if X is an expression.               

* **Optimisation** `[advanced]`

  In some case, we know that optimisation is possible.
  If the RHS class derives from `Tag::has_special_assign`, then the instruction ::
 
    A = rhs;

  will be rewritten by compiler as ::

    rhs.assign_invoke(A);
 
  An example is the matmul class, which stores the ref. to the objects to be multiplied.
  If A and B are matrices, writing ::

    C = matmul(A,B); // (1)

  is rewritten as ::
 
    matmul(A,B).assign_invoke(C); 
 
  which is then itself rewritten into (without all the messy details) ::

    call_lapack( A, B, C); // (2)

  The point is that : 

  * the user code is (1), very simple and readable
  * but the compiler compiles (2), which eliminates temporaries...


Compound operators (+=, -=, * =, /=)
-------------------------------------------------


 * **Syntax** 

   The syntax is natural ::

    template<typename RHS> array & operator += (const RHS & X);
    template<typename RHS> array & operator -= (const RHS & X);
    template<typename RHS> array & operator *= (const Scalar & S);
    template<typename RHS> array & operator /= (const Scalar & S);

 * **Behaviour**

   - Domain must match.
   - X is evaluated and added term by term.

To be written : special operators.
