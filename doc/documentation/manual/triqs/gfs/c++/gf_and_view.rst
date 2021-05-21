.. highlight:: c

.. _gf_and_view:

gf and views 
==============

**Synopsis**:

.. code-block:: c

  template<typename Variable, typename Target=matrix_valued, typename Opt=void> class gf;      
  template<typename Variable, typename Target=matrix_valued, typename Opt=void> class gf_view; 
  template<typename Variable, typename Target=matrix_valued, typename Opt=void> class gf_const_view; 

Hereafter, we describe the containers, the views.
Properties specific to each :ref:`specializations<gf_special>` are described in a specific page, linked in the table below.

Template parameters
----------------------------

+-----------------------------------------+-------------------------------+-------------------------------+--------------------------------------+
| Template parameter                      | Accepted type                 | Access in the class           | Meaning                              |
+=========================================+===============================+===============================+======================================+
| Variable                                | Cf table below                | variable_t                    | The domain of definition of the      |
|                                         |                               |                               | Green function                       |
+-----------------------------------------+-------------------------------+-------------------------------+--------------------------------------+
| Target                                  | Cf Table below                | target_t                      | The target space of the function     |
+-----------------------------------------+-------------------------------+-------------------------------+--------------------------------------+
| Opt                                     | void                          | option_t                      | Currently unused                     |
+-----------------------------------------+-------------------------------+-------------------------------+--------------------------------------+


The various specializations of the container and its views are decribed in in the :ref:`specializations<gf_special>` page.

The *Variable* template parameter can take the following values: 

+--------------------------+--------------------------------------------+
| Variable                 | Meaning                                    |
+==========================+============================================+
| imfreq                   | Imaginary Matsubara frequency              |
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
| prod<Gs...> | Cartesian product of gf<Gs> ... functions. |
+--------------------------+--------------------------------------------+

The *Target* template parameter can take the following values: 
 
+-------------------------+-----------------------------------------------------+
| Target                  | Meaning                                             |
+=========================+=====================================================+
| scalar_valued           | The function is scalar valued (double, complex...). |
+-------------------------+-----------------------------------------------------+
| matrix_valued [default] | The function is matrix valued.                      |
+-------------------------+-----------------------------------------------------+
| tensor_valued<R>        | The function is tensor valued with rank             |
+-------------------------+-----------------------------------------------------+


Member types 
--------------------------------------

+----------------+-------------------------------------------------------------+
| Member type    | Definitions                                                 |
+================+=============================================================+
| view_type      | The corresponding view type                                 |
+----------------+-------------------------------------------------------------+
| regular_type   | The corresponding regular type i.e. the container itself    |
+----------------+-------------------------------------------------------------+
| mesh_t         | The mesh                                                    |
+----------------+-------------------------------------------------------------+
| target_shape_t | Type storing the information to construct the target space, |
|                | Depends on the specialisation (a shape for matrix_valued    |
|                | gf, empty for a scalar valued, ... Cf Specialisations)      |
+----------------+-------------------------------------------------------------+
| data_t         | Type of the data array                                      |
+----------------+-------------------------------------------------------------+
| singularity_t  | Type of the singularity (tail, nothing...)                  |
+----------------+-------------------------------------------------------------+
| symmetry_t     | Symmetry (unused at this stage).                            |
+----------------+-------------------------------------------------------------+

Member functions
---------------------

+-------------------------------------------+------------------------------------------+
| Member function                           | Meaning                                  |
+===========================================+==========================================+
| :ref:`(constructor)<gf_constr>`           |                                          |
+-------------------------------------------+------------------------------------------+
| (destructor)                              |                                          |
+-------------------------------------------+------------------------------------------+
| :ref:`operator ()<gf_call>`               | Evaluation on a point of the domain      |
+-------------------------------------------+------------------------------------------+
| :ref:`operator []<gf_subscript>`          | Access to the value on the mesh          |
+-------------------------------------------+------------------------------------------+
| :ref:`mesh<gf_data>`                      | Access to the mesh                       |
+-------------------------------------------+------------------------------------------+
| :ref:`singularity<gf_data>`               | Access to the singularity                |
+-------------------------------------------+------------------------------------------+
| :ref:`symmetry<gf_data>`                  | Access to the symmetry                   |
+-------------------------------------------+------------------------------------------+
| :ref:`data<gf_data>`                      | Direct view of the data [Advanced]       |
+-------------------------------------------+------------------------------------------+
| :ref:`operator =<gf_reg_assign>`          | Assigns values to the container          |
+-------------------------------------------+------------------------------------------+
| :ref:`operator +=,-=,*=,/=<gf_comp_ops>`  | compound assignment operators            |
+-------------------------------------------+------------------------------------------+


.. toctree::
   :hidden:

   gf_constructors
   gf_data
   gf_assign
   gf_call
   gf_subcript
   compound_ops
   call 
   resize
   STL 

Non-member functions
------------------------


+----------------------------------------------------------------------+------------------------------------------------------------+
| Member function                                                      | Meaning                                                    |
+======================================================================+============================================================+
| :ref:`swap<gf_swap>`                                                 | Swap of 2 containers                                       |
+----------------------------------------------------------------------+------------------------------------------------------------+
| :ref:`operator\<\<<gf_stream>`                                       | Writing to stream                                          |
+----------------------------------------------------------------------+------------------------------------------------------------+
| :ref:`reinterpret_scalar_valued_gf_as_matrix_valued<gf_reinterpret>` | Reinterpret a scalar_valued gf as a 1x1 matrix_valued view |
+----------------------------------------------------------------------+------------------------------------------------------------+


.. toctree::
   :hidden:

   stream
   swap
  
   

