.. highlight:: c

.. _arr_play_concept:

.. warning::

   This part is alpha : work in progress. May change at any time.

Using basic concepts
###########################################


make_immutable_array
-------------------------------

**Synopsis**::

  template<typename Expr, int ... ph> auto make_immutable_array(Expr const &, clef::pair<ph,range> ...);  (1)
  template<typename Function>         auto make_immutable_array(Function, range ...);                     (2)

* (1) 
  From a clef expression, and a range for each placeholder in the expression, build
  a lazy object modelling :ref:`ImmutableCuboidArray` concept, with the domain built from the ranges : 
  
  Parameters are :

    * expr The lazy expression
    * i_=R  : `i_` is a placeholder, R a range. The `i_=R` produce a clef::pair of `i_` and R , which is the parameter.

* (2) 
  From a function object and a set of range, build 
  a lazy object modelling :ref:`ImmutableCuboidArray` concept, with the domain built from the ranges : 


**Example** : 

.. compileblock::

   #include <triqs/arrays.hpp>
   #include <triqs/arrays/make_immutable_array.hpp>
   using triqs::arrays::array; using triqs::arrays::range; using triqs::clef::placeholder;
   int main() {  
    placeholder<0> i_; placeholder<1> j_;

    auto a =  make_immutable_array( 1.0/(2 + i_ + j_), i_= range(0,2), j_=range(0,2));
   
    std::cout << "a = " << a                  << std::endl;
    std::cout << "a = " << array<double,2>(a) << std::endl;
    
    // or if you prefer using a lambda...
    auto b = make_immutable_array ( [](int i, int j) { return i-j;}, range (0,2), range(0,2));

    std::cout << "b = " << b                  << std::endl;
    std::cout << "b = " << array<double,2>(b) << std::endl;

   }



