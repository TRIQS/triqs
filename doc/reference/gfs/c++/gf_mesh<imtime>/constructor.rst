..
   Generated automatically using the command :
   c++2doc.py -N triqs all_triqs.hpp
   /home/tayral/Work/TRIQS_1.4/install/include/triqs/./gfs/./meshes/matsubara_time.hpp

.. highlight:: c


.. _gf_mesh<imtime>_constructor:

constructor
=============

**Synopsis**:

.. code-block:: c

    gf_mesh<imtime> () ;                                                                 (1)

    gf_mesh<imtime> (const triqs::gfs::gf_mesh<triqs::gfs::imtime> & x) ;                (2)

    gf_mesh<imtime> (matsubara_time_domain d, int n_time_slices) ;                       (3)

    gf_mesh<imtime> (double beta, triqs::gfs::statistic_enum S, int n_time_slices) ;     (4)

(1)
(2)
(3)  constructor
(4)  constructor

Parameters
-------------


 (1)


 (2)


 (3)


   * **d**: domain




   * **n_time_slices**: number of time slices



 (4)


   * **beta**: inverse temperature




   * **S**: statistic (Fermion/Boson)




   * **n_time_slices**: number of time slices



Return value
--------------

None