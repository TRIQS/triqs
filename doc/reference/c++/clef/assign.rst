.. highlight:: c

Automatic assignment 
=======================

Principle
----------------

It is possible to use a subset of possible expressions as Left Hand Side (LHS) in an assignment statement, e.g. ::

 A(x_)        = some_expression_of_x_
 A[i_]        = some_expression_of_i_
 A(x_)(i_,j_) = some_expression_of_x_i_j
 A[x_](i_,j_) = some_expression_of_x_i_j

* Right Hand Side (RHS) of the = statement can be any expression.
* Left Hand Side (LHS) of the = sign. A must be a `lazy-assignable`, called by [] or (), one or several time.

This writing means that a regular function ::
  
  x_ -> some_expression_of_x_

will be given to A so that it can fill itself by evaluating this function.
A determines the range of value of x on which the function is called.

Example
---------

.. compileblock::

  #include "triqs/clef.hpp"
  #include "triqs/clef/adapters/vector.hpp"
  #include <iostream> 

  int main() { 
  int N = 10;
  double pi = std::acos(-1);
  std::vector<double> V(N);

  // automatic assignment of vector and use of lazy math function
  triqs::clef::placeholder <0> k_; 
  triqs::clef::lazy(V) [k_] << cos( (2* pi* k_)/ N );

  // check result... 
  for (size_t u=0; u<V.size(); ++u) std::cout<< u << " "<<V[u]<< "  "<< cos((2*pi*u)/N)<<std::endl;
  }

Implementing automatic assign for an object 
------------------------------------------------------------

It is sufficient to add a function ::

   template<typename Fnt> void triqs_clef_auto_assign (Obj & x, Fnt f);

The compiler will rewrite ::

   obj(x_,y_, ...) = expression

into ::

   triqs_clef_auto_assign (obj, make_function( expression, x_, y_, ....)) 

The function is found by ADL. It is therefore useful to implement it e.g. as a friend function.

Similarly, for [ ], adding a function ::

   template<typename Fnt> void triqs_clef_auto_assign_subscript (Obj & x, Fnt f);

The compiler will rewrite ::

   obj[i_] = expression

into ::

   triqs_clef_auto_assign_subscript (obj, make_function( expression, i_)) 



A complete example : 

.. compileblock::

    #include <triqs/clef.hpp>
    #include <iostream>

    struct Obj{ 
    double v; 
    Obj(double v_): v(v_){}

    // lazy call : cf ...
    template< typename... Args>
    typename triqs::clef::result_of::make_expr_call<std::reference_wrapper<const Obj>,Args...>::type
    operator()( Args&&... args ) const { return make_expr_call (std::ref(* this),args...);}

    // The non const version (which then stores a non-const reference ....)
    template< typename... Args>
    typename triqs::clef::result_of::make_expr_call<std::reference_wrapper<Obj>,Args...>::type
    operator()( Args&&... args ) { return  make_expr_call (std::ref(* this),args...);}

    template<typename Fnt> friend void triqs_clef_auto_assign (Obj & x, Fnt f) { 
    x.v++; std::cout<< " called triqs_clef_auto_assign "<< f(100)<<std::endl;
    }
    friend std::ostream & triqs_clef_formal_print(std::ostream & out, Obj const & x) {return out<<"Obj";}
    };

    int main() {
    Obj f(2);
    triqs::clef::placeholder<3> x_;
    std::cout<< f.v << std::endl;
    f(x_ ) << 8*x_ ;
    //f(x_ + y_) = 8*x_; // leads to a compile error as expected
    std::cout<< f.v << std::endl;
    }

