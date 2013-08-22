.. highlight:: c

.. _STL:

Iterators and STL
##################################################################

Standard iterators are provided that model the boost Mutable_ForwardIterator and ForwardIterator concepts
(and hence are STL compliant).

The iterator implements also two additional methods :

* it can be casted to a bool : it is true iif the iteration is not at end.
* it.indices() : returns const & to the indices at the point of the iteration.

Examples:: 

 array<long,2> A (2,3);
 for (auto it = A.begin(); it; ++it) *it =it.indices()[0] + 10 *it.indices()[1];


Some examples of usage : 


.. compileblock::

  #include <triqs/arrays.hpp>
  #include <vector>
  #include <map>
  #include <algorithm>
  using triqs::arrays::array; using triqs::arrays::matrix; using triqs::clef::placeholder;
  int main(){

   // For example, one can make a vector of arrays ... ::

   array<long,2> A (2,3);
   std::vector<array<long,2> > VV; 
   VV.push_back(A);

   //  ... or a map ::
   std::map<int, array<long,2> > MAP; 
   MAP[1] = A;
   
   //  We can put a std::vector in an array ... ::
   // --> should make a direct constructor for this.. + factory in the reverse direction...
   std::vector<int> V (10,2);
   array<int,1 > B(V.size()), C(V.size());
   std::copy(V.begin(),V.end(),B.begin());

   //  ... or in reverse ::
   B*=2;  
   std::copy(B.begin(),B.end(),V.begin()); 
  
   //  ... or use other algorithms of std::
   std::cout<<" max(B) "<< * std::max_element(B.begin(),B.end())<<std::endl;

   // or replace 
   placeholder<0> i_; 
   B(i_) <<  3*i_ ;
   std::cout<<" B "<< B << std::endl;
   std::replace_if (B.begin(), B.end(), [](int i) { return i>21;}, 0);
   std::cout<<" B "<< B << std::endl;
    
  }



