.. highlight:: c

.. _gf_subscript:

Operator []
==================================

**Synopsis** ::

  value_type const & operator[](mesh_t::index_t const &) const          (1a)
  value_type       & operator[](mesh_t::index_t const &)                (1b)

  value_type const & operator[](mesh_t::mesh_point_t const &) const     (2a)
  value_type       & operator[](mesh_t::mesh_point_t const &)           (2b)

  value_type const & operator[](closest_mesh_pt_t const &) const        (3a)
  value_type       & operator[](closest_mesh_pt_t const &)              (3b)

  ClefExpression  operator[](ClefExpression expr) const                 (4)

The operator [] is in charge of accessing the data of the Green function
on the mesh. This is therefore a way to modify the Green function.

This is valid for both the container (e.g. gf), and the view (e.g. gf_view).

Note that the :ref:`gf_mesh\<prod\<X,Y,...\>\> <gf_product>` provides
additional functionality for the operator [].


.. _gf_subscript_index:

(1) Use the linear index of the mesh
-----------------------------------------

Each mesh_t has an associated index_t (e.g. the Matsubara index (long) for gf_mesh<imfreq>)
that can be used to access the data of the Green function.
Please refer to the respective mesh documentation page for the index_t and meaning.

.. _gf_subscript_mesh_point:

(2) Use the mesh_point of the mesh
-----------------------------------------

Each mesh_t has further an associated mesh_point_t that can be used to access the data.
These mesh points are naturally acquired when looping over a mesh

.. code::

        for( auto const & iw : g_iw.mesh() )
          g_iw[iw] = 1.0 / iw;

Note however that this will not set a proper singularity of the Green function.


.. _gf_subscript_closest:
..

(3) Find the closest mesh point from a domain point
---------------------------------------------------------

* May or may not be provided by the specialisation. Cf specialization.

* This is normally used with the helper function::

   closest_mesh_point_t closest_mesh_point(Anything x)

  closest_mesh_point_t is just a little "vehicle" that can contain anything (by copy or ref ???),
  and carry it through the [].

* Usage ::

     g [ closest_mesh_point(x) ] += y;


.. _gf_subscript_lazy:

(4) Interaction with clef expressions
-------------------------------------------------

Like (), [] can be called with a ClefExpression.






