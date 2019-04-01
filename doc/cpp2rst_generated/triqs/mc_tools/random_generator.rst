..
   Generated automatically by cpp2rst

.. highlight:: c


.. _random_generator:

random_generator
==================

Random generator, adapting the boost random generator.


 The name of the generator is given at construction, and its type is erased in this class.
 For performance, the call to the generator is bufferized, with chunks of 1000 numbers.

**Synopsis**:

.. code-block:: c

    class random_generator;

 The name of the generator is given at construction, and its type is erased in this class.
 For performance, the call to the generator is bufferized, with chunks of 1000 numbers.

Member functions
----------------

+---------------------------------------------------+-----------------------------------------------------+
| Member function                                   | Comment                                             |
+===================================================+=====================================================+
| :ref:`operator= <random_generator_operator=>`     |                                                     |
+---------------------------------------------------+-----------------------------------------------------+
| :ref:`name <random_generator_name>`               | Name of the random generator                        |
+---------------------------------------------------+-----------------------------------------------------+
| :ref:`operator() <random_generator_operator()>`   | Returns a integer in [0,i-1] with flat distribution |
+---------------------------------------------------+-----------------------------------------------------+
| :ref:`preview <random_generator_preview>`         | Returns a double in [0,1[ with flat distribution    |
+---------------------------------------------------+-----------------------------------------------------+
| :ref:`constructor <random_generator_constructor>` | Constructor                                         |
+---------------------------------------------------+-----------------------------------------------------+

.. toctree::
    :hidden:

    random_generator/operator=
    random_generator/name
    random_generator/operator()
    random_generator/preview
    random_generator/constructor