.. highlight:: c

.. _gf_and_view:

gf and gf_view 
=================

gf is the Green function container, with its view gf_view,  defined as ::
  
  template<typename Variable, typename Target=matrix_valued, typename Opt=void> class gf;      // regular type
  template<typename Variable, typename Target=matrix_valued, typename Opt=void> class gf_view; // view type   

In this section, we describe all common properties of gf. 
In practice, one works with one of the specialization described below (:ref:`gf_special`), 
with their specific aspects (domains, interpolation methods, ....).
 

Template parameters
----------------------------

* **Variable** determines the domain of the Green function
* **Target** determines the value of the Green function
* **Opt** is currently not used [default to void]. It may be used to differentiate different Green functions
   with the same Variable, Target but different behaviour (e.g. interpolation, ...).

They can have the following values : 

+--------------------------+--------------------------------------------+
| Variable                 | Meaning                                    |
+==========================+============================================+
| gf_imfreq                | Imaginary Matsubara frequency              |
+--------------------------+--------------------------------------------+
| imtime                   | Imaginary Matsubara time                   |
+--------------------------+--------------------------------------------+
| refreq                   | Real frequency                             |
+--------------------------+--------------------------------------------+
| retime                   | Real time                                  |
+--------------------------+--------------------------------------------+
| legendre                 | Legendre polynomial representation         |
+--------------------------+--------------------------------------------+
| block_index              | Block Green function                       |
+--------------------------+--------------------------------------------+
| cartesian_product<Gs...> | Cartesian product of gf<Gs> ... functions. |
+--------------------------+--------------------------------------------+

and 

+-------------------------+-----------------------------------------------------+
| Target                  | Meaning                                             |
+=========================+=====================================================+
| matrix_valued [default] | The function is matrix valued.                      |
+-------------------------+-----------------------------------------------------+
| scalar_valued           | The function is scalar valued (double, complex...). |
+-------------------------+-----------------------------------------------------+

.. _gf_special:

Specializations
-------------------

+-----------------+------------------+--------------------+----------------------------+
| Variable\Target | matrix_valued    | scalar_valued      | G (another Green function) |
+=================+==================+====================+============================+
| imfreq          | :doc:`gf_imfreq` | :doc:`gf_imfreq_s` |                            |
+-----------------+------------------+--------------------+----------------------------+
| imtime          | :doc:`gf_imtime` | :doc:`gf_imtime_s` |                            |
+-----------------+------------------+--------------------+----------------------------+
| refreq          | :doc:`gf_refreq` | :doc:`gf_refreq_s` |                            |
+-----------------+------------------+--------------------+----------------------------+
| retime          | :doc:`gf_retime` | :doc:`gf_retime_s` |                            |
+-----------------+------------------+--------------------+----------------------------+
| block_index     |                  |                    | :doc:`block_gf<gf_block>`  |
+-----------------+------------------+--------------------+----------------------------+

.. toctree::
   :hidden:
   :maxdepth: 1

   gf_imfreq
   gf_imfreq_s
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


