.. highlight:: c

.. _gf_data:

Data access
====================

mesh
------------

* **Synopsis**:

  .. cpp:function:: mesh_t const & () const


  Access to the mesh of the green function.

data
------------

* **Synopsis**:

  .. cpp:function:: data_t const & () const
  .. cpp:function:: data_t       & ()


  Access to the data array where the value of the gf is stored on the mesh points.

singularity
------------

* **Synopsis**:

  .. cpp:function:: singularity_t const & () const
  .. cpp:function:: singularity         & ()


  Access to the singularity type

symmetry
------------

* **Synopsis**:

  .. cpp:function:: symmetry_t const & () const

  Access to the symmetry type. Cannot be modifed after construction. *(why ?)*.


