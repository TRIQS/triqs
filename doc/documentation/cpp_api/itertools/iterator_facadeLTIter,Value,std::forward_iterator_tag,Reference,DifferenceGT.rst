..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param
.. role:: cppbrief


.. _itertools__iterator_facadeLTIter_Value_std__forward_iterator_tag_Reference_DifferenceGT:

itertools::iterator_facade<Iter,Value,std::forward_iterator_tag,Reference,Difference>
=====================================================================================

Defined in header <**>

.. code-block:: c

    template<typename Iter, typename Value, typename Reference, typename Difference> class iterator_facade<Iter,Value,std::forward_iterator_tag,Reference,Difference>

A helper for the implementation of forward iterators using CRTP




Template parameters
-------------------

 * **Iter**: The Iterator Class to be implemented
 `Iter` is required to have the following member functions
 - bool equal(Iter other)
 - value_type [const] [&] dereference()
 - void increment()


Member types
------------

+-------------------+---------------------------+--+
| value_type        | Value                     |  |
+-------------------+---------------------------+--+
| reference         | Reference                 |  |
+-------------------+---------------------------+--+
| pointer           | Value *                   |  |
+-------------------+---------------------------+--+
| difference_type   | Difference                |  |
+-------------------+---------------------------+--+
| iterator_category | std::forward_iterator_tag |  |
+-------------------+---------------------------+--+


Member functions
----------------

+---------------------------------------------------------------------------------------------------------------+--+
| :ref:`operator++ <iterator_facadeLTIter_Value_std__forward_iterator_tag_Reference_DifferenceGT_operator++>`   |  |
+---------------------------------------------------------------------------------------------------------------+--+
| :ref:`operator== <iterator_facadeLTIter_Value_std__forward_iterator_tag_Reference_DifferenceGT_operator==>`   |  |
+---------------------------------------------------------------------------------------------------------------+--+
| :ref:`operator!= <iterator_facadeLTIter_Value_std__forward_iterator_tag_Reference_DifferenceGT_operator!=>`   |  |
+---------------------------------------------------------------------------------------------------------------+--+
| :ref:`operator* <iterator_facadeLTIter_Value_std__forward_iterator_tag_Reference_DifferenceGT_operator*>`     |  |
+---------------------------------------------------------------------------------------------------------------+--+
| :ref:`operator-\> <iterator_facadeLTIter_Value_std__forward_iterator_tag_Reference_DifferenceGT_operator-GT>` |  |
+---------------------------------------------------------------------------------------------------------------+--+

.. toctree::
    :hidden:

    iterator_facadeLTIter,Value,std::forward_iterator_tag,Reference,DifferenceGT/operator++
    iterator_facadeLTIter,Value,std::forward_iterator_tag,Reference,DifferenceGT/operator==
    iterator_facadeLTIter,Value,std::forward_iterator_tag,Reference,DifferenceGT/operator!=
    iterator_facadeLTIter,Value,std::forward_iterator_tag,Reference,DifferenceGT/operator*
    iterator_facadeLTIter,Value,std::forward_iterator_tag,Reference,DifferenceGT/operator->