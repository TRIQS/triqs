.. highlight:: c

Compatibility with STL and iterators, Interaction with STL 
##################################################################

The arrays are compatible with STL containers and algorithms.

Iterators
================

 Standard iterators are provided that model the boost Mutable_ForwardIterator and ForwardIterator concepts
 (and hence are STL compliant).

 The iterator implements also two additionnal methods :

 * it can be casted to a bool : it is true iif the iteration is not at end.
 * it.indices() : returns const & to the indices at the point of the iteration.

 Examples:: 

  array<long,2> A (2,3);
  for (auto it = A.begin(); it; ++it) *it =it.indices()[0] + 10 *it.indices()[1] ;
  //for (array<long,2>::iterator it = A.begin(); it; ++it) *it =it.indices()[0] + 10 *it.indices()[1] ;

Indices generators 
================================

The domain have an index generator that enumerates the indices in the domain.

Examples::

  array<long,2> A (2,3);

  for (auto it = A.indexmap().domain().begin(); it; ++it) cout<<"  "<<*it<<endl;

The indices can also be produces in a custom order ::

   typedef IndexMaps::cuboid_index_generator< array<long,2>::indexmap_type::domain_type, Permutations::permutation<0,1> > it_type;
   for (it_type it(A.indexmap().domain()); it; ++it) cout<<"  "<<*it<<endl;

.. warning::
  improve this : too long, by passing a parameter ... of the Permutation type.... with default, not used, to begin....
  something like : 
  for (auto it = A.indexmap().domain().begin(permutation<0,1>()); it; ++it) cout<<"  "<<*it<<endl;



Using STL containers and algorithms
===================================================================

* `Regular classes` have a standard value semantics, which enable them 
  to be used in STL containers, like vector, list, map, unordered_map  ...

.. warning::
   Views can not be used in STL containers. They have no default constructors.

* For example, one can make a vector of arrays ... ::

   array<long,2> A (2,3);
   std::vector<array<long,2> > VV; 
   VV.push_back(A);

* ... or a map ::

    std::map<string, array<long,2> > MAP; 
    MAP["1"] = A;

*  We can put a std::vector in an array ... ::
  
    std::vector<int> V (10);
    array<int,1 > B(V.size()), C(V.size());
    std::copy(V.begin(),V.end(),B.begin());

* ... or in reverse ::
     
    std::copy(B.begin(),B.end(),V.begin()); 

* ... or use other algorithms of std::
       
    bool te(int x) { return (x<25);}
    //....
    cout<<" max(B) "<< *std::max_element(B.begin(),B.end())<<endl;
    std::replace_if (B.begin(), B.end(), te, 0);
    std::swap(B,C);


.. warning::

   It works, but it may not be optimal. For example, swap is certainly not optimal [ to be improved].



