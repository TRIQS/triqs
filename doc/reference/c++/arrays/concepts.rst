Concepts
=============================================================

In this section, we define the basic concepts (in the C++ sense)
related to the multidimentional arrays. 
Readers not familiar with the idea of concepts in programming can skip this section,
which is however needed for a more advanced usage of the library.


A multidimentional array is basically a function of some indices, typically integers taken in a specific domain, 
returning the element type of the array, e.g. int, double. 
Indeed, if a is an two dimensionnal array of int, 
it is expected that a(i,j) returns an int or a reference to an int, for i,j integers in some domain.

We distinguish two separate notions, whether this function is `pure` or not, 
i.e. whether one can or not modify a(i,j).

* An `Immutable` array is just a pure function on the domain of definition.
  a(i,j) returns a int, or a int const &, that can not be modified (hence immutable).

* A `Mutable` array is on the other hand, a piece of memory, addressed by the indices, 
  which can be modified. a(i,j) can return a int &.

The formal definition is given below

.. note:: 
 
   The tag (made by derivation) is use to quickly recognize that an object
   models a given concept, due to the current lack of concept support in C++.
 
   It is used e.g. to detect to which algebra (Array or Matrix/Vector) the object belongs...

.. _ImmutableCuboidArray:

ImmutableCuboidArray 
----------------------------

* **Purpose** : 
  
  The most abstract definition of something that behaves like an immutable array.
  
   * it has a domain (hence a rank).
   * it can be evaluated on any value of the indices in the domain
   
   * NB : It does not need to be stored in memory. A formal expression, e.g. model this concept.

* **Definition** ([A] denotes something optional).

 * The class derives from : TRIQS_CONCEPT_TAG_NAME(ImmutableCuboidArray) 
  
 ==================================================================================================  =============================================================================
 Elements                                                                                            Comment
 ==================================================================================================  =============================================================================
 domain_type                                                                                         Type of the domain.
 domain_type [const &] domain() const                                                                Access to the domain.
 value_type                                                                                          Type of the element of the array
 value_type  [const &] operator() (size_t ... i) const                                               Evaluation. Must have exactly rank argument (checked at compiled time).
 ==================================================================================================  =============================================================================

* **Examples** : 
   * array, array_view, matrix, matrix_view, vector, vector_view.
   * array expressions.

.. _MutableCuboidArray:

MutableCuboidArray 
-------------------------

* **Purpose** :   An array where the data can be modified...
* **Refines** :  :ref:`ImmutableCuboidArray`.


* **Definition** ([A] denotes something optional).

 * The class derives from : TRIQS_CONCEPT_TAG_NAME(MutableCuboidArray) 
  
 ==================================================================================================  =============================================================================
 Elements                                                                                            Comment
 ==================================================================================================  =============================================================================
 domain_type                                                                                         Type of the domain.
 domain_type [const &] domain() const                                                                Access to the domain.
 value_type                                                                                          Type of the element of the array
 value_type const & operator() (size_t ... i) const                                                  Element access: Must have exactly rank argument (checked at compiled time).
 value_type       & operator() (size_t ... i)                                                        Element access: Must have exactly rank argument (checked at compiled time).
 ==================================================================================================  =============================================================================

* **Examples** : 
   * array, array_view, matrix, matrix_view, vector, vector_view.

.. _ImmutableMatrix:

ImmutableMatrix
-------------------------------------------------------------------

Same as :ref:`ImmutableCuboidArray`, except that : 

 * the rank of the domain must be 2 
 * the class must derive from TRIQS_CONCEPT_TAG_NAME(ImmutableMatrix).


.. _ImmutableVector:

ImmutableVector
-------------------------------------------------------------------

Same as :ref:`ImmutableCuboidArray`, except that : 

 * the rank of the domain must be 1 
 * the class must derive from TRIQS_CONCEPT_TAG_NAME(ImmutableVector).


.. _MutableMatrix:

MutableMatrix
-------------------------------------------------------------------

Same as :ref:`MutableCuboidArray`, except that : 

 * the rank of the domain must be 2 
 * the class must derive from TRIQS_CONCEPT_TAG_NAME(MutableMatrix).

.. _MutableVector:

MutableVector
-------------------------------------------------------------------

Same as :ref:`MutableCuboidArray`, except that : 

 * the rank of the domain must be 1 
 * the class must derive from TRIQS_CONCEPT_TAG_NAME(MutableVector).


Why concepts ? [Advanced]
-----------------------------

Why is it useful to define those concepts ?

Simply because of lot of the library algorithms only use those concepts, and can be used 
for an array, or any custom class that model the concept.

Example : 

* Problem: we want to quickly assemble a small class to store a diagonal matrix.
  We want this class to operate with other matrices, e.g. be part of expression, be printed, 
  or whatever.
  But we only want to store the diagonal element.

* A simple solution :

  .. compileblock ::

    #include <triqs/arrays.hpp>
    #include <iostream>
    namespace triqs { namespace arrays { // better to put it in this namespace for ADL... 
     
     template<typename T> class immutable_diagonal_matrix_view : TRIQS_CONCEPT_TAG_NAME(ImmutableMatrix) { 
      
      array_view<T,1> data; // the diagonal stored as a 1d array
      
      public:
     
      immutable_diagonal_matrix_view(array_view<T,1> v) : data (v) {} // constructor
         
      // the ImmutableMatrix concept 
      typedef indexmaps::cuboid::domain_t<2> domain_type;
      domain_type domain() const { auto s = data.shape()[0]; return {s,s}; }
      typedef T value_type;
      T operator()(size_t i, size_t j) const { return (i==j ? data(i) : 0);} // just kronecker...
      
      friend std::ostream & operator<<(std::ostream & out, immutable_diagonal_matrix_view const & d) { return out<<"diagonal_matrix "<<d.data;}
     };
    }}
          
    /// TESTING 
    using namespace triqs::arrays;
    int main(int argc, char **argv) {
     auto a = array<int,1> {1,2,3,4};
     auto d = immutable_diagonal_matrix_view<int>{a};
     std::cout << "domain = " << d.domain()<< std::endl;  
     std::cout << "d   = "<< d << std::endl;
     std::cout << "2*d = "<< matrix<int>(2*d) << std::endl;
     std::cout << "d*d = "<< matrix<int>(d*d) << std::endl;
    }


* Discussion

  * Of course, this solution is not perfect. Several algorithms could be optimised if we know that a matrix is diagonal.
    E.g. multiplying a diagonal matrix by a full matrix. Currently, it creates a full matrix from the diagonal one, and 
    call gemm. This is clearly not optimal.

    However, this is not the point.

    This class *just works* out of the box, and takes only a few minutes to write.
    One can of course then work more and specialize e.g. the operator * to optimize the multiplication, 
    or any other algorithm, `if and when this is necesssary`. That is an implementation detail, 
    that be done later, or by someone else in the team, without stopping the work.

  * One can generalize for a Mutable diagonal matrix. Left as an exercise...




