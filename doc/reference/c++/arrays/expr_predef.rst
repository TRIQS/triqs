.. highlight:: c

.. _predef_expression: 

Predefined Expressions
-------------------------------------------------

   - Using predefined expressions : 
      
       - matmul
 
         The code::
          
          matrix<double> M1(2,2), M2(2,2), M3;
          M3 = matmul(M1,M2);
         
         will do the following : 
           -  matmul returns a lazy object modelling the :ref:`Expression` concept.
           -  a result this is compiled as some `matmul_with_lapack(M1,M2,M3)` : **there is NO intermediate copy**.

        - mat_vec_mul

