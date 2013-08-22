.. highlight:: c

.. _gf_and_view:

gf and gf_view 
=================

gf is the Green function container, with its view gf_view,  defined as ::
  
  template<typename Variable, typename Target=matrix_valued, typename Opt=void> class gf;         // the regular type
  template<typename Variable, typename Target=matrix_valued, typename Opt=void> class gf_view;    // the view type   

In this section, we describe all common properties of gf. 
In practice, one works with one of the specialization described below (:ref:`gf_special`), 
with their specific aspects (domains, interpolation methods, ....).


Template parameters
----------------------------

* Variable determines the domain of the Green function. It can be : 

 ============================   ====================================================================
 Variable tag                   Meaning                                    
 ============================   ====================================================================
 :ref:`imfreq<gf_imfreq>`       Imaginary Matsubara frequency
 imtime                         Imaginary Matsubara time     
 refreq                         Real frequency
 retime                         Real time     
 legendre                       Legendre polynomial representation
 block_index                    Block Green function 
 cartesian_product<Gs...>       Cartesian product of gf<Gs> ... functions.
 ============================   ====================================================================

* Target determines the value of the Green function

 ============================   ====================================================================
 Target tag                     Meaning                                    
 ============================   ====================================================================
 matrix_valued [default]        The function is matrix valued.
 scalar_valued                  The function is scalar valued (double, complex...).
 ============================   ====================================================================

* Opt is currently not used [default to void].

.. _gf_special:

Specializations
-------------------


.. toctree::
   :maxdepth: 1

   gf_imfreq
   gf_refreq
   gf_imtime
   gf_retime
   gf_block
   gf_prod

Construction/factories
-------------------------------

gf/gf_view have very basic constructors : 
default, copy, move, and one constructor from the data used by the functions (reserved for advanced users).

Various specializations however provides several factories, adapted to each case, of the form ::

  auto g= make_gf<Variable, Target, Opt> ( ....) ;

This is the recommended way to construct `gf` objects.
Cf examples in various specializations.


Member types
-----------------



Member functions
---------------------


Non-member functions
------------------------


