..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _triqs__gfs__bgf_unary_m_expr:

triqs::gfs::bgf_unary_m_expr
============================

Defined in header <**>

.. code-block:: c

    template<typename L> class bgf_unary_m_expr

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

+------------+--------------------------+--+
| L_t        | std::decay_t<L>          |  |
+------------+--------------------------+--+
| variable_t | typename L_t::variable_t |  |
+------------+--------------------------+--+
| target_t   | typename L_t::target_t   |  |
+------------+--------------------------+--+


Member functions
----------------

+---------------------------------------------------+--+
| :ref:`constructor <bgf_unary_m_expr_constructor>` |  |
+---------------------------------------------------+--+
| :ref:`size <bgf_unary_m_expr_size>`               |  |
+---------------------------------------------------+--+
| :ref:`block_names <bgf_unary_m_expr_block_names>` |  |
+---------------------------------------------------+--+
| :ref:`operator[] <bgf_unary_m_expr_operator[]>`   |  |
+---------------------------------------------------+--+
| :ref:`operator() <bgf_unary_m_expr_operator()>`   |  |
+---------------------------------------------------+--+

.. toctree::
    :hidden:

    bgf_unary_m_expr/constructor
    bgf_unary_m_expr/size
    bgf_unary_m_expr/block_names
    bgf_unary_m_expr/operator[]
    bgf_unary_m_expr/operator()


Non Member functions
--------------------

+-----------------------------------------------------+--+
| :ref:`operator\<\< <bgf_unary_m_expr_operatorLTLT>` |  |
+-----------------------------------------------------+--+

.. toctree::
    :hidden:

    bgf_unary_m_expr/operator<<