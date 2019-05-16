..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__mc_tools__random_generator:

triqs::mc_tools::random_generator
=================================

*#include <triqs/mc_tools.hpp>*

.. rst-class:: cppsynopsis

      class  :red:`random_generator`

Random generator, adapting the boost random generator.

The name of the generator is given at construction, and its type is erased in this class.
For performance, the call to the generator is bufferized, with chunks of 1000 numbers.


Member functions
----------------

+-----------------------------------------------------------------------+--+
| :ref:`(constructor) <triqs__mc_tools__random_generator__constructor>` |  |
+-----------------------------------------------------------------------+--+
| :ref:`operator= <triqs__mc_tools__random_generator__operator=>`       |  |
+-----------------------------------------------------------------------+--+
| :ref:`name <triqs__mc_tools__random_generator__name>`                 |  |
+-----------------------------------------------------------------------+--+
| :ref:`operator() <triqs__mc_tools__random_generator__operator()>`     |  |
+-----------------------------------------------------------------------+--+
| :ref:`preview <triqs__mc_tools__random_generator__preview>`           |  |
+-----------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    random_generator/constructor
    random_generator/operator=
    random_generator/name
    random_generator/operator()
    random_generator/preview