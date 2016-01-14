..
   Generated automatically using the command :
   c++2doc.py -N triqs all_triqs.hpp
   /home/tayral/Work/TRIQS_1.4/install/include/triqs/./gfs/./meshes/matsubara_time.hpp

.. highlight:: c


.. _gf_mesh<imtime>_get_interpolation_data:

get_interpolation_data
========================

**Synopsis**:

.. code-block:: c

    B::interpol_data_t get_interpolation_data (interpol_t::Linear1d , double tau) const;

redefine the interpolation to handle the anti-periodicity of fermions.
 It also reduces tau to [0,beta]
 because the get_interpolation_data is not virtual, to preserve inlining
 I simply rewrite evaluate again, it is short (same code as linear).
 it is necessary to do it as the level of the get_interpolation_data
 in order e.g. g(x,tau) to behave properly

Parameters
-------------


Return value
--------------

None