.. highlight:: c

to be removed...
-------------------------------------------------

* **Definition** :
  
  By `expression`, we mean here an object, typically representing a mathematical expression, 
  which models the :ref:`HasImmutableArrayInterface` concept.

* **Use** :
  
  Expression can be used : 
   - as RHS (Right Hand Side) of various operators (=, +=, -=, ...)
   - at any place where an `expression` is expected.

* **How to make expression ?**

  Expressions can be build easily in various ways : 

   - Using :ref:`arith_expression`, e.g. A + 2*B
   - Using predefined expressions, e.g. for Linear Algebra operations.
     For example, in the code::
          
          matrix<double> M1(2,2), M2(2,2), M3;
          M3 = matmul(M1,M2);
         
      `matmul` returns an expression, a lazy object modelling the :ref:`Expression` concept.
      REF : 

   - Building :ref:`custom_expression`.








.. toctree::
   :maxdepth: 1

   expr_arith
   expr_predef
   expr_custom



