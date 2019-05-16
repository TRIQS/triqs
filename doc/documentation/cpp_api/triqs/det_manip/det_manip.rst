..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _triqs__det_manip__det_manip:

triqs::det_manip::det_manip
===========================

*#include <triqs/det_manip.hpp>*

.. rst-class:: cppsynopsis

     template<typename FunctionType> class  :red:`det_manip`




Member types
------------

+------------------------+--------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| x_type                 | typename f_tr::template f_tr::decay_arg<0>::type |                                                                                                                                                                                                    |
+------------------------+--------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| y_type                 | typename f_tr::template f_tr::decay_arg<1>::type |                                                                                                                                                                                                    |
+------------------------+--------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| value_type             | typename f_tr::result_type                       |                                                                                                                                                                                                    |
+------------------------+--------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| det_type               | det_manip::value_type                            |  options the det could be kept in a long double to minimize overflow
      //using det_type = std::conditional_t<std::is_same<value_type, double>::value, long double, std::complex<long double>>; |
+------------------------+--------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| vector_type            | arrays::vector<value_type>                       |                                                                                                                                                                                                    |
+------------------------+--------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| matrix_type            | arrays::matrix<value_type>                       |                                                                                                                                                                                                    |
+------------------------+--------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| matrix_view_type       | arrays::matrix_view<value_type>                  |                                                                                                                                                                                                    |
+------------------------+--------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| matrix_const_view_type | arrays::matrix_const_view<value_type>            |                                                                                                                                                                                                    |
+------------------------+--------------------------------------------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+


Member functions
----------------

+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`(constructor) <triqs__det_manip__det_manip__constructor>`                                   | Constructor. |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`reserve <triqs__det_manip__det_manip__reserve>`                                             |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`get_singular_threshold <triqs__det_manip__det_manip__get_singular_threshold>`               |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`set_singular_threshold <triqs__det_manip__det_manip__set_singular_threshold>`               |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`get_n_operations_before_check <triqs__det_manip__det_manip__get_n_operations_before_check>` |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`set_n_operations_before_check <triqs__det_manip__det_manip__set_n_operations_before_check>` |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`get_precision_warning <triqs__det_manip__det_manip__get_precision_warning>`                 |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`set_precision_warning <triqs__det_manip__det_manip__set_precision_warning>`                 |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`get_precision_error <triqs__det_manip__det_manip__get_precision_error>`                     |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`set_precision_error <triqs__det_manip__det_manip__set_precision_error>`                     |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`operator= <triqs__det_manip__det_manip__operator=>`                                         |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`clear <triqs__det_manip__det_manip__clear>`                                                 |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`size <triqs__det_manip__det_manip__size>`                                                   |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`get_x <triqs__det_manip__det_manip__get_x>`                                                 |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`get_y <triqs__det_manip__det_manip__get_y>`                                                 |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`get_x_internal_order <triqs__det_manip__det_manip__get_x_internal_order>`                   |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`get_y_internal_order <triqs__det_manip__det_manip__get_y_internal_order>`                   |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`get_function <triqs__det_manip__det_manip__get_function>`                                   |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`determinant <triqs__det_manip__det_manip__determinant>`                                     |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`inverse_matrix <triqs__det_manip__det_manip__inverse_matrix>`                               |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`inverse_matrix_internal_order <triqs__det_manip__det_manip__inverse_matrix_internal_order>` |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`matrix <triqs__det_manip__det_manip__matrix>`                                               |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`try_insert_from_function <triqs__det_manip__det_manip__try_insert_from_function>`           |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`complete_insert <triqs__det_manip__det_manip__complete_insert>`                             |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`complete_insert2 <triqs__det_manip__det_manip__complete_insert2>`                           |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`try_remove <triqs__det_manip__det_manip__try_remove>`                                       |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`complete_remove <triqs__det_manip__det_manip__complete_remove>`                             |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`try_remove2 <triqs__det_manip__det_manip__try_remove2>`                                     |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`complete_remove2 <triqs__det_manip__det_manip__complete_remove2>`                           |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`try_change_col <triqs__det_manip__det_manip__try_change_col>`                               |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`complete_change_col <triqs__det_manip__det_manip__complete_change_col>`                     |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`try_change_row <triqs__det_manip__det_manip__try_change_row>`                               |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`complete_change_row <triqs__det_manip__det_manip__complete_change_row>`                     |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`try_change_col_row <triqs__det_manip__det_manip__try_change_col_row>`                       |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`complete_change_col_row <triqs__det_manip__det_manip__complete_change_col_row>`             |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`try_refill <triqs__det_manip__det_manip__try_refill>`                                       |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`complete_refill <triqs__det_manip__det_manip__complete_refill>`                             |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`_regenerate_with_check <triqs__det_manip__det_manip___regenerate_with_check>`               |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`is_singular <triqs__det_manip__det_manip__is_singular>`                                     |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`regenerate <triqs__det_manip__det_manip__regenerate>`                                       |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`complete_operation <triqs__det_manip__det_manip__complete_operation>`                       |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`reject_last_try <triqs__det_manip__det_manip__reject_last_try>`                             |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`insert <triqs__det_manip__det_manip__insert>`                                               |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`insert_at_end <triqs__det_manip__det_manip__insert_at_end>`                                 |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`insert2 <triqs__det_manip__det_manip__insert2>`                                             |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`insert2_at_end <triqs__det_manip__det_manip__insert2_at_end>`                               |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`remove <triqs__det_manip__det_manip__remove>`                                               |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`remove_at_end <triqs__det_manip__det_manip__remove_at_end>`                                 |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`remove2 <triqs__det_manip__det_manip__remove2>`                                             |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`remove2_at_end <triqs__det_manip__det_manip__remove2_at_end>`                               |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`change_col <triqs__det_manip__det_manip__change_col>`                                       |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`change_row <triqs__det_manip__det_manip__change_row>`                                       |              |
+---------------------------------------------------------------------------------------------------+--------------+
| :ref:`roll_matrix <triqs__det_manip__det_manip__roll_matrix>`                                     |              |
+---------------------------------------------------------------------------------------------------+--------------+


