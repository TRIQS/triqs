..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _histogram_mesh_point:

triqs::statistics::histogram::mesh_point
========================================


**Synopsis**

 .. rst-class:: cppsynopsis

    1. | :cppbrief:`Get position of bin's center`
       | double :red:`mesh_point` (int :param:`n`) const







Parameters
^^^^^^^^^^

 * **n**: Bin index


Returns
^^^^^^^

 * Position of the center, :math:`n (b - a) / (n_\mathrm{bins} - 1)`