
Concepts
=============================================================

BoostSerializable
-------------------------------------------------

* define serialize for boost serialize library

Printable
-------------------------------------------------

  ======================================================  ===========================================================
  Elements                                                Comment
  ======================================================  ===========================================================
  std::ostream & operator << (std::ostream & out, ...)    Printing  
  ======================================================  ===========================================================

Domain
------------------------------------------------- 

* **Purpose**  : The domain of definition of the array, i.e. the possible value of the indices.
* **Refines** : BoostSerializable, Printable
* **Definition** : 

+------------------------------------------------------------------+-------------------------------------------------------+
| Elements                                                         | Comment                                               |
+==================================================================+=======================================================+
| static const unsigned int rank                                   | rank                                                  |
+------------------------------------------------------------------+-------------------------------------------------------+
| index_value_type                                                 | type of the multi-index                               |
+------------------------------------------------------------------+-------------------------------------------------------+
| default constructor, copy construction                           |                                                       |
+------------------------------------------------------------------+-------------------------------------------------------+
| operator =                                                       |                                                       |
+------------------------------------------------------------------+-------------------------------------------------------+
| operator ==, !=                                                  |                                                       |
+------------------------------------------------------------------+-------------------------------------------------------+
| size_t number_of_elements() const                                | number of elements                                    |
+------------------------------------------------------------------+-------------------------------------------------------+
| generator                                                        | type of the IndexGenerator that generates the indices |
+------------------------------------------------------------------+-------------------------------------------------------+
| begin() const/ end() const                                       | a generator at start/end                              |
+------------------------------------------------------------------+-------------------------------------------------------+

* **Examples** :
  
  Typically, this is is a multi-index for an array, matrix, ...., e.g.
  
   * Cuboid<rank> : standard hyperrectangular arrays. This is little more than the tuple of the lengths.
   * triangle, .... ?

.. _HasImmutableArrayInterface:

HasImmutableArrayInterface
------------------------------------------------- 

* **Purpose** : The most abstract definition of something that behaves like an immutable array.
   * it has a domain (hence a rank).
   * it can be evaluated on any value of the indices in the domain
   * By combining the generator of the domain with the evaluation, it is therefore easy to 
     iterate on the values of such an object. 
   * NB : It does not need to be stored in memory. A formal expression, e.g. model this concept.

* **Definition** ([A|B] denotes that the return type maybe A or B).

 ==================================================================================================  =============================================================
 Elements                                                                                            Comment
 ==================================================================================================  =============================================================
 value_type                                                                                          Type of the element of the array
 domain_type                                                                                         Type of the domain.
 [domain_type const & | domain_type] domain() const                                                  Access to the domain.
 [ value_type | value_type const &] operator[] (domain_type::index_value_type const &) const         Evaluation.
 ==================================================================================================  =============================================================

* **Examples** : 
   * array, array_view, matrix, matrix_view, vector, vector_view (all implemented as Indexmap_Storage_Pair)
   * array expressions (in which case the returns are not const & since they are computed, not stored).

IndexGenerator
-------------------------------------------------
* **Purpose** : Generate the indices of a domain.

* **Definition** :   

 ==============================================================  ==================================================================================================
 Elements                                                        Comment
 ==============================================================  ==================================================================================================
 domain_type                                                     Type of the domain whose indices are generated.
 default contruction, copy construction 
 construction from (domain_type const &, bool atend=false)       
 IndexGenerator & operator=(const IndexGenerator & )
 operator ++                                                     
 operator =
 operator ==, !=
 domain_type::index_value_type  const & operator * () const      Access to the value of the multi-index
 bool at_end() const                                             True iif the generator has reached the last value
 ==============================================================  ==================================================================================================


* **Examples** :

  * cuboid_index_generator 

IndexMap
-------------------------------------------------

* **Purpose** : 
  
  * Store the mapping of the index domain to a linear array. 
  * It is basically a function : indices --> 1d position, the bijection between the indices
    of an element and its position in the memory block.
* **Refines** : BoostSerializable, Printable
* **Definition** : 

 ========================================================================  ==================================================================================================
 Elements                                                                  Comment
 ========================================================================  ==================================================================================================
 * domain_type                                                             The type of the domain.
 * domain_type const & domain() const                                      The domain.
 * default constructor, copy construction                                  Cpy is a true copy.
 * can be constructed from domain_type const &     
 * size_t operator[] (domain_type::index_value_type const & key ) const    The mapping itself.
 * iterator                                                                A type modeling IndexMapIterator, which is the optimal memory traversal.
                                                                           NB : the order of indices is chosen for optimal traversal of memory, it 
                                                                           does not need to be "natural". 
                                                                           cuboid_map also provides a natural_iterator for that purpose.
 ========================================================================  ==================================================================================================
                                                                           
