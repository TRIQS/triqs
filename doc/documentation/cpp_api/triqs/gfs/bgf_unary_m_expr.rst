..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__gfs__bgf_unary_m_expr:

triqs::gfs::bgf_unary_m_expr
============================

*#include <triqs/gfs.hpp>*

.. rst-class:: cppsynopsis

     template<typename L> class  :red:`bgf_unary_m_expr`

-------------------------------------------------------------------
   // a special case : the unary operator !


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

+------------------------------------------------------------------+--+
| :ref:`(constructor) <triqs__gfs__bgf_unary_m_expr__constructor>` |  |
+------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    bgf_unary_m_expr/constructor