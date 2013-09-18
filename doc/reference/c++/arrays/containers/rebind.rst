.. highlight:: c

.. _arr_rebind:

rebind
==================================

* **Synopsis** for X_view (X=array, matrix, vector):

  .. cpp:function:: void rebind(array_view const & a) 

* **Synopsis** for X_const_view (X=array, matrix, vector):

  .. cpp:function:: void rebind(array_view const & a) 
  .. cpp:function:: void rebind(array_const_view const & a) 

* **Effect** : 

  Rebinds the view

  After this method, the view points to the same as *a*.
  Following the :ref:`memory management rules <arr_view_memory>`, if nothing else views the 
  data originally pointed to by the view, this data is destroyed.

 
