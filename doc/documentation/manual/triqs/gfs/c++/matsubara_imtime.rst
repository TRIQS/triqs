..
   Generated automatically using the command:
   c++2doc.py -N triqs all_triqs.hpp

.. highlight:: c

.. _gf_mesh_imtime:


mesh::imtime
=================

**Synopsis**:

.. code-block:: c

      class mesh::imtime;

Imaginary-time mesh

  Mesh for the imaginary-time axis from :math:`0` to :math:`\beta`.
  The time points are defined as follows, for :math:`n=0{\dots}N-1`:

.. math::
		\tau_n=\frac{n}{N-1}\beta

..
 where :math:`N` is the size of the mesh.

.. figure:: matsubara_imtime.png
   :alt:  Pictorial representation of ``mesh::imtime{beta, Fermion/Boson, 4}``.
   :align: center

   Pictorial representation of ``mesh::imtime{beta, Fermion/Boson, 4}``.



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
| :ref:`constructor <mesh::imtime_constructor>`                          |                                                                        |
+------------------------------------------------------------------------+------------------------------------------------------------------------+
| :ref:`get_interpolation_data <mesh::imtime_get_interpolation_data>`    | redefine the interpolation to handle the anti-periodicity of fermions. |
+------------------------------------------------------------------------+------------------------------------------------------------------------+
| :ref:`evaluate <mesh::imtime_evaluate>`                                | evaluation                                                             |
+------------------------------------------------------------------------+------------------------------------------------------------------------+

.. toctree::

   :hidden:

   mesh::imtime/constructor
   mesh::imtime/get_interpolation_data
   mesh::imtime/evaluate
