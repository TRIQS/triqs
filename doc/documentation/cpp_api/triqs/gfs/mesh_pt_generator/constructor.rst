..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _mesh_pt_generator_constructor:

triqs::gfs::mesh_pt_generator::constructor
==========================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`do NOT check = of mesh, otherwise e.g. block iterator does not work (infinite loop...)`
       | :red:`mesh_pt_generator` ()

    2. | :red:`mesh_pt_generator` (const MeshType * :param:`m`, bool :param:`atEnd` = false)

Documentation



 **1)**         bool equal(mesh_pt_generator const & other) const { return ((mesh == other.mesh) && (other.u==u) );}