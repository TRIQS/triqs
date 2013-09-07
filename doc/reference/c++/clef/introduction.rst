.. highlight:: c


Motivation : a little tour of CLEF 
=====================================

A usual, the best is to start with a few examples, to show the library in action.


.. compileblock::

  #include <triqs/clef.hpp>
  #include <triqs/arrays.hpp>
  #include <iostream> 
  #include <algorithm> 
  int main() { 
   // Declaring some placeholders (i.e. dummy variables). 
   triqs::clef::placeholder <0> i_; 
   triqs::clef::placeholder <1> j_; 

   // Declaring a 3x3 matrix
   triqs::arrays::matrix<double> A (3,3);

   // Automatically filling the matrix 
   // -> forget about the bounds, it is automatic
   // -> forget about the best order to order the for loops for performance, it is also automatic
   A(i_,j_) << i_ + 2*j_;

   // Cheking the result
   std::cout<< A<< std::endl;

   // It also works for std container: we just have to add a call clef::make_expr function
   std::vector<double> V(10); 
   double pi = std::acos(-1);

   // Automatically filling the vector with the evaluation of the expression in i_
   triqs::clef::make_expr(V) [i_] << cos( 2* pi / 5.0 * i_ );
  
   // -> by the way, the constant calculation is precomputed
   // (expressions are partially evaluated as soon as possible)
   // illustration : 
   // the time_consuming_function will be called only once in the loop, while cos is called 10 times
   auto time_consuming_function=[](double x){std::cout<<"call time_consuming_function"<<std::endl;return 2*x;};  
   triqs::clef::make_expr(V) [i_] << cos( time_consuming_function(10) * i_ );

   // If you insist using on more complex container ....
   std::vector<std::vector<double>> W(3, std::vector<double>(5));
   triqs::clef::make_expr(W)[i_] [j_] << i_ + cos( time_consuming_function(10) * j_  + i_);

   // You can also put a CLEF expression in a std::function
   // a function i -> 2*i +1 
   std::function<int(int)>        f =   i_ >>  2*i_ +1;
   // a function (i,j)  -> 2*i +j
   std::function<double(int,int)> g =  var(i_,j_) >> 2*i_ +j_; 
   // checking ...
   std::cout<< "f(10) =" << f(10)<< "  g(1,2) =" << g(1,2)<< std::endl;   
   
   // You can also use a Curry form : h is a function i-> j -> 2*i+ j
   auto h = i_ >> (j_ >> 2*i_ +j_);
   std::cout<< "h(1)(2) = " << h(1)(2) << std::endl;

   // You an also use this to quickly write some lambda, as an alternative syntax to the C++ lambda
   // with e.g. STL algorithms (with the advantage that the function is polymorphic !).
   std::vector<int> v = {0,-1,2,-3,4,5,-6};
   // replace all negative elements (i.e. those for which i -> (i<0) return true), by 0
   std::replace_if(begin(v), end(v), i_ >> (i_<0), 0);
   // for non believer, it really worked ... 
   for (auto const & x : v) std::cout <<x <<" "; std::cout << std::endl; 
   }



