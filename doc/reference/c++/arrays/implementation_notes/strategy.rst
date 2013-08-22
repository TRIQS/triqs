.. _Design:

Strategy
=============================================================

All the classes are a combination of a system of indices (called IndexMap I in the following)
and a physical storage S in the computer (a block of memory), denoted as an IndexMap_Storage_Pair (I,S).

* I models the IndexMap concept [REF below].

 * It is the bijection between the a set of indices and the position in the memory block. It can be though as a coordinate system on the (linear) memory block.
 * Various types of indices are possible (only the first is implemented now).

    * cuboid (the standard hypercubic array, the only one currently implemented)
    * triangular arrays
    * band matrix
    * multi-indices, with indices made of pair<int,int> e.g.
     
* S models the Storage concept [REF].

 * It is a handle to the memory block containing the actual data.
 * It can be e.g.: 

      * a C++ shared pointer to a memory block.
      * a reference to a numpy array.
 
This design has several consequences : 

* **Interoperability** : classes are widely interoperable, e.g. one can add a array and a matrix (if dimensions are ok of course).
  one can also add a python numpy and a C++ array without any further coding.

* It is straighforward to construct a matrix_view<T> from an array<T,2>, since it is the same couple <I,S>, 
  just interpreted differently.

* It is easy to view a array<T,4> as a matrix by gathering indices (properly ordered in memory) : 
  one just has to provide a new IndexMap I2 to see the same data.
  [ --> very useful for vertex computation in many body...]

* Slicing, or partial view is very natural : it is just a function on indexmaps : I--> I2, 
  independantly of any storage.


Quick guide through the implementation
=============================================================

The implementation is divided into basically 4 parts : 

* Storages : implements two storages shared_block and numpy

* IndexMaps : implements cuboid index map, its domain and iterators

* impl/indexmap_storage_pair.hpp : the basic implementation class for all user class.
  It is basically just a couple of an indexmap and a storage, with shallow copy.
  It also forward the slices to the indexmap and construct the correct views.

* upper level : 
  * user class : array, array_view, matrix, matrix_view, vector, vector_view
  * expression.hpp : boost proto expression
  * numpy_interface.hpp : helper to get numpy into array
  * lapack/blas interface
  * hdf5 support.

