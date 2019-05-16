..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__mesh_pointLTgf_meshLTcartesian_productLTVs___GTGTGT__advance:

triqs::gfs::mesh_point<gf_mesh<cartesian_product<Vs...>>>::advance
==================================================================

*#include <triqs/gfs/mesh_point.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | void :red:`advance` ()





index[0] +=1; if index[0]==m.component[0].size() { index[0]=0; index[1] +=1; if  ....}  and so on until dim