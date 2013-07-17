.. highlight:: c

.. _ElementAccess:

Element access 
==================================

The elements of the arrays can be naturally accessed using the ( ) operator. 

  If A is an array<T,R> and i,j,k, its indices
    
  A(i,j,k) is the element (i,j,k) of the array.

.. compileblock:: 

    #include <triqs/arrays.hpp>
    namespace tqa=triqs::arrays;
    int main(){
       tqa::array<double,2> A(2,3);
       A() = 0;     //  assign 0 to a complete view of A.
       A(0,0) = 5;  
       A(1,1) = 2* A(0,0); 
       std::cout <<"A = "<< A << std::endl;
    }


  
