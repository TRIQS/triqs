High-Frequency moments of the Green's function
##############################################

The coefficients of the high-frequency expansion of a Green function
(in short tail) play an essential role in acquiring accurate results
for e.g. Fourier transforms and density calculations.

.. math::

   G(i\omega) \approx \sum_{n = 0}^N \frac{a_n}{(i\omega)^n}
   
The TRIQS library provides functionalities to determine these tail coefficiencts
through a least-square fitting procedure.

Python API
==========

In Python, the :code:`fit_tail` functions are not free but are rather member functions of the respective
Green functions.

.. code-block:: python

   G.fit_tail([known_moments])

The functionality is otherwise identical to :ref:`C++ <gf_tail>`.
