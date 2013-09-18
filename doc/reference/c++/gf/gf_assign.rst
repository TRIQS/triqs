
.. highlight:: c

.. _gf_reg_assign:

Assignment 
=========================


**Synopsis** :: 

    
    gf & operator=(gf const & X);                       (1)  
    
    template<???   RHS> gf & operator=(RHS const & X);  (2)

The container have a quite general assignment operator.

* (1) just make a copy of X into *\*this*.
 
* (2) **RHS** can be anything that models the :ref:`???` concept 
  

.. warning::

     In both cases, if needed, the container is resized. 
     
     Hence **assignment invalidates all pointers and views to it**.


Move assign operator
---------------------------

**Synopsis** :: 

    gf & operator=(gf && X);

Standard move assign operator.
Move the data of X as data of `*this`. 
