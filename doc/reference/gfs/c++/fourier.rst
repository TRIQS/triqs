.. highlight:: c

Fourier transforms
###################

Matrix-valued
==============

Synopsis
----------

Here is a synopsis::

 auto fourier (gf<imtime,Target,Opt>            const &)
 auto fourier (gf_view<imtime,Target,Opt>       const &)
 auto fourier (gf_const_view<imtime,Target,Opt> const &)

 auto inverse_fourier (gf<imfreq,Target,Opt>            const &)
 auto inverse_fourier (gf_view<imfreq,Target,Opt>       const &)
 auto inverse_fourier (gf_const_view<imfreq,Target,Opt> const &)

 gf<imfreq, Target, Opt> make_gf_from_fourier(gf<imtime, Target, Opt>            const&);
 gf<imfreq, Target, Opt> make_gf_from_fourier(gf_view<imtime, Target, Opt>       const&);
 gf<imfreq, Target, Opt> make_gf_from_fourier(gf_const_view<imtime, Target, Opt> const&);

 gf<imfreq, Target, Opt> make_gf_from_fourier(gf<imtime, Target, Opt>            const&, int n_iw);
 gf<imfreq, Target, Opt> make_gf_from_fourier(gf_view<imtime, Target, Opt>       const&, int n_iw);
 gf<imfreq, Target, Opt> make_gf_from_fourier(gf_const_view<imtime, Target, Opt> const&, int n_iw);
  
 gf<imtime, Target, Opt> make_gf_from_inverse_fourier(gf<imfreq, Target, Opt>            const&);
 gf<imtime, Target, Opt> make_gf_from_inverse_fourier(gf_view<imfreq, Target, Opt>       const&);
 gf<imtime, Target, Opt> make_gf_from_inverse_fourier(gf_const_view<imfreq, Target, Opt> const&);

 gf<imtime, Target, Opt> make_gf_from_inverse_fourier(gf<imfreq, Target, Opt>            const&, int n_tau);
 gf<imtime, Target, Opt> make_gf_from_inverse_fourier(gf_view<imfreq, Target, Opt>       const&, int n_tau);
 gf<imtime, Target, Opt> make_gf_from_inverse_fourier(gf_const_view<imfreq, Target, Opt> const&, int n_tau);


fourier, inverse_fourier
---------------------------

The fourier/inverse_fourier functions do **not** perform the Fourier transformation, 
but returns a small lazy object (basically saying "Fourier Transform of XXX"), 
which is then used in an assignment of a *view* of a gf.

The reason is the following: when putting e.g. a Fourier transform of a function in time, say gt, 
into a Green function in frequencies, say gw, we want to say something like::

  gw = fourier(gt); // ??? (1)

However, if the fourier function performs the transformation, how could it know the details
of the mesh of gw ? That information is not available when calling *fourier*.

Since *fourier* returns a small lazy object, the library can then rewrite (1) internally into something like ::

  call_the_fourier_implementation(gt, gw);

where all the information about the mesh of gw is now available to the implementation.

Moreover, since fourier(gt) does not possess a domain (for the same reason), (1)
makes no sense: RHS of gf assignment requires a domain (cf concepts).
We therefore use *a view* as LHS::

   gw() = fourier(gt); // correct usage.

**make_gf_from_fourier, make_gf_from_inverse_fourier**

In the case where we want to create a *new* container from the fourier transform of gt, 
we can use the function make_gf_from_fourier.

Example
----------

.. triqs_example:: ./fourier_0.cpp


.. toctree::
   :maxdepth: 1

   fourier_impl_notes


Tensor-valued
==================

.. code-block:: c

    #include <../../triqs/gfs/functions/fourier_tensor.hpp>

fourier
---------

**Synopsis**:

.. code-block:: c

    gf<triqs::gfs::imfreq, tensor_valued<3>, triqs::gfs::nothing> fourier
   (gf_const_view<triqs::gfs::imtime, tensor_valued<3>, triqs::gfs::nothing> g_in = 3,
   array_const_view<triqs::gfs::tail, 3> tail = 3, int n_pts = 1025) ;

Fourier transform of tensor-valued functions

Parameters
-------------

* **g_in**: input Green's function :math:`g_{abc}(\tau)`

* **tail**: singularity of the input Gf.

* **n_pts**: defined as n_pts = n_max +1 where n_max is the maximal matsubara index


Return value
--------------

:math:`g_{abc}(i\omega)`



.. warning::
     tail not used