* The type also has to define two free functions and to specialize a template : 

 ==========================================================  ==================================================================================================
 Elements                                                    Comment
 ==========================================================  ==================================================================================================
 * bool compatible_for_assignment (M1, M2)                   Returns whether an array/matrix/vector with map M1 can be equated to a array/matrix/vector with 
                                                             map M2   
 * bool raw_copy_possible (M1, M2)                           Is the assignment of an array/matrix/vector with map M2 into an array/matrix/vector with map M1 
                                                             doable with raw copy
 * struct indexmap_iterator_adapter< It, I >                 Metafunction :  

                                                                - I is the IndexMap class
                                                                - It any similar IndexMapIterator which returns (in ::type) the IndexMapIterator on I
                                                                  with the same order traversal as It.
                                                             
                                                             Example : It is a IndexMapIterator on I1 stored in C order, I is in Fortran order, 
 	                                                     the result will be an IndexMapIterator on I that presents the data of I in C order
                                                             This is used in copying array with different indexmaps.
 ==========================================================  ==================================================================================================


* **Examples** : 
 * cuboid_map<IterationOrder>   :  a map of the cuboid indices in a fixed order in memory.

IndexMapIterator
-------------------------------------------------

* **Purpose** :  
    * A basic iterator on an IndexMap which can be dereferenced into the shift of successive elements compared to the start of the memory block. 
    * These iterators are kept as simple as possible, so that it is easy to implement new indices maps and their iterators.
    * NB : In particular, they are *not* necessary STL-compliant. The array_iterator class will 
   take such an iterator and a Storage and produce a true, STL compliant iterator on the array (iterator_adapter).

* **Definition** : 

 ==========================================================  ==================================================================================================
 Elements                                                    Comment
 ==========================================================  ==================================================================================================
 indexmap_type                                               The index_map on which the iterator is iterating
 domain_type                                                 Type of the domain whose indices are generated.
 default contruction, copy construction 
 construction from (domain_type const &, bool atend=false)    
 IndexMapIterator & operator=(const IndexMapIterator & )
 IndexMapIterator & operator ++                              
 operator ==, !=
 std::ptrdiff_t  operator*() const                           Dereference as a shift from the beginning of the array 
 domain_type::index_value_type  const & indices () const     Access to the value of the multi-index at the iterator position
 bool at_end() const                                         True iif the generator has reached the last value (in practice quicker that it = XX.end()).
 ==========================================================  ==================================================================================================

* **Example(s)** :

  * cuboid_map_iterator

Storage  
-------------------------------------------------

* **Purpose** : 
   * The storage of the array in memory, e.g. plain C++ array, a numpy, etc...
   * A Storage keeps the reference to the memory block where the array is stored.
   * NB : This memory block can be typically shared between various arrays and views, 
     so the Storage is just a reference. The memory is deallocated only
     when all storages referencing it has been destroyed.
* **Refines** :  BoostSerializable
* **Definition** :

   ======================================================  ==================================================================================================
   Elements                                                Comment
   ======================================================  ==================================================================================================
   value_type                                              Type of the element stored, e.g. int, const int, double, const double, ...
   default construction                                    Makes a storage of size 0
   copy construction                                       a shallow copy (another reference to the same data).
                                                           the copy construction is possible from another storage of the same value_type
                                                           up to the const qualifier.
                                                           The construction of a storage with value_type=T from a storage with value_type const T
                                                           is forbidden at compile time.
   void operator = (const STO &)                           A shallow copy of the reference to the data.
 
   clone() const                                           Create a clone of the data.
   const_clone() const                                     Create a clone of the data with const value_type (e.g. int--> const int).
 
   void raw_copy_from(const STO & X)                       Copy all the data from X to * this. Behaviour undefined if sizes do not match.
       
   size_t size() const                                     Number of elements in the storage
  
   value_type & operator[](size_t p) const                 Access to the data.  Behaviour is undefined if empty()==true.
   ======================================================  ==================================================================================================



StorageOrder concept
-------------------------------------------------

* **Purpose** :

  * Store the order of indices in memory.
  * Can be fixed at compile time, or dynamically (not implemented).

* **Refines** :  BoostSerializable
* **Definition** :

   ======================================================  ==================================================================================================
   Elements                                                Comment
   ======================================================  ==================================================================================================
   size_t index_number(size_t i)
   static unsigned int rank
   default construction                                    
   copy construction                                       
   bool is_Fortran() const                                 Is it Fortran-style ordering ?
   bool is_C() const                                       Is it C-style ordering ?
   
   ======================================================  ==================================================================================================


* The type also has to define the == operator : 

 ==========================================================  ==================================================================================================
 Elements                                                    Comment
 ==========================================================  ==================================================================================================
 Operator ==                                                 Defined between any of the ordering.
 ==========================================================  ==================================================================================================


