
Multivariable Green's functions
-------------------------------

.. warning::
   This part of the library is largely experimental and subject to API breaks

TRIQS comes with multivariable Green's functions such as
:math:`G_{\sigma,\sigma',I}(i\omega,i\Omega)` or
:math:`\gamma(i\omega,i\omega',i\Omega)`.

They come with properties similar to the single-variable Greens
functions, namely: 

 - they have a **mesh** (which is the cartesian product of two one-dimensional meshes) 

 - they have **data** (a numpy array of shape ``dim_var1 x dim_var2 x ... x dim_target_1 x dim_target_2 x ...``) 

 - they can be stored in a **HDF5 archive** 
  
 - they can be **broadcast**, **reduced**... 

 - they can be **sliced in the target space**, one can access e.g. the element :math:`G_{\uparrow,\downarrow,z}(i\omega,i\Omega)` in the example above

The main additional feature is the ability to slice the Greens function
with respect to its frequency arguments. This is done via the functions
**``slice_at_const_w1(n)``**, **``slice_at_const_w2(n)``**... where n is
the Matsubara index of the first, second, ... argument, respectively.

For instance, if G is the object representing
:math:`\gamma(i\omega,i\omega',i\Omega)`,
**``G.slice_at_const_w3(10).slice_at_const_w2(4)``** is the function
:math:`i\omega_n \rightarrow \gamma(i\omega_n,i\omega'_4,i\Omega_{10})`

The construction and use of these functions is illustrated for the
following vertex function:

.. math:: G^{\eta=\mathrm{ch},\mathrm{sp}}_{\sigma,\sigma',I}(i\omega,i\Omega) = \frac{U^2}{4}\frac{1}{(i\omega+i\Omega)(i\omega)}

This is a block function whose two blocks are defined on a product of
two Matsubara frequency meshes, and are tensor-valued of rank 3: the
blocks are encoded by objects of the type **``GfImFreq_x_ImFreqTv3``**.

.. plot:: userguide/gfs/multivar.py
   :include-source:
   :scale: 70

