.. _detmanip:

Manipulations of determinants
=============================

.. warning::

   This library is stable, but documentation is currently being written
   and needs a serious rereading and cleaning

The purpose of this little class is to regroup standard block manipulations on determinant, used in several algorithms.

Given a function :math:`F(x,y)`, and two sets of values :math:`x_i,y_i \ 0\leq i < N`,
we can define the :math:`N\times N` square matrix

.. math::

   M_{i,j} = F(x_i,y_j)

When adding/removing a line and column (i.e. a value of x, y),
:math:`M^{-1}` and :math:`det M` can be fast updated using standard block matrix computations.
This class implements these general operations. It contains:

* Datas:

  * :math:`M^{-1}` and :math:`det M`
  * a vector containing  :math:`x_i,y_i \ 0\leq i \leq N`

* Methods to quickly update :math:`M^{-1}` and :math:`\det M` when one:

  * adds/removes a line and a column (i.e. adding or removing one x and one y)
  * adds/removes two lines and two columns (i.e. adding or removing two x and two y)
  * changes a line/colum, etc...



.. toctree::
   :maxdepth: 1

   ../../generated/triqs/det_manip
   det_manip
   implementation_notes
   ./../../../userguide/c++/det_manip/tutorial
