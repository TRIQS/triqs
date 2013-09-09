.. highlight:: c

.. _Lazy:

Interaction with CLEF expressions 
============================================

* The containers and their view classes can be used with the :doc:`../clef/contents` library : 

  * They can be called with CLEF expressions.
  * :doc:`Automatic assignment<../clef/assign>` has been set up.

* Using the CLEF library offers a quick and efficient way to fill an array with multiple advantages : 

  * It is simpler and more readeable than a series of for loops.
  * It is usually more optimal since the for loops are automatically written in the TraversalOrder of the array.


* **Example** : 

.. compileblock::

    #include <triqs/arrays.hpp>
    using triqs::arrays::array; using triqs::clef::placeholder;
    int main(){
       placeholder<0> i_; placeholder<1> j_;
       array<double,2> A(2,2), B(2,2);  
       
       A(i_,j_) <<  i_ + 2*j_ ;
       B(i_,j_) <<  A(j_,i_)/2; 
       
       std::cout << "A = "<<A << std::endl;
       std::cout << "B = "<<B << std::endl;
    }

.. note:: 
   The syntax uses a <<, not = since the array is not assigned to an expression
   but filled by the evaluation thereof.



  

