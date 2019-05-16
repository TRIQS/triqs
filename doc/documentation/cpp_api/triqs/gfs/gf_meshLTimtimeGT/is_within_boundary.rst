..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__gf_meshLTimtimeGT__is_within_boundary:

triqs::gfs::gf_mesh<imtime>::is_within_boundary
===============================================

*#include <triqs/gfs/gf_mesh.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | bool :red:`is_within_boundary` (double :param:`x`) const





For imtime the point is always in the mesh, since we use anti-periodicity or periodicity. Needed for cartesian product.