Operations
~~~~~~~~~~

+---------------------------------------------------------------+--+
| :ref:`try_insert <triqs__det_manip__det_manip__try_insert>`   |  |
+---------------------------------------------------------------+--+
| :ref:`try_insert2 <triqs__det_manip__det_manip__try_insert2>` |  |
+---------------------------------------------------------------+--+

.. toctree::
    :hidden:

    det_manip/constructor
    det_manip/reserve
    det_manip/get_singular_threshold
    det_manip/set_singular_threshold
    det_manip/get_n_operations_before_check
    det_manip/set_n_operations_before_check
    det_manip/get_precision_warning
    det_manip/set_precision_warning
    det_manip/get_precision_error
    det_manip/set_precision_error
    det_manip/operator=
    det_manip/clear
    det_manip/size
    det_manip/get_x
    det_manip/get_y
    det_manip/get_x_internal_order
    det_manip/get_y_internal_order
    det_manip/get_function
    det_manip/determinant
    det_manip/inverse_matrix
    det_manip/inverse_matrix_internal_order
    det_manip/matrix
    det_manip/try_insert
    det_manip/try_insert_from_function
    det_manip/complete_insert
    det_manip/try_insert2
    det_manip/complete_insert2
    det_manip/try_remove
    det_manip/complete_remove
    det_manip/try_remove2
    det_manip/complete_remove2
    det_manip/try_change_col
    det_manip/complete_change_col
    det_manip/try_change_row
    det_manip/complete_change_row
    det_manip/try_change_col_row
    det_manip/complete_change_col_row
    det_manip/try_refill
    det_manip/complete_refill
    det_manip/_regenerate_with_check
    det_manip/is_singular
    det_manip/regenerate
    det_manip/complete_operation
    det_manip/reject_last_try
    det_manip/insert
    det_manip/insert_at_end
    det_manip/insert2
    det_manip/insert2_at_end
    det_manip/remove
    det_manip/remove_at_end
    det_manip/remove2
    det_manip/remove2_at_end
    det_manip/change_col
    det_manip/change_row
    det_manip/roll_matrix


Non Member functions
--------------------

+---------------------------------------------------------+--+
| :ref:`boost <triqs__det_manip__det_manip__boost>`       |  |
+---------------------------------------------------------+--+
| :ref:`h5_write <triqs__det_manip__det_manip__h5_write>` |  |
+---------------------------------------------------------+--+
| :ref:`h5_read <triqs__det_manip__det_manip__h5_read>`   |  |
+---------------------------------------------------------+--+
| :ref:`foreach <triqs__det_manip__det_manip__foreach>`   |  |
+---------------------------------------------------------+--+

.. toctree::
    :hidden:

    det_manip/boost
    det_manip/h5_write
    det_manip/h5_read
    det_manip/foreach