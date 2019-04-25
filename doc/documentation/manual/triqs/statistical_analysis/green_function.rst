.. highlight:: c

A practical example: computing the error bar of a Green's function
===================================================================

In the following example, we construct a series of noisy imaginary-time Green's functions and compute the associated averages and error bars.

.. warning::

  So far, the interface of the statistical analysis tools is rudimentary. It may evolve to allow users to compute the error bar of a stack of Green's functions directly.

.. literalinclude:: ./green_function_0.cpp

