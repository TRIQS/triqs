.. highlight:: c

.. _gf_subscript:

Operator []
==================================

**Synopsis** ::

  value_type const & operator[](mesh_t::index_t const &) const      (1a)  
  value_type       & operator[](mesh_t::index_t const &)            (1b)  
  
  value_type const & operator[](closest_mesh_pt_t const &) const    (2a)  
  value_type       & operator[](closest_mesh_pt_t const &)          (2b)  
   
  ClefExpression  operator[](ClefExpression expr) const             (3) 

 
The operator [] is in charge of accessing the data of the Green function 
on the mesh. This is therefore a way to modify the Green function.


NB : This is valid for both the container (e.g. gf), and the view (e.g. gf_view).


.. _gf_subscript_index:

(1) Use the linear index of the mesh 
-----------------------------------------


.. _gf_subscript_closest:
.. 
    
(2) Find the closest mesh point from a domain point
---------------------------------------------------------

* May or may not be provided by the specialisation. Cf specialization.

* This is normally used with the helper function::

   closest_mesh_point_t closest_mesh_point(Anything x) 

  closest_mesh_point_t is just a little "vehicle" that can contain anything (by copy or ref ???), 
  and carry it through the [].
  
* Usage ::

     g [ closest_mesh_point(x) ] += y;

  
.. _gf_subscript_lazy:

(3) Interaction with clef expressions 
-------------------------------------------------

Like (), [] can be called with a ClefExpression.



  


