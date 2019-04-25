.. highlight:: c

.. _gf_clef:

Interaction with CLEF expressions 
============================================

The gf containers and their view classes can be used with the :doc:`../clef/contents` library: 

* They can be called with CLEF expressions.
* :doc:`Automatic assignment<../clef/assign>` has been set up.

Using the CLEF library offers a quick and efficient way to fill an array with multiple advantages: 

* It is simpler and more readeable than a series of for loops.
* It is more optimal since the loops are automatically written in the best order for memory traversal.

**Example**: 

.. literalinclude:: ./clef_0.cpp

.. note:: 

   The LHS uses () and not brackets, even though it is on the mesh, because of the strange C++ limitation 
   that [] cannot be overloaded for several variables...
