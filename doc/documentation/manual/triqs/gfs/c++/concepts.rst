.. highlight:: c

Concepts
#################

A Green function is simply a function, which has:

* a `domain` for its variable(s) (e.g. Matsubara/real time/frequencies, Legendre coefficients).
* a `target` space, i.e. the value of the Green function which can be:

   * a scalar (double, complex)
   * a matrix,
   * another Green function (See below, currying Green functions ... REF ... ).

In this section, we define the general concepts for these objects.

First, we need to distinguish the `domain` on which the function is defined
from its representation in a computer, which we call a `mesh`.

.. note::

    "mesh" should be understood here in a general and abstract way,
    as the representation of the domain in the computer.
    In most cases, it is indeed a real mesh on a domain (e.g. a Brillouin zone),
    but the set of Legendre coefficients is also a mesh in our sense.

We will therefore now formally define the concept for `domain`, for `mesh`,
the notion of `pure function on a domain` (i.e. a mathematical Green function)
and the notion of `function on a grid`.


.. _Concept_Domain:

Domain
-------------------------------------------------

* **Purpose** : The domain of definition of a function. It is a mathematical definition of the domain,
  and does not contain any mesh, or details on its representation in a computer.

* **Refines**: RegularType.

* **Definition**:

+----------+--------------------------------------------------------------------+
| Elements | Comment                                                            |
+==========+====================================================================+
| point_t  | Type of element in the domain (int, int, double, k_vector, ...) as |
|          | in the call of  a function over this domain.                       |
+----------+--------------------------------------------------------------------+

* **Examples** :

 * Matsubara time
 * Matsubara frequencies (boson/fermion): in this case, point_t is `matsubara_freq`, a simple type containing (n, beta, statistics).
 * Real frequencies
 * Real time
 * Brillouin zone
 * Cartesian product of previous domains to build multi-variable functions.

.. _Concept_PureFunctionOnDomain:

PureFunctionOnDomain
-----------------------

* **Purpose** :
   A mathematical (pure) function from a domain to a target space.
       * it has a domain of definition
       * it can be called on any point of the domain, as a *pure* function, i.e. without any side effect.

* **Refines**   :

* **Definition**:

