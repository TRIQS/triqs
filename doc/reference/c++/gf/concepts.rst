.. highlight:: c

Green functions  
#################

Here are the concepts and techniques related to the Green functions.

All Green functions are implemented as a gf<Descriptor> and gf_view<Descriptor> where : 

* Descriptor contains everything specific to the Green function
  (number of variables, domain of definition, interpolation techniques, etc...).

* The gf/gf_view class implements the generic part (hdf5, view mechanism, interaction with clef library, etc...).

We first present the concept of the Descriptor, and the concepts of its elements.
In a second part, we present a more detailed description of gf/gf_view.

Pure functions on domains
==============================

This formalize a mathematical function : 

* it has a domain of definition
* it can be called on any point of the domain, as a *pure* function, i.e. without any side effect.

Domain concept
------------------------------------------------- 

* **Purpose**  : The domain of definition of a function. It is a mathematical definition of the domain,
  and does not contain any mesh, or details on its representation in a computer.

* **Refines** : CopyConstructible, DefaultContructible, EqualComparable, BoostSerializable, H5-serializable.

* **Definition** : 

+----------------------------------------------------------------------------+---------------------------------------------------------------------+
| Elements                                                                   | Comment                                                             |
+============================================================================+=====================================================================+
| point_t                                                                    | Type of element in the domain (int, int, double, k_vector, ...) as  |
|                                                                            | in the call of  a function over this domain. In particular, in      |
|                                                                            | Matsubara, it is a complex.                                         |
+----------------------------------------------------------------------------+---------------------------------------------------------------------+

* **Examples** :
  
   * Matsubara frequencies (boson/fermion)
   * Matsubara time
   * Real frequencies
   * Real time 
   * Brillouin zone


PureFunctionOnDomain 
-----------------------

* **Purpose**  : A function from a domain to a target space. 

* **Refines**   :

* **Definition** : 

