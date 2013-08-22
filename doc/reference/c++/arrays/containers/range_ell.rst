.. highlight:: c

.. _arr_range:

range
=========

  `range` mimics the Python `range`. Arguments of the constructor can be : 
  
  * no argument : it then takes the whole set of indices in the dimension (like `:` in python) ::    
    
     A(range(), 0) // take the first column of A

  * two arguments to specify a range ::

     A(range (0,3), 0)  // means  A(0,0), A(1,0), A(2,0)

    .. warning::
       the second element is excluded : range(0,3) is 0,1,2, like in Python.

  * three arguments  : a range with a step ::

      A(range(0,4,2), 0) // means A(0,0), A(2,0)  
   

.. _arr_ellipsis:

ellipsis
===============

* Ellipsis can be provided in place of `range`, as in python. The type `ellipsis` is similar to range
  except that it is implicitely repeated to as much as necessary.

* Example:

  .. compileblock ::

     #include <triqs/arrays.hpp>
     using triqs::arrays::array; using triqs::arrays::ellipsis; 
     int main(){
      array<long,4> B(2,3,4,5) ;
      B(0,ellipsis(),3)  ; // same as  B(0, range(),range(), 3 ) 
      B(0,ellipsis(),2,3); // same as  B(0, range(), 2, 3 ) 
      B(ellipsis(),2,3)  ; // same as  B( range(),range(),  2, 3 ) 
     }


* NB : there can be at most one ellipsis per expression (otherwise it would be meaningless).

* Example of usage : 
 
  Ellipsis are useful to write generic algorithms. For example, imagine that you want to sum 
  arrays on their first index :

  .. compileblock ::
        
      #include <triqs/arrays.hpp>
      using triqs::arrays::array; using triqs::arrays::ellipsis; 
      
      // a generic function that sum array, array_view or in fact anything 
      // with the right concept on its first dimension
      template<typename ArrayType>
       array<typename ArrayType::value_type, ArrayType::rank-1> sum0 (ArrayType const & A) {
       array<typename ArrayType::value_type, ArrayType::rank-1> res = A(0,ellipsis());
       for (size_t u =1; u< first_dim(A); ++u) res += A(u,ellipsis());
       return res;
       }

      // test
      int main(){
        array<double,2> A(5,2); A()=2;
        array<double,3> B(5,2,3); B() = 1;
        std::cout<< sum0(A) << sum0(B) <<std::endl;
      }


