..
   Generated automatically using the command :
   c++2doc.py -N triqs all_triqs.hpp
   /home/tayral/Work/TRIQS_1.4/install/include/triqs/./gfs/./meshes/matsubara_freq.hpp

.. highlight:: c


.. _gf_mesh<imfreq>_constructor:

constructor
=============

**Synopsis**:

.. code-block:: c

    gf_mesh<imfreq> (domain_t dom, long n_pts = 1025, triqs::gfs::matsubara_mesh_opt opt) ;                                      (1)

    gf_mesh<imfreq> () ;                                                                                                         (2)

    gf_mesh<imfreq> (double beta, triqs::gfs::statistic_enum S, long n_pts = 1025,
   triqs::gfs::matsubara_mesh_opt opt) ;     (3)

(1)  constructor

 Full constructor
(2)  default constructor
(3)  constructor

 Full constructor

Parameters
-------------


 (1)


   * **dom**: domain




   * **n_pts**: defined as n_pts = n_max + 1 (n_max: last matsubara index)




   * **matsubara_mesh_opt**: tells whether the mesh is defined for all frequencies or only for positive frequencies



 (2)


 (3)


   * **beta**: inverse temperature




   * **S**: statistic (Fermion or Boson)




   * **n_pts**: defined as n_pts = n_max + 1 (n_max: last matsubara index)




   * **matsubara_mesh_opt**: tells whether the mesh is defined for all frequencies or only for positive frequencies



Return value
--------------

None