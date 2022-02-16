.. highlight:: c

.. _ising_solution:

Example: the Ising chain in a magnetic field
--------------------------------------------

Here is the a simple Monte-Carlo for a one-dimensional Ising chain.  The
problem is described in detail in this section about :ref:`the Ising model
<isingex>`.

The configuration
*****************

We start by defining a configuration class on which the move and measure
classes will act. We write this class in a file :file:`configuration.hpp`:

.. literalinclude:: /documentation/manual/triqs/mc_tools/configuration.hpp
    :language: cpp

The move
********

The move class should have three methods: `attempt()`, `accept()` and `reject()`:

.. literalinclude:: /documentation/manual/triqs/mc_tools/moves.hpp
    :language: cpp

Measure
*******

The measure class has two methods, `accumulate` and `collect_results`:

.. literalinclude:: /documentation/manual/triqs/mc_tools/measures.hpp
    :language: cpp


Main program
************

The Monte-Carlo itself can now be written:

.. literalinclude:: /documentation/manual/triqs/mc_tools/ising.cpp
    :language: cpp

.. literalinclude:: /documentation/manual/triqs/mc_tools/ising.output

