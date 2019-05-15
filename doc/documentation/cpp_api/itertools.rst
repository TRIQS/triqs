Reference C++ API for itertools
###############################



Classes
-------

.. table::
   :width: 50% 50%

+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+-----------------------------------------------------------------+
| :ref:`iterator_facade\<Iter,Value,std::forward_iterator_tag,Reference,Difference\> <itertools__iterator_facadeLTIter_Value_std__forward_iterator_tag_Reference_DifferenceGT>` | A helper for the implementation of forward iterators using CRTP |
+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+-----------------------------------------------------------------+
| :ref:`range <itertools__range>`                                                                                                                                               | A range of integer indices that mimics the Python `range`.      |
+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+-----------------------------------------------------------------+

.. toctree::
    :hidden:

    itertools/iterator_facadeLTIter,Value,std::forward_iterator_tag,Reference,DifferenceGT
    itertools/range


Functions
---------

+--------------------------------------+---------------------------------------------------------------------------------------+
| :ref:`transform <transform>`         | Transform (lazy)applies a unary lambda function to every                              |
+--------------------------------------+---------------------------------------------------------------------------------------+
| :ref:`enumerate <enumerate>`         | Lazy-enumerate a range.                                                               |
+--------------------------------------+---------------------------------------------------------------------------------------+
| :ref:`zip <zip>`                     | Lazy-zip a range.                                                                     |
+--------------------------------------+---------------------------------------------------------------------------------------+
| :ref:`product <product>`             | Lazy-product of multiple ranges. This function returns itself a range of tuple<T...>. |
+--------------------------------------+---------------------------------------------------------------------------------------+
| :ref:`slice <slice>`                 | Lazy-slice a range.                                                                   |
+--------------------------------------+---------------------------------------------------------------------------------------+
| :ref:`stride <stride>`               | Lazy-stride a range.                                                                  |
+--------------------------------------+---------------------------------------------------------------------------------------+
| :ref:`product_range <product_range>` | A product of an arbitrary number of integer ranges                                    |
+--------------------------------------+---------------------------------------------------------------------------------------+
| :ref:`chunk_range <chunk_range>`     | Given an integer range [start, end), chunk it as equally as possible into n_chunks.   |
+--------------------------------------+---------------------------------------------------------------------------------------+
| :ref:`foreach <foreach>`             | Apply a function f to every element of an integer range                               |
+--------------------------------------+---------------------------------------------------------------------------------------+

.. toctree::
    :hidden:

    itertools/transform
    itertools/enumerate
    itertools/zip
    itertools/product
    itertools/slice
    itertools/stride
    itertools/product_range
    itertools/chunk_range
    itertools/foreach