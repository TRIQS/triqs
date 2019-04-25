.. _statevector_concept:

StateVector Concept
===================

* **Purpose** : A container type, which is semantically an element of a vector space over some number field.
                The vector space must be equipped with a scalar product.
                Elements of the container are idenified with expansion coefficients with respect to some fixed basis.
                The container provides random read/write access to its individual elements as well as
                read-only iteration using **foreach** function.

* **Refines** : CopyConstructible

* **Definition** :

+------------------------------------------+---------------------------------------------------------------------------------+
| Members                                  | Comment                                                                         |
+==========================================+=================================================================================+
| value_type                               | Type of element in the container. Values of this type must form a number field. |
+------------------------------------------+---------------------------------------------------------------------------------+
| int size() const                         | Returns the number of elements in the container.                                |
+------------------------------------------+---------------------------------------------------------------------------------+
| value_type [const]& operator()(size_t)   | Read/write access to a single element.                                          |
| [const]                                  |                                                                                 |
+------------------------------------------+---------------------------------------------------------------------------------+

+------------------------------------------+---------------------------------------------------------------------------------+
| Free functions                           |                                                                                 |
+==========================================+=================================================================================+
| foreach(StateVector const& V, L)         | Applies a callable object *L(size_t, value_type)* to (index,value) pairs of *V*.|
|                                          | The iteration goes **at least** through all non-vanising elements in *V*.       |
+------------------------------------------+---------------------------------------------------------------------------------+
| | V1 + V2                                | Addition and subtraction of *StateVector* objects.                              |
| | V1 += V2                               |                                                                                 |
| | V1 - V2                                |                                                                                 |
| | V1 -= V2                               |                                                                                 |
+------------------------------------------+---------------------------------------------------------------------------------+
| | x * V                                  | Multiplication and division of *StateVector* object by a constant of type       |
| | V * x                                  | *value_type*.                                                                   |
| | V \*= x                                |                                                                                 |
| | V / x                                  |                                                                                 |
| | V /= x                                 |                                                                                 |
+------------------------------------------+---------------------------------------------------------------------------------+
| value_type dot_product(V1,V2)            | Scalar product of two *StateVector* objects.                                    |
+------------------------------------------+---------------------------------------------------------------------------------+
| StateVector                              | Returns a copy of *V* with all elements set to zero.                            |
| make_zero_state(StateVector const& V)    |                                                                                 |
+------------------------------------------+---------------------------------------------------------------------------------+
