
.. index:: Green's functions

.. _green:

.. warning:: THIS IS BEING REWRITTEN.


Basic notions
##############

Green's functions are among the most important objects when dealing with
many-body problems. TRIQS makes it easy for you to manipulate them. Before
starting with some example, let us readily emphasize the essential notion
that Green's functions have a **block** structure:

.. math::

  G^a_{\alpha_a \beta_a} (\tau) = - \mathcal{T} \langle c^a_{\alpha_a}(\tau) c^{a \dagger}_{\beta_a} (0)\rangle.

Here, :math:`a` labels a block, while :math:`\alpha_a, \beta_a` are indices
**within** the block :math:`a`. In other words, there can be off-diagonal
elements within a block but not between blocks.  Whenever possible, one should
take advantage of the symmetries of the problem under consideration and define
blocks for the Green's functions.


Types of Green's functions
==========================

:ref:`Matsubara Green's function in imaginary time <GfImTime>` <GfImTime>
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This object stores a matrix-valued Green function in imaginary time
in the Matsubara formalism:

.. math::

  G_{\alpha \beta} (\tau) \equiv - \mathcal{T} \langle c_{\alpha}(\tau) c^{\dagger}_{\beta} (0)\rangle \qquad \text{ for } 0 \leq \tau \leq \beta


:ref:`Matsubara Green's function in imaginary frequencies <GfImFreq>` <GfImFreq>
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This object stores a matrix-valued Green function in imaginary frequencies
in the Matsubara formalism:

.. math::
  G_{\alpha \beta} (i \omega_n) \equiv \int_0^\beta G_{\alpha \beta} ( \tau) e^{i \omega_n \tau}

where :math:`G(\tau)` is the Matsubara Green function in imaginary time <GfImTime>.


:ref:`Matsubara Green's function with Legendre polynomials <GfLegendre>` <GfLegendre>
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This object stores a matrix-valued Matsubara Green's function in imaginary time using
Legendre polynomials. It is connected to the usual imaginary-time Matsubara Green's
function through:

.. math::
  G_{l, \alpha \beta} = \sqrt{2l+1} \int_0^\beta d\tau P_l (x(\tau)) G_{\alpha \beta}(\tau),


where :math:`x(\tau) = 2\tau/\beta -1` and the :math:`P_l` are the Legendre
polynomials.  :math:`G_{\alpha \beta}(\tau)` is the Matsubara Green's function in imaginary time <GfImTime>.


:ref:`Green function in real time <GfReTime>` <GfReTime>
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This object stores a matrix-valued Green function in real time, for example a retarded GF:

.. math::
  G_{\alpha \beta} (t) \equiv - \theta(t) \langle\{c_{\alpha}(t) c^{\dagger}_{\beta} (0)\}\rangle


:ref:`Retarded Green's function in real frequencies <GfReFreq>` <GfReFreq>
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This object stores a matrix-valued Green function in real frequencies:

.. math::
  G_{\alpha \beta} (\omega) \equiv \int_{-\infty}^{\infty} G_{\alpha \beta} ( t ) e^{-i \omega t}

where :math:`G_{\alpha \beta}(t)` is the Green function in real time <GfReTime>.



Fourier and Legendre transforms
===============================

The Fourier transforms from real and imaginary frequencies to times, and inverse, are currently implemented,
along with the analogous transformation from the Legendre expansion to imaginary time and frequencies.

For real time/frequency:

    .. math:: \tilde G(\omega)=\int_{-\infty}^\infty dt G(t)e^{i\omega t}

    .. math:: G(t)=\int_{-\infty}^\infty \frac{d\omega}{2\pi} \tilde G(\omega)e^{-i\omega t}

For Matsubara (imaginary) time/frequency:

    .. math:: \tilde G(i\omega_n)=\int_{0}^\beta d\tau G(t)e^{i\omega_n \tau}

    .. math:: G(\tau)=\sum_{n=-\infty}^\infty \frac{1}{\beta} \tilde G(i\omega_n)e^{-i\omega_n \tau}

The :math:`\omega_n`'s are :math:`\frac{(2n+1)\pi}{\beta}` for fermions, :math:`\frac{2n\pi}{\beta}` for bosons (as :math:`G(\tau+\beta)=-G(\tau)` for fermions, :math:`G(\tau)` for bosons).



High-frequency tail
===========================

The high-frequency behavior (or **tail**) of a Green's function is of paramount importance in the context of Fourier transformations: since fermionic Green's functions are slowly decaying functions of frequency,
special care has to be taken when Fourier-transforming them to time.

The "tail" of a Green's function encodes the behavior of the Green's
function :math:`G` at large Matsubara frequencies, namely

.. math::  \mathbf{G}(i\omega_n) \stackrel {=}{\infty} \mathbf{a}_{0} +\mathbf{a}_{1}\cdot \frac{1}{ i\omega_n} +\mathbf{a}_{2}\cdot \frac{1}{ (i\omega_n)^2} +\dots


Generically, the tail is parametrized by matrix-valued coefficients
:math:`\mathbf{a}_{i}` (of size :math:`N_1\times N_2`\ )

.. math:: t = \sum_{i=o_{min}}^{o_{max}} \mathbf{a}_i (i\omega_n)^{-i}