+--------------------------------------+----------------------------------------------------------+
| Elements                             | Comment                                                  |
+======================================+==========================================================+
| domain_t const & domain() const      | Returns the domain (deduced as domain_t)                 |
+--------------------------------------+----------------------------------------------------------+
| operator (domain_t::point_t) const   | Calling for all elements of the Domain (including infty  |
|                                      | if it is in the domain...                                |
+--------------------------------------+----------------------------------------------------------+

* NB: Note that the return type of the function is *NOT* part of the concept,
  it has to be deduced by the compiler (using C++11 decltype, std::result_of, eg..).


.. _Concept_Mesh:

Mesh
-------------------------------------------------

* **Purpose** : A mesh over a domain, and more generally the practical representation of the domain in a computer.
  It does not really need to be a mesh: e.g. if the function is represented on a polynomial basis,
  it is the parameters of this representation (max number of coordinates, e.g.)

* **Refines**: RegularType,  H5-serializable

* **Definition**:

+-------------------------------------------------------+-------------------------------------------------------------------------------+
| Elements                                              | Comment                                                                       |
+=======================================================+===============================================================================+
| domain_t                                              | Type of the Domain represented, modeling the Domain concept                   |
+-------------------------------------------------------+-------------------------------------------------------------------------------+
| domain_t const & domain() const                       | Access to the domain                                                          |
+-------------------------------------------------------+-------------------------------------------------------------------------------+
| index_t                                               | Type of indices of a point on the grid. Typically a tuple of long or a long   |
+-------------------------------------------------------+-------------------------------------------------------------------------------+
| long size() const                                     | The number of points in the mesh.                                             |
+-------------------------------------------------------+-------------------------------------------------------------------------------+
| domain_t::point_t index_to_point(index_t) const       | From the index of a mesh point, compute the corresponding point in the domain |
+-------------------------------------------------------+-------------------------------------------------------------------------------+
| long index_to_linear(index_t const &) const           | Flattening the index of the mesh into a contiguous linear index               |
+-------------------------------------------------------+-------------------------------------------------------------------------------+
| mesh_point_t                                          | A type modeling MeshPoint concept (see below).                                |
+-------------------------------------------------------+-------------------------------------------------------------------------------+
| mesh_point_t operator[](index_t const & index ) const | From an index, return a mesh_point_t containing this a ref to this mesh and   |
|                                                       | the index.                                                                    |
+-------------------------------------------------------+-------------------------------------------------------------------------------+
| mesh_pt_generator<mesh_t> const_iterator              | A generator of all the mesh points.                                           |
+-------------------------------------------------------+-------------------------------------------------------------------------------+
| const_iterator begin()/end() const                    | Standard access to iterator on the mesh Standard access to iterator on the    |
| cbegin()/cend() const                                 | mesh                                                                          |
+-------------------------------------------------------+-------------------------------------------------------------------------------+


+---------------------------+-----------------------------------------------------------+
| Free functions            | Comment                                                   |
+===========================+===========================================================+
| void  foreach (mesh_t, F) | If F is a function of synopsis                            |
|                           | auto F( mesh_t::mesh_point_t)                             |
|                           | it calls F for each point on the mesh, in arbitrary order |
+---------------------------+-----------------------------------------------------------+

.. _Concept_MeshPoint:

MeshPoint
-------------------------------------------------

* **Purpose** : Abstraction of a point on a mesh. A little more than a ref to the mesh and a index.

* **Refines**:  CopyConstructible.

* **Definition**:

+------------------------------------------------+--------------------------------------------------------------------+
| Elements                                       | Comment                                                            |
+================================================+====================================================================+
| mesh_t                                         | Type of the mesh                                                   |
+------------------------------------------------+--------------------------------------------------------------------+
| mesh_t const * m                               | A pointer to the mesh to which the point belongs.                  |
+------------------------------------------------+--------------------------------------------------------------------+
| mesh_t::index_t index                          | The index of the point                                             |
+------------------------------------------------+--------------------------------------------------------------------+
| mesh_point_t( mesh_t const &, index_t const &) | Constructor: a mesh point at the given index                       |
+------------------------------------------------+--------------------------------------------------------------------+
| mesh_point_t( mesh_t const &)                  | Constructor: the first mesh point                                  |
+------------------------------------------------+--------------------------------------------------------------------+
| mesh_t::index_t [const &,] index() const       | The index corresponding to the point                               |
+------------------------------------------------+--------------------------------------------------------------------+
| size_t linear_index() const                    | The linear index of the point (same as m->index_to_linear(index()) |
+------------------------------------------------+--------------------------------------------------------------------+
| void advance()                                 | Advance to the next point on the mesh (used by iterators).         |
+------------------------------------------------+--------------------------------------------------------------------+
| void at_end()                                  | Is the point at the end of the grid                                |
+------------------------------------------------+--------------------------------------------------------------------+
| void reset()                                   | Reset the mesh point to the first point                            |
+------------------------------------------------+--------------------------------------------------------------------+
| cast_t operator cast_t() const                 | == mesh_t::domain_t::point_t *implicit* cast to the corresponding  |
|                                                | domain point                                                       |
+------------------------------------------------+--------------------------------------------------------------------+

For one dimensional mesh, we also require that the MeshPoint implement the basic arithmetic operations
using the cast.

* **Discussion**:

A MeshPoint is just an index of a point on the mesh, and containers like gf can easily be overloaded for this type
to have a direct access to the grid (Cf [] operator of gf).

However, since the MeshPoint can be implicitely casted into the domain point, simple
expression like  ::

  g[p] = 1/ (p +2)

make sense and fill the corresponding point wiht the evaluation of 1/ (p+2) in the domain.

As a result, because iterating on a mesh result in a series of object modelling MeshPoint,
one can write naturally ::

    // example of g, a Green function in Matsubara frequencies w
    for (auto w: g.mesh())
       g[w] = 1/(w + 2)
    // This runs overs the mesh, and fills the function with 1/(w+2)
    // In this expression, w is casted to the domain_t::point_t, here a complex<double>
    // which allows to evaluate the function

