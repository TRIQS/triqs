.. highlight:: c

.. _clef_auto_assign:

Automatic assignment of containers 
===================================

Another use of expression is the automatic assignment of containers.

**Synopsis** : 

 If C is a container, ::

  C(x_)        << some_expression_of_x_
  C[i_]        << some_expression_of_i_
  C(x_)(i_,j_) << some_expression_of_x_i_j
  C[x_](i_,j_) << some_expression_of_x_i_j

depending of course of the operator that the container support.
The Right Hand Side (RHS) of the << statement can be any expression, 
while Left Hand Side (LHS) of the << sign is a container supporting the operation (see below).

This statement simply will be rewritten by the CLEF library as ::

  triqs_clef_auto_assign (C, x_ -> some_expression_of_x_);                  // pseudo code
  triqs_clef_auto_assign (C, [](auto x_) {return some_expression_of_x_;});  // or in C++ lambda syntax

The function `triqs_clef_auto_assign` has to be overloaded for the container and the correct
functional form, and it is expected to fill C by evaluating this function.

Such a function is provided for TRIQS objects (arrays, matrix, Green function),
and also for some STL container like std::vector.

Example :

.. compileblock::

  #include "triqs/clef.hpp"
  #include <iostream> 
  using namespace triqs::clef;

  int main() { 
  int N = 5;
  double pi = std::acos(-1);

  // automatic assignment of vector 
  placeholder <0> k_; 
  std::vector<double> V(N);
  make_expr(V) [k_] << cos( (2* pi* k_)/ N );

  // chaining them ...
  placeholder <1> i_; 
  std::vector<std::vector<double>> W(3, std::vector<double>(N));
  make_expr(W)[i_] [k_] << i_ + cos( (2* pi* k_)/ N );

  // check result... 
  for (size_t u=0; u<V.size(); ++u) if (std::abs(V[u] -cos((2*pi*u)/N))> 1.e-10) throw "error!";
  for (size_t w=0; w<W.size(); ++w) 
   for (size_t u=0; u<W[w].size(); ++u) 
   if (std::abs( W[w][u] - (w+cos((2*pi*u)/N)))> 1.e-10) throw "error!";
  }

**Details**

The synopsis of the `triqs_clef_auto_assign` functions is ::

   template<typename Fnt> void triqs_clef_auto_assign (Obj & x, Fnt f);

The compiler will then rewrite ::

   obj(x_,y_, ...) = expression

into ::

   triqs_clef_auto_assign (obj, make_function( expression, x_, y_, ....)) 

The function must be found by ADL. It is therefore useful to implement it e.g. as a friend function.

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
    using namespace triqs::clef;

    struct Obj{ 
     double v; 
     TRIQS_CLEF_IMPLEMENT_LAZY_CALL();
     // 
     template<typename Fnt> friend void triqs_clef_auto_assign (Obj & x, Fnt f) { 
       std::cout<< " called triqs_clef_auto_assign "<< f(x.v++)<<std::endl;
     }
    };

    int main() {
     Obj f{2};
     placeholder<3> x_;
     std::cout<< f.v << std::endl;
     f(x_ ) << 8*x_ ;
     std::cout<< f.v << std::endl;
    }

