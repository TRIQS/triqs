.. highlight:: c

.. _Mapped:

Simple functions on arrays
==================================

The following functions are mapped on the array (using :ref:`map`):

`abs  real  imag  pow  cos  sin  tan  cosh  sinh  tanh  acos  asin  atan  exp  log  sqrt  floor`
 
meaning that e.g. if `A` is an array,
real(A) models :ref:`ImmutableCuboidArray`, with the same size and returns the real part of the elements.
In other words, it applies the function term-by-term.

.. note:: 

  These functions do NOT compute a new array in memory, they are lazy.

Example : 

.. compileblock::

    #include <triqs/arrays.hpp>
    using triqs::arrays::matrix; using triqs::clef::placeholder;
 
    int main(){
       placeholder<0> i_; placeholder<1> j_;
       matrix<double> A(2,2);  
       A(i_,j_) <<  i_ - j_ ;

       std::cout << "A = "<<A << std::endl;
       std::cout << "abs(A)= "<<matrix<double>(abs(A))<< std::endl;
    }




  


