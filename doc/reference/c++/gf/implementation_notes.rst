

Implementation notes
===========================

This part describes how to create a new green function....

.. _Concept_HasConstIterator:

Mesh 
------------------------------------------------- 

* **Purpose**  : An object which has a const_iterator and corresponding functions.

* **Refines** : 

* **Definition** : 
  
+--------------------------------------------------------------+-------------------------------------------------------------------------------+
| Elements                                                     | Comment                                                                       |
+==============================================================+===============================================================================+
| mesh_pt_generator<mesh_t> const_iterator                     | A generator of all the mesh points.                                           |
+--------------------------------------------------------------+-------------------------------------------------------------------------------+
| const_iterator begin()/end() const                           | Standard access to iterator on the mesh                                       |
| const_iterator cbegin()/cend() const                         | Standard access to iterator on the mesh                                       |
+--------------------------------------------------------------+-------------------------------------------------------------------------------+



Doxygen documentation
------------------------------------------------- 


The :doxy:`full C++ documentation<triqs::gf::gf>` is available here.

+------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+
| Elements                                                                           | Comment                                                                       |
+====================================================================================+===============================================================================+
| struct tag {};                                                                     | A tag for the gf                                                              |
+------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+
| mesh_t                                                                             | Mesh for the gf, modeling Mesh concept                                        |
+------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+
| storage_t                                                                          | The type of the storage of the data (array, vector, etc....)                  |
+------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+
| singularity_t                                                                      | Type of object storing the singularities of the gf. It is used e.g. in the    |
|                                                                                    | Fourier transformation, density computation, etc... For a simple g(omega),    |
|                                                                                    | g(t), it is typically a high frequency tail. For a more complex function      |
|                                                                                    | g(nu,nu'), it can be different.                                               |
+------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+
| symmetry_t                                                                         | Type of the object storing the symmetry property of the Green function. It is |
|                                                                                    | *nothing* by default. This type must be a value (DefaultConstructible,        |
|                                                                                    | CopyConstructible, Assignable)                                                |
+------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+
| target_indices_t                                                                   | Type of the indices of the gf, typically array<std::string,arity>             |
+------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+
| static const int arity                                                             | Number of variable authorized in calling the gf (just for compile time check  |
|                                                                                    | and nice error message, it is not really necessary)                           |
+------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+
| struct evaluator { auto operator()( mesh_t const &, DATA_t const &, S_t const &,   | All the permitted const call of the gf !  (DATA_t defined below) with the     |
| Args&&... args) .... as many overload as necessary }                               | parenthesis operator The gf<...> function create such a struct, so it can     |
|                                                                                    | hold some data ...                                                            |
+------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+
| static std::string h5_name()                                                       | Name for hdf5 naming (attribute of the tree in which the gf is stored).       |
+------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+

* **Values vs Views**
  target_t, singularity_t, indices_t are expected to be *values*.
  The corresponding views, i.e., target_view_t, singularity_view_t, indices_view_t will be deduced from the value type, and 
  replaced by the value_type if no view is available.

* S_t is singularity_t or its corresponding view type (if it exists).
  



