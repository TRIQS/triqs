
.. highlight:: c

std::vector and math
---------------------------------

A simple example using std::vector.

.. compileblock::

   #include "triqs/clef.hpp"

   int main() {
   // init some variables  
   int N = 10; double pi = std::acos(-1); std::vector<double> V(N);

   // Filling the vector automatically from an expression of k_
   triqs::clef::placeholder<0> k_; 
   triqs::clef::lazy(V) [k_]  << cos( (2* pi* k_)/ N );

   // check result... 
   for (size_t u=0; u<V.size(); ++u)
   std::cout<< u << " "<<V[u]<< "  "<< cos((2*pi*u)/N)<<std::endl;
   }

