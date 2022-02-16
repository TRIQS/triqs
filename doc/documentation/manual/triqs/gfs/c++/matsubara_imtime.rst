..
   Generated automatically using the command:
   c++2doc.py -N triqs all_triqs.hpp

.. highlight:: c

.. _gf_mesh_imtime:


gf_mesh<imtime>
=================

**Synopsis**:

.. code-block:: c

      class gf_mesh<imtime>;

Imaginary-time mesh

  Mesh for the imaginary-time axis from :math:`0` to :math:`\beta`.
  The time points are defined as follows, for :math:`n=0{\dots}N-1`:

.. math::
		\tau_n=\frac{n}{N-1}\beta

..
 where :math:`N` is the size of the mesh.

.. figure:: matsubara_imtime.png
   :alt:  Pictorial representation of ``gf_mesh<imtime>{beta, Fermion/Boson, 4}``.
   :align: center

   Pictorial representation of ``gf_mesh<imtime>{beta, Fermion/Boson, 4}``.



Member types
-----------------

+-------------+---------+
| Member type | Comment |
+=============+=========+
| B           |         |
+-------------+---------+

Member functions
-----------------

+------------------------------------------------------------------------+------------------------------------------------------------------------+
| Member function                                                        | Comment                                                                |
+========================================================================+========================================================================+
| :ref:`constructor <gf_mesh<imtime>_constructor>`                       |                                                                        |
+------------------------------------------------------------------------+------------------------------------------------------------------------+
| :ref:`get_interpolation_data <gf_mesh<imtime>_get_interpolation_data>` | redefine the interpolation to handle the anti-periodicity of fermions. |
+------------------------------------------------------------------------+------------------------------------------------------------------------+
| :ref:`evaluate <gf_mesh<imtime>_evaluate>`                             | evaluation                                                             |
+------------------------------------------------------------------------+------------------------------------------------------------------------+

.. toctree::

   :hidden:

   gf_mesh<imtime>/constructor
   gf_mesh<imtime>/get_interpolation_data
   gf_mesh<imtime>/evaluate