+--------------------------------------+----------------------------------------------------------+
| Elements                             | Comment                                                  |
+======================================+==========================================================+
| domain_t                             | Type of the Domain represented, modelling Domain concept |
+--------------------------------------+----------------------------------------------------------+
| domain_t const & domain() const      | Returns the domain                                       |
+--------------------------------------+----------------------------------------------------------+
| operator (domain_t::point_t) const   | Calling for all elements of the Domain (including infty  |
|                                      | if it is in the domain...                                |
+--------------------------------------+----------------------------------------------------------+

* NB : Note that the return type of the function is *NOT* part of the concept, 
  it has to be deduced by the compiler (using C++11 decltype, std::result_of, eg..).

Functions stored on a mesh
================================

A special kind of function is those stored on a mesh.
The mesh can be regular or not, multi-dimensionnal.

We first define the concept of a mesh, and then the concept of a function discretized over that mesh.

Mesh concept
------------------------------------------------- 

* **Purpose**  : A mesh over a domain, and more generally the practical representation of the domain in a computer.
  It does not really need to be a mesh : e.g. if the function is represented on a polynomial basis, 
  it is the parameters of this representation (max number of coordinates, e.g.)

* **Refines** : CopyConstructible, DefaultContructible, EqualComparable, BoostSerializable, H5-serializable.

* **Definition** : 
  
+--------------------------------------------------------------+-------------------------------------------------------------------------------+
| Elements                                                     | Comment                                                                       |
+==============================================================+===============================================================================+
| domain_t                                                     | Type of the Domain represented, modeling the Domain concept                   |
+--------------------------------------------------------------+-------------------------------------------------------------------------------+
| domain_t const & domain() const                              | Access to the domain                                                          |
+--------------------------------------------------------------+-------------------------------------------------------------------------------+
| index_t                                                      | Type of indices of a point on the grid. Typically a tuple of long or a long   |
+--------------------------------------------------------------+-------------------------------------------------------------------------------+
| size_t size() const                                          | The number of points in the mesh.                                             |
+--------------------------------------------------------------+-------------------------------------------------------------------------------+
| domain_t::point_t index_to_point(index_t) const              | From the index of a mesh point, compute the corresponding point in the domain |
+--------------------------------------------------------------+-------------------------------------------------------------------------------+
| size_t index_to_linear(index_t const &) const                | Flattening the index of the mesh into a contiguous linear index               |
+--------------------------------------------------------------+-------------------------------------------------------------------------------+
| mesh_point_t                                                 | See below                                                                     |
+--------------------------------------------------------------+-------------------------------------------------------------------------------+
| mesh_point_t operator[](index_t const & index ) const        | From an index, return a mesh_point_t containing this a ref to this mesh and   |
|                                                              | the index.                                                                    |
+--------------------------------------------------------------+-------------------------------------------------------------------------------+
| mesh_pt_generator<mesh_t> iterator                           | A generator of all the mesh points.                                           |
+--------------------------------------------------------------+-------------------------------------------------------------------------------+
| iterator begin()/end() const                                 | Standard access to iterator on the mesh                                       |
+--------------------------------------------------------------+-------------------------------------------------------------------------------+
| friend bool operator == (mesh_t const& M1, mesh_t const &M2) | Comparison between meshes                                                     |
+--------------------------------------------------------------+-------------------------------------------------------------------------------+


* **Examples** : Some domains and the corresponding possible meshes.

+-----------------------------------------------------+--------------------------------------------------------+
| Domain Type                                         | What does the corresponding mesh type contain ?        |
+=====================================================+========================================================+
| Matsubara frequencies                               | Nmax, the max number of Matsubara Freq.                |
+-----------------------------------------------------+--------------------------------------------------------+
| Matsubara time                                      | The time slicing is on a mesh, or the Legendre mesh is |
|                                                     | we store the function with Legendre representation.    |
+-----------------------------------------------------+--------------------------------------------------------+
| Real frequencies                                    | Parameters of the mesh in frequency                    |
+-----------------------------------------------------+--------------------------------------------------------+
| Real time                                           |                                                        |
+-----------------------------------------------------+--------------------------------------------------------+
| Brillouin zone                                      | parameters of the mesh over the BZ, symmetry ?         |
+-----------------------------------------------------+--------------------------------------------------------+


MeshPoint concept
------------------------------------------------- 

* **Purpose**  : Abstraction of a point on a mesh. A little more than a ref to the mesh and a index.

* **Refines** :  CopyConstructible.

* **Definition** : 

+------------------------------------------------+-----------------------------------------------------------------------------+
| Elements                                       | Comment                                                                     |
+================================================+=============================================================================+
| mesh_t                                         | Type of the mesh                                                            |
+------------------------------------------------+-----------------------------------------------------------------------------+
| mesh_t const * m                               | A pointer to the mesh to which the point belongs.                           |
+------------------------------------------------+-----------------------------------------------------------------------------+
| mesh_t::index_t index                          | The index of the point                                                      |
+------------------------------------------------+-----------------------------------------------------------------------------+
| mesh_point_t( mesh_t const &, index_t const &) | Constructor                                                                 |
+------------------------------------------------+-----------------------------------------------------------------------------+
| mesh_t::index_t [const &|] index() const       | The index corresponding to the point                                        |
+------------------------------------------------+-----------------------------------------------------------------------------+
| size_t linear_index() const                    | The linear index of the point (same as m->index_to_linear(index())          |
+------------------------------------------------+-----------------------------------------------------------------------------+
| void advance()                                 | Advance to the next point on the mesh (used by iterators).                  |
+------------------------------------------------+-----------------------------------------------------------------------------+
| void at_end()                                  | Is the point at the end of the grid                                         |
+------------------------------------------------+-----------------------------------------------------------------------------+
| void reset()                                   | Reset the mesh point to the first point                                     |
+------------------------------------------------+-----------------------------------------------------------------------------+
| cast_t                                         | type of the corresponding domain point                                      |
| operator cast_t() const                        | cast to the corresponding domain point                                      |
+------------------------------------------------+-----------------------------------------------------------------------------+
| Implements the basic operations on the domain  | Only for non composite mesh                                                 |
| by using the cast operation                    |                                                                             |
+------------------------------------------------+-----------------------------------------------------------------------------+

The MeshPoint mechanism
---------------------------

A MeshPoint is just a storage of a reference to the mesh and the index of the point in a custom structure.
The interest of having such a structure is that : 

* The gf function has a operator()(mesh_t::mesh_point_t) (see below) which is a direct access to the data on the grid.
  Hence if MP is a such a MeshPoint, g(MP) is equivalent to something like g.data_on_grid_at_index( MP.index)

* MP however can be casted to a point in the domain and therefore it *is* a domain_t::point_t as well.

  As a result, g(MP) = 1/(MP + 2) makes senses iif it makes senses in the domain.

* Moreover, because of the iterator on the mesh, one can write ::

    // example of g, a Green function in Matsubara frequencies w
    for (auto w : g.mesh()) 
       g(w) = 1/(w + 2)
    // This runs overs the mesh, and fills the function with 1/(w+2)
    // In this expression, w is casted to the domain_t::point_t, here a complex<double>
    // which allows to evaluate the function


FunctionOnMesh concept 
-----------------------------

* **Purpose**  : A function from a domain to a target space, represented in a mesh.
  This function can only be evaluated on the mesh points. 

* **Refines**   : BoostSerializable, H5-serializable.

* **Definition** : 

+--------------------------------------------------+------------------------------------------------------------------------+
| Elements                                         | Comment                                                                |
+==================================================+========================================================================+
| mesh_t                                           | Type of the mesh representing the domain.                              |
+--------------------------------------------------+------------------------------------------------------------------------+
| mesh_t const & mesh() const                      | Returns the mesh.                                                      |
+--------------------------------------------------+------------------------------------------------------------------------+
| auto operator   ( grid_pt<mesh_t> const &) const | Calling on a grid_pt gives direct access to the value on a grid point. |
| auto & operator ( grid_pt<mesh_t> const &)       | Const and non const version.                                           |
+--------------------------------------------------+------------------------------------------------------------------------+

* **NB** : the result type of the () operator is deduced. Not needed in the concept.


descriptors
======================================

A descriptor is a structure that contains  everything specific to the Green function
(number of variables, domain of definition, interpolation techniques, etc...).


Descriptor concept
---------------------------

* **Purpose**  :

* **Refines**   : 
 
* **NB** : Pure static, does NOT contains any data.

* **Definition** : 

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
  

The gf/gf_view class
====================

The gf/gf_view classes are generic Green function, templated on Descriptor.

They handle : 

* view/non view aspect, copy, etc...
* hdf5 interface
* clef interface
* the MeshPoint mechanism as explained above.
* perfect forwarding of all other const call of operator() to Descriptor.

Constructors are limited to a minimal number :

* empty one for gf (value semantics).
* copy construction, from gf and gf_view of the same descriptor.
* construction from the data of the GF.

The other custom construction are delegated to make_gf functions::

   gf<Descriptor> make_gf (Descriptor, my_favorite arguments).

We use here the simplest dispatch using the fact that Descriptor is an empty struct, 
so we can dispath the make_gf. Example of use ::

   auto G = make_gf (matsubara_freq(), beta, Fermion, make_shape(2,2));
  


Doxygen documentation
======================

The :doxy:`full C++ documentation<triqs::gf::gf>` is available here.

