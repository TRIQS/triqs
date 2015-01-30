
Manipulating local Green's functions 
------------------------------------

Let us start with a problem of free electrons: an impurity `d`
level embedded in a flat conduction bath :math:`\Delta` of `s`-electrons.
To construct and plot the corresponding 2x2 Green's function:

.. math::

  \hat{G}^\mathrm{s+d} ( \omega) = \begin{pmatrix} \omega - \epsilon_d & t \\ t & \Delta^{-1} \end{pmatrix}^{-1}

we first create the corresponding energy
mesh on the real axis in the energy interval :math:`\omega \in [-2,2]`.
The Green's function is generated using classes of the ``gf.local`` module by setting up
:math:`\left[\hat{G}^\mathrm{s+d}\right]^{-1}` and inverting it.
Finally, the obtained bath and impurity densities of states are plotted using the TRIQS function ``oplot``: 

.. plot:: tutorials/gfs/impinbath.py
   :include-source:
   :scale: 70

You will find more information on the local Green's function implementation in TRIQS in :doc:`the corresponding chapter of the manual <../reference/gfs/py/contents>`.
