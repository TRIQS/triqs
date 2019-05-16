..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__mesh_pt_generator__constructor:

triqs::gfs::mesh_pt_generator::constructor
==========================================

*#include <triqs/gfs/mesh_pt_generator.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :red:`mesh_pt_generator` ()

    2. | :red:`mesh_pt_generator` (const MeshType * :param:`m`, bool :param:`atEnd` = false)

Documentation



 **1)**   do NOT check = of mesh, otherwise e.g. block iterator does not work (infinite loop...)
     //bool equal(mesh_pt_generator const & other) const { return ((mesh == other.mesh) && (other.u==u) );}