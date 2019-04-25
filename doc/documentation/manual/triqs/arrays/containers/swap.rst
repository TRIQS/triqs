
swap & deep_swap
==================================

There are two possible interpretation of "swapping two arrays": 
either use 3 moves like std::swap, i.e. swapping the pointer to the data in memory, 
or making a deep swap, i.e. really swapping all the elements in memeory.

.. _arr_swap:

swap 
--------

* swap just exchange the (shared) pointer to the data in memory, 
  it does not affect the data them self.
  This distinction of importance for views in particular.

* For the regular type, std::swap and swap are equivalent.
  For the views, std::swap is explicitely deleted, since it is incorrect
  due to the lack of move constructor for a view.
  Use `swap` instead.

* **Example** :

.. literalinclude:: ./swap_0.cpp
.. _arr_deep_swap:

deep_swap
--------------

.. warning::

   Currently implemented only for triqs::vector and triqs::vector_view.


* deep_swap swaps the data in memory.

* **Example** (compare with swap) :
      
.. literalinclude:: ./swap_1.cpp
