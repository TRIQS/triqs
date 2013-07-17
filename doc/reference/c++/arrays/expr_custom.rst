.. highlight:: c

.. _custom_expression: 

Custom Expressions
-------------------------------------------------

   - Building custom expressions. The transpose example...
      
      Describe the concept, what to implement, etc....
        
        - Transpose:: 

           array<long,2> A (2,2), B(2,2),C(2,2);
           C= A + 2*B;
           C = A + Transpose(B);         // Transpose(X) returns a lazy object that models HasImmutableArrayInterface. 
           C = A + Transpose(B + B);     // X can also be an expression...
           C = Transpose(B);             //
          
           // non square
           array<long,2> R(2,3),Rt(3,2);
           cout<<" R = "<< array<long,2>(Transpose(R)) <<endl;
          
           // mapping any function 
           C =  map_expr(&sqr,A);
           cout<<" C = "<< map_expr(&sqr,A,"SQR")<<" = "<<C<<endl;
          
           // matmul as expression Oi are 'C' or 'F'
           matrix<double,O1> M1(2,2); matrix<double,O2> M2(2,2); matrix<double,O3> M3;
           // The central instruction : note that matmul returns a lazy object 
           // that has ImmutableArray interface, and defines a specialized version assignment
           // As a result this is equivalent to some matmul_with_lapack(M1,M2,M3) : there is NO intermediate copy.
           M3 = matmul(M1,M2);
    

     See expression.hpp.
     At the moment, only +,-, * and / by scalar are implemented.
     
     An expression models HasImmutableArrayInterface, i.e. :
      
     * It has a domain (computed from the expression)
     * It can be evaluated.
     
     It is then easy to mix them with other objects,
     that model the same concept. See e.g. expr2.cpp (map_expr) for examples.
     
     
     * *Multiplication* : 
      not decided, since it is not the same for array or matrices.
      Two choices : 

      * Do not add \* for array, matrices (use matmul e.g.) and allow mixing array, matrix
        e.g. add an array<int,2> and a matrix <int>

      * Add the \*, but then do different expression for array and matrix/vector,
        then one can not mix them.
        In that case, it is however trivial to say e.g. M + matrix_view<int>(A) if A is an array.
     
     
