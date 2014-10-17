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

We distinguish two separate notions based on whether this function is `pure` 
or not, i.e. whether one can or not modify a(i,j).

* An `Immutable` array is simply a pure function on the domain of definition.
  a(i,j) returns a int, or a int const &, that cannot be modified (hence immutable).

* A `Mutable` array is an Immutable array that *can* be modified. The non-const
object returns a reference, e.g. a(i,j) can return a int &. Typically this is 
a piece of memory, with a integer coordinate system on it.
 
The main point here is that an `Immutable` array is a much more general notion: 
a formal expression consisting of arrays (e.g. A + 2*B) models this concept, 
but not the `Mutable` one.
Most algorithms only use the `Immutable` array notion, where they are pure 
(mathematical) functions that return something depending on the value of an 
object, without side effects.


.. _ImmutableCuboidArray:

ImmutableCuboidArray 
----------------------------

* **Purpose**: 
  
  The most abstract definition of something that behaves like an immutable array on a cuboid domain.
  
   * it has a cuboid domain (hence a rank).
   * it can be evaluated on any value of the indices in the domain
   
   * NB: It does not need to be stored in memory. For example, a formal expression models this concept.

* **Definition** ([...] denotes something optional).

+-------------------------------------------------------+-------------------------------------------------------------------------+
| Members                                               | Comment                                                                 |
+=======================================================+=========================================================================+
| domain_type == cuboid_domain<Rank>                    | Type of the domain, with rank `Rank`                                    |
+-------------------------------------------------------+-------------------------------------------------------------------------+
| domain_type [const &] domain() const                  | Access to the domain.                                                   |
+-------------------------------------------------------+-------------------------------------------------------------------------+
| value_type                                            | Type of the element of the array                                        |
+-------------------------------------------------------+-------------------------------------------------------------------------+
| value_type  [const &] operator() (size_t ... i) const | Evaluation. Must have exactly rank argument (checked at compiled time). |
+-------------------------------------------------------+-------------------------------------------------------------------------+

* **Examples**: 
   * array, array_view, matrix, matrix_view, vector, vector_view.
   * array expressions.

.. _MutableCuboidArray:

MutableCuboidArray 
-------------------------

* **Purpose**:   An array where the data can be modified.
* **Refines**:  :ref:`ImmutableCuboidArray`.

* **Definition** 

+----------------------------------------------+-----------------------------------------------------------------------------+
| Members                                      | Comment                                                                     |
+==============================================+=============================================================================+
| value_type  & operator() (size_t ... i)      | Element access: Must have exactly rank argument (checked at compiled time). |
+----------------------------------------------+-----------------------------------------------------------------------------+

* **Examples**: 
   * array, array_view, matrix, matrix_view, vector, vector_view.

.. _ImmutableArray:

ImmutableArray
-------------------------------------------------------------------

* Refines :ref:`ImmutableCuboidArray`

* If X is the type: 

  * ImmutableArray<A> == true_type

NB: this traits marks the fact that X belongs to the Array algebra.

.. _ImmutableMatrix:

ImmutableMatrix
-------------------------------------------------------------------

* Refines :ref:`ImmutableCuboidArray`

* If A is the type: 

  * ImmutableMatrix<A> == true_type
  * A::domain_type::rank == 2

NB: this traits marks the fact that X belongs to the MatrixVector algebra.

.. _ImmutableVector:

ImmutableVector
-------------------------------------------------------------------

* Refines :ref:`ImmutableCuboidArray`

* If A is the type: 

  * ImmutableMatrix<A> == true_type
  * A::domain_type::rank == 1

NB: this traits marks the fact that X belongs to the MatrixVector algebra.


.. _MutableArray:

MutableArray
-------------------------------------------------------------------

* Refines :ref:`MutableCuboidArray` 

* If A is the type: 

  * ImmutableArray<A> == true_type
  * MutableArray<A> == true_type

NB: this traits marks the fact that X belongs to the Array algebra.

.. _MutableMatrix:

MutableMatrix
-------------------------------------------------------------------

* Refines :ref:`MutableCuboidArray` 

* If A is the type: 

  * ImmutableMatrix<A> == true_type
  * MutableMatrix<A> == true_type
  * A::domain_type::rank ==2

NB: this traits marks the fact that X belongs to the MatrixVector algebra.

.. _MutableVector:

MutableVector
-------------------------------------------------------------------

* Refines :ref:`MutableCuboidArray` 

* If A is the type: 

  * ImmutableMatrix<A> == true_type
  * MutableMatrix<A> == true_type
  * A::domain_type::rank ==1

NB: this traits marks the fact that X belongs to the MatrixVector algebra.


Why concepts ? [Advanced]
-----------------------------

Why is it useful to define these concepts ?

Simply because of lot of the library algorithms only use these concepts,
and such algorithms can be used for any array or custom class that models
the concept.

For example: 

* Problem: we want to quickly assemble a small class to store a diagonal matrix.
  We want this class to operate with other matrices, e.g. be part of an 
  expression, be printed, etc.
  However, we only want to store the diagonal element.

* A simple solution :

  .. triqs_example:: ./concepts_0.cpp
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




