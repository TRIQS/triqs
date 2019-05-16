..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__foreach:

triqs::gfs::foreach
===================

*#include <triqs/gfs.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :green:`template<typename Lambda>`
       | void :red:`foreach` (:ref:`gf_mesh\<imtime\> <triqs__gfs__gf_meshLTimtimeGT>` const & :param:`m`, Lambda :param:`F`)

    2. | :green:`template<typename Lambda>`
       | void :red:`foreach` (:ref:`gf_mesh\<imfreq\> <triqs__gfs__gf_meshLTimfreqGT>` const & :param:`m`, Lambda :param:`F`)

    3. | :green:`template<typename Lambda>`
       | void :red:`foreach` (:ref:`segment_mesh <triqs__gfs__segment_mesh>` const & :param:`m`, Lambda :param:`F`)

Documentation



 **1)**   foreach for this mesh



 **2)**   \brief foreach for this mesh



 **3)**   \brief foreach for this mesh





Parameters
^^^^^^^^^^

 * :param:`m` a mesh

 * :param:`F` a function of synopsis auto F (matsubara_time_mesh::mesh_point_t)

  Calls F on each point of the mesh, in arbitrary order.