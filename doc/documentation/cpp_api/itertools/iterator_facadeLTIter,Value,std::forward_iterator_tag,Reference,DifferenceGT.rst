..
   Generated automatically by cpp2rst

.. highlight:: c
.. role:: red
.. role:: green
.. role:: param


.. _itertools__iterator_facadeLTIter_Value_std__forward_iterator_tag_Reference_DifferenceGT:

itertools::iterator_facade<Iter,Value,std::forward_iterator_tag,Reference,Difference>
=====================================================================================

*#include <itertools.hpp>*

.. rst-class:: cppsynopsis

     template<typename Iter, typename Value, typename Reference, typename Difference> class  :red:`iterator_facade<Iter,Value,std::forward_iterator_tag,Reference,Difference>`

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