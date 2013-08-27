.. highlight:: c

.. _arr_reg_assign:

Assignment 
=========================


**Synopsis** :: 

    
    array & operator=(array const & X);                       (1)  
    
    template<ImmutableCuboidArray RHS> array & operator=(RHS const & X);  (2)

The container have a quite general assignment operator.
We will illustrate it on the `array` class, it is the same for `matrix` and `vector`.

* (1) just make a copy of X. 
 
* (2) **RHS** can be anything that models the :ref:`ImmutableCuboidArray` concept 
  
      e.g. : array, array_view, matrix, matrix_view, 
      but also formal expression (See , e.g. A+B), or any custom object of your choice.

      `Effect` : all the elements viewed by the view are replaced
      by the evaluation of RHS.

.. warning::

     In both cases, if needed, the container is resized. 
     
     Hence **assignment invalidates all pointers and views to it**.


Move assign operator
---------------------------

**Synopsis** :: 

    array & operator=(array && X);

Standard move assign operator.
Move the data of X as data of `*this`. 
