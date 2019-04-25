.. highlight:: c

.. _arr_play_concept:

.. warning::

   This part is alpha: work in progress. May change at any time.

Using basic concepts
###########################################


make_immutable_array
-------------------------------

**Synopsis**::

  template<typename Expr, int ... ph> auto make_immutable_array(Expr const &, clef::pair<ph,range> ...);  (1)
  template<typename Function>         auto make_immutable_array(Function, range ...);                     (2)

* (1) 
  From a clef expression, and a range for each placeholder in the expression, build
  a lazy object modelling :ref:`ImmutableCuboidArray` concept, with the domain built from the ranges: 
  
  Parameters are :

    * expr The lazy expression
    * i_=R : `i_` is a placeholder, R a range. The `i_=R` produce a clef::pair of `i_` and R , which is the parameter.

* (2) 
  From a function object and a set of range, build 
  a lazy object modelling :ref:`ImmutableCuboidArray` concept, with the domain built from the ranges: 


**Example**: 

.. triqs_example:: ./play_with_concept_0.cpp