.. code-block:: c

    #include <../../triqs/gfs/functions/fourier_tensor.hpp>

inverse_fourier
------------------

**Synopsis**:

.. code-block:: c

    gf<triqs::gfs::imtime, tensor_valued<3>, triqs::gfs::nothing> inverse_fourier
   (gf_const_view<triqs::gfs::imfreq, tensor_valued<3>, triqs::gfs::nothing> g_in = 3,
   array_const_view<triqs::gfs::tail, 3> tail = 3, int n_tau = 1025) ;

Inverse Fourier transform of tensor-valued functions

Parameters
-------------


Return value
--------------

:math:`g_{abc}(\tau)`


Two-variable functions
#########################

.. code-block:: c

    #include <../../triqs/gfs/functions/fourier_two_variables.hpp>

.. __fourier:

fourier
==========

**Synopsis**:

.. code-block:: c

    gf<cartesian_product<triqs::gfs::imfreq, triqs::gfs::imfreq>, tensor_valued<3> >
   fourier (gf_const_view<cartesian_product<triqs::gfs::imtime,
   triqs::gfs::imtime>, tensor_valued<3> > g2t = 3, int n_w_1, int n_w_2, bool
   positive_matsub_only_1 = true, bool positive_matsub_only_2 = true) ;

FT from  :math:`g_{uvw}(\tau,\tau')` to :math:`g_{uvw}(i\omega,i\Omega)`

   Fourier transform from  :math:`g_{uvw}(\tau,\tau')` to :math:`g_{uvw}(i\omega,i\Omega)` :


.. math::
		g_{uvw}(i\omega,i\Omega)=\iint_{0}^{\beta}d{\tau}d\tau'e^{i\omega\tau+i\Omega\tau'}g_{uvw}(\tau,\tau')

..

   In two steps:
#. Fourier transform from  :math:`g_{uvw}(\tau,\tau')` to  :math:`g_{uvw}(i\omega,\tau')`
#. Fourier transform from  :math:`g_{uvw}(i\omega,\tau')` to  :math:`g_{uvw}(i\omega,i\Omega)`

Parameters
-------------

* **g2t**: :math:`g_{uvw}(\tau,\tau')`

* **n_w_1**: defined as n_w_1 = n_max + 1 (n_max: max Matsubara index)

* **n_w_2**: defined as n_w_2 = n_max + 1 (n_max: max Matsubara index

* **positive_matsub_only_1**: if true only positive fermionic Matsubara frequencies

* **positive_matsub_only_2**: if true only positive bosonic Matsubara frequencies


Return value
--------------

:math:`g_{uvw}(i\omega,i\Omega)`

.. code-block:: c

    #include <../../triqs/gfs/functions/fourier_two_variables.hpp>

.. __inverse_fourier:

inverse_fourier
==================

**Synopsis**:

.. code-block:: c

    gf<cartesian_product<triqs::gfs::imtime, triqs::gfs::imtime>, tensor_valued<3> >
   inverse_fourier (gf_const_view<cartesian_product<triqs::gfs::imfreq,
   triqs::gfs::imfreq>, tensor_valued<3> > g2w = 3, int n_t_1, int n_t_2, bool fit_tails =
   false) ;

FT from :math:`g_{uvw}(i\omega,i\Omega)` to :math:`g_{uvw}(\tau,\tau')`

 Fourier Transform from :math:`g_{uvw}(i\omega,i\Omega)` to :math:`g_{uvw}(\tau,\tau')` :


.. math::
		g_{uvw}(\tau,\tau')=\frac{1}{\beta^2}\sum_{n=n_\mathrm{min}}^{n_\mathrm{n_max}}\sum_{m=n_\mathrm{min}}^{n_\mathrm{max}}e^{-i\omega_n\tau-i\Omega_m\tau'}g_{uvw}(i\omega_n,i\Omega_m)

..

   In two steps:
#. Fourier transform from  :math:`g_{uvw}(i\omega,i\Omega)` to  :math:`g_{uvw}(\tau,i\Omega)`
#. Fourier transform from :math:`g_{uvw}(\tau,i\Omega)` to  :math:`g_{uvw}(i\omega,i\Omega)`

Parameters
-------------

* **g2w**: :math:`g_{uvw}(i\omega,i\Omega)`

* **n_t_1**: number of imaginary time points

* **n_t_2**: number of imaginary time points


Return value
--------------

:math:`g_{uvw}(i\tau,i\tau')`




.. warning::
     fitting the tails!
