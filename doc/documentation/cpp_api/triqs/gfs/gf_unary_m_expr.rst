..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__gfs__gf_unary_m_expr:

triqs::gfs::gf_unary_m_expr
===========================

Defined in header <**>

.. code-block:: c

    template<typename L> class gf_unary_m_expr

-------------------------------------------------------------------

     a special case : the unary operator !
         a special case : the unary operator !

Public members
--------------

+---+---+--+
| l | L |  |
+---+---+--+


Member types
------------

+------------+-----------------------------------------+--+
| L_t        | typename std::remove_reference<L>::type |  |
+------------+-----------------------------------------+--+
| variable_t | typename L_t::variable_t                |  |
+------------+-----------------------------------------+--+
| target_t   | typename L_t::target_t                  |  |
+------------+-----------------------------------------+--+


Member functions
----------------

+--------------------------------------------------+--+
| :ref:`constructor <gf_unary_m_expr_constructor>` |  |
+--------------------------------------------------+--+
| :ref:`mesh <gf_unary_m_expr_mesh>`               |  |
+--------------------------------------------------+--+
| :ref:`data_shape <gf_unary_m_expr_data_shape>`   |  |
+--------------------------------------------------+--+
| :ref:`indices <gf_unary_m_expr_indices>`         |  |
+--------------------------------------------------+--+
| :ref:`operator[] <gf_unary_m_expr_operator[]>`   |  |
+--------------------------------------------------+--+
| :ref:`operator() <gf_unary_m_expr_operator()>`   |  |
+--------------------------------------------------+--+

.. toctree::
    :hidden:

    gf_unary_m_expr/constructor
    gf_unary_m_expr/mesh
    gf_unary_m_expr/data_shape
    gf_unary_m_expr/indices
    gf_unary_m_expr/operator[]
    gf_unary_m_expr/operator()


Non Member functions
--------------------

+----------------------------------------------------+--+
| :ref:`operator\<\< <gf_unary_m_expr_operatorLTLT>` |  |
+----------------------------------------------------+--+

.. toctree::
    :hidden:

    gf_unary_m_expr/operator<<