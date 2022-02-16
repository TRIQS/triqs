.. _gf_tail:

High-Frequency moments of the Green's function
##############################################

The coefficients of the high-frequency expansion of a Green function
(in short tail) play an essential role in acquiring accurate results
for e.g. Fourier transforms and density calculations.

.. math::

   G(i\omega) \approx \sum_{n = 0}^N \frac{a_n}{(i\omega)^n}

The TRIQS library provides functionalities to determine these tail coefficiencts
through a least-square fitting procedure.


API
===

We provide a variety of free functions in order to determine the tail coefficients.
Given a Matsubara or Real-frequency Green function :math:`G(i\omega)` with arbitrary
:code:`G.target_shape()` we have the following possibilities.

.. code-block:: c

   fit_tail(G) -> std::pair<array<dcomplex, Rank>, double>

* This function will consider twenty percent of the frequencies (the largest by absolute value) for the least square fit.
  The largest order :math:`N` for the fit is automatically determined based on the value of the largest frequency such that
   :math:`\|\omega_{max}\|^{1-N} > 10^{-16}`. The function will return a pair containing both the resulting high-frequency
  moment array and an error corresponding to the mean deviation from the data.

.. code-block:: c

   fit_tail(G, known_moments)

* We can provide additional information to the fitting routine by passing an array with a set of known moments.
  The shape of this array should be of the form :math:`(N + 1, i, j, ...)` where the :code:`target_shape` of G is given by :math:`(i, j, ...)`.

.. code-block:: c

   fit_hermitian_tail(G_iw, [known_moments])

* Matsubara Green functions fulfill the general relation :math:`G_{ij}(i\omega) = G_{ji}^*(-i\omega)`, which entails
  that the high-frequency coefficient matrices have to be hermitian. The :code:`fit_hermitian_tail` function will enforce this
  property, but will otherwise take the same arguments as :code:`fit_tail`.

.. note::

   For convenience we prodive a free function :code:`make_zero_tail(G, max_order)` that will,
   given the Green function G and the :code:`max_order`, generate a tail-array of the proper shape initialized to 0.

Example
~~~~~~~

.. literalinclude:: ./tail.cpp
   :language: cpp

Adjusting the fit parameters (Advanced)
---------------------------------------

The experienced user can adjust the parameters of the fitting procedure directly:

.. code-block:: c

   G.mesh().set_tail_fit_parameters(tail_fraction, n_tail_max, expansion_order)

The fitting parameters are

* :code:`tail_fraction` (double, default = 0.2)
  A value between 0 and 1.0 denoting the fraction of the frequency mesh
  (starting from the outer frequencies) to be considered for the fit.

* :code:`n_tail_max` (integer, default = 30)
  The maximum number of frequency points to be considered for the fit
  on both the positive and negative side.

* :code:`expansion_order` (integer, optional)
  This parameter will fix the largest order to be considered for the fit.

Adjusting the fit window (Advanced)
-----------------------------------

We provide a free function to allow fitting on an adjusted Matsubara frequency window ranging from indices n_min to n_max
(and [-n_max-1, -n_min-1] correspondingly)

.. code-block:: c

   fit_tail_on_window(G_iw, n_min, n_max, known_moments, n_tail_max) -> std::pair<array<dcomplex, R>, double>

The parameters :code:`known_moments` and :code:`n_tail_max` behave as described above. Note that a variant of this function called
:code:`fit_hermitian_tail_on_window` enforces hermiticity of the coefficient matrices.
