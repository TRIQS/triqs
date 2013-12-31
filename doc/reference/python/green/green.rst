
.. index:: Green's functions

.. module:: pytriqs.gf.local

.. _green:


Green's functions
=================

Green's functions are among the most important objects when dealing with
many-body problems. TRIQS makes it easy for you to manipulate them. Before
starting with some example, let us readily emphasize the essential notion
that Green's functions have a **block** structure:

.. math::

  G^a_{\alpha_a \beta_a} (\tau) = - \mathcal{T} <c^a_{\alpha_a}(\tau) c^{a \dagger}_{\beta_a} (0)>

Here, :math:`a` labels a block, while :math:`\alpha_a, \beta_a` are indices
**within** the block :math:`a`. In other words, there can be off-diagonal
elements within a block but not between blocks.  Whenever possible, one should
take advantage of the symmetries of the problem under consideration and define
blocks for the Green's functions.

In this documentation, we will start with the block Green's function, 
and then proceed with the general Green's function and its block structure.


.. toctree::
  :maxdepth: 2

  tutorial
  block
  transforms
  full
  tail
