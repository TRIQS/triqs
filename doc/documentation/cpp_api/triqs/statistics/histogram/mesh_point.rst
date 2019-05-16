..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__statistics__histogram__mesh_point:

triqs::statistics::histogram::mesh_point
========================================

*#include <triqs/statistics/histogram.hpp>*



**Synopsis**

 .. rst-class:: cppsynopsis

       | double :red:`mesh_point` (int :param:`n`) const





Get position of bin's center





Parameters
^^^^^^^^^^

 * :param:`n` Bin index


Returns
^^^^^^^

Position of the center, :math:`n (b - a) / (n_\mathrm{bins} - 1)